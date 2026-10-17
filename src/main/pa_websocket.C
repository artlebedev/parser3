/** @file
	Parser: WebSocket (RFC 6455) support: handshake and frames.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#include "pa_websocket.h"
#include "pa_base64.h"
#include "pa_sha1.h"
#include "../lib/pcre/pa_pcre_internal.h"

volatile const char * IDENT_PA_WEBSOCKET_C="$Id: pa_websocket.C,v 1.3 2026/10/17 22:58:09 moko Exp $" IDENT_PA_WEBSOCKET_H;

#define MAX_CONTROL_PAYLOAD 125
#define MIN_READ_SIZE 4096

/* ********************************************************** */

static bool has_token(const char *list, const char *token) {
	size_t token_length=strlen(token);
	while(*list){
		while(*list==' ' || *list=='\t' || *list==',')
			list++;
		const char *end=list;
		while(*end && !(*end==' ' || *end=='\t' || *end==','))
			end++;
		if((size_t)(end-list)==token_length && !strncasecmp(list, token, token_length))
			return true;
		list=end;
	}
	return false;
}

const char* pa_websocket_accept_key(const char* method, const char* upgrade, const char* connection, const char* key, const char* version) {
	if(!method || strcmp(method, "GET"))
		throw Exception("websocket.handshake", 0, "request method must be GET");
	if(!upgrade || !has_token(upgrade, "websocket"))
		throw Exception("websocket.handshake", 0, "Upgrade header must be websocket");
	if(!connection || !has_token(connection, "upgrade"))
		throw Exception("websocket.handshake", 0, "Connection header must be Upgrade");
	if(!version || strcmp(version, "13"))
		throw Exception("websocket.handshake", 0, "Sec-WebSocket-Version must be 13");

	// 16 bytes in base64
	char *decoded;
	if(!key || strlen(key)!=24 || key[22]!='=' || key[23]!='=' || pa_base64_decode(key, 24, decoded, Base64Options(false))!=16)
		throw Exception("websocket.handshake", 0, "Sec-WebSocket-Key must be base64 encoded 16 bytes");

	const char *guid="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	PA_SHA1_CTX c;
	unsigned char digest[20];
	pa_SHA1Reset(&c);
	pa_SHA1Input(&c, (const unsigned char*)key, 24);
	pa_SHA1Input(&c, (const unsigned char*)guid, strlen(guid));
	pa_SHA1ReadDigest(digest, &c);

	return pa_base64_encode((const char*)digest, sizeof(digest), Base64Options(false));
}

/* ********************************************************** */

// text messages and the reason of Close must be UTF-8 (RFC 3629)
static bool valid_utf8(const char *data, size_t size) {
	int offset;
	return !pa_pcre_valid_utf((unsigned char *)data, size, &offset);
}

void WebSocket::Buffer::fit(size_t room) {
	if(pos==size)
		pos=size=0;
	if(allocated-size>=room)
		return;

	if(pos){ // moving the unread bytes to the start
		memmove(data, data+pos, size-pos);
		size-=pos;
		pos=0;
		if(allocated-size>=room)
			return;
	}
	allocated=max(allocated+allocated/4, size+room);
	data=data ? (char *)pa_realloc(data, allocated) : (char *)pa_malloc_atomic(allocated);
}

void WebSocket::Buffer::append(const char *bytes, size_t length) {
	if(!length)
		return;
	fit(length);
	memcpy(data+size, bytes, length);
	size+=length;
}

void WebSocket::Message::set(Opcode aopcode, const char *adata, size_t asize, int acode) {
	opcode=aopcode;
	data=adata;
	size=asize;
	code=acode;
}

bool WebSocket::valid_status(int code) {
	return (code>=1000 && code<=1003) || (code>=1007 && code<=1014) || (code>=3000 && code<=4999);
}

WebSocket::WebSocket(HTTPD_Connection &aconnection) :
	accepted(false), closed(false),
	connection(aconnection), max_message_size(0),
	need(0), eof(false),
	msg_opcode(0),
	close_sent(false), error_code(0), error_reason("") {}

void WebSocket::start(size_t amax_message_size) {
	max_message_size=amax_message_size;
	accepted=true;

	// the frames of the client can follow the request headers at once
	String::C data=connection.unprocessed();
	rbuf.append(data.str, data.length);

	connection.set_nodelay();
	connection.set_send_timeout(pa_httpd_timeout);
}

bool WebSocket::fill(int timeout_ms) {
	int ready;
	while((ready=pa_sock_ready(connection.sock, false, timeout_ms))<0) {
		int no=pa_socks_errno();
		if(pa_socks_is_interrupted(no))
			continue;
		throw Exception("httpd.read", 0, "error waiting for data: %s (%d)", pa_socks_strerr(no), no);
	}
	if(!ready)
		return false;

	size_t room=need>rbuf.unread() ? need-rbuf.unread() : 0;
	rbuf.fit(max(room, (size_t)MIN_READ_SIZE));

	size_t received=connection.receive(rbuf.data+rbuf.size, rbuf.allocated-rbuf.size);
	if(received)
		rbuf.size+=received;
	else
		eof=true;
	return true;
}

WebSocket::Parsed WebSocket::invalid(int code, const char *reason) {
	error_code=code;
	error_reason=reason;
	return INVALID;
}

WebSocket::Parsed WebSocket::parse(Frame &frame) {
	const unsigned char *p=(const unsigned char *)rbuf.data+rbuf.pos;
	size_t available=rbuf.unread();
	need=0;

	if(available<2)
		return INCOMPLETE;

	bool fin=(p[0] & 0x80)!=0;
	int opcode=p[0] & 0x0F;
	bool masked=(p[1] & 0x80)!=0;
	uint64_t length=p[1] & 0x7F;
	size_t header=2;

	if(p[0] & 0x70)
		return invalid(PROTOCOL_ERROR, "reserved bits are set");
	if(!masked)
		return invalid(PROTOCOL_ERROR, "frame of the client is not masked");
	if(opcode!=0 && opcode!=TEXT && opcode!=BINARY && opcode!=CLOSE && opcode!=PING && opcode!=PONG)
		return invalid(PROTOCOL_ERROR, "unknown opcode");

	if(length==126){
		header+=2;
		if(available<header)
			return INCOMPLETE;
		length=((uint64_t)p[2]<<8) | p[3];
		if(length<126)
			return invalid(PROTOCOL_ERROR, "length is not minimal");
	} else if(length==127){
		header+=8;
		if(available<header)
			return INCOMPLETE;
		length=0;
		for(int i=0; i<8; i++)
			length=(length<<8) | p[2+i];
		if(length>>63)
			return invalid(PROTOCOL_ERROR, "length is too big");
		if(length<=0xFFFF)
			return invalid(PROTOCOL_ERROR, "length is not minimal");
	}

	if(opcode & 0x08){ // control frame
		if(!fin || length>MAX_CONTROL_PAYLOAD)
			return invalid(PROTOCOL_ERROR, "control frame is fragmented or too long");
	}

	if(length>max_message_size)
		return invalid(TOO_BIG, "message is too big");

	const unsigned char *key=p+header;
	header+=4;
	if(available<header+length){
		need=header+(size_t)length;
		return INCOMPLETE;
	}

	char *payload=rbuf.data+rbuf.pos+header;
	for(size_t i=0; i<(size_t)length; i++)
		payload[i]^=key[i & 3];

	frame.fin=fin;
	frame.opcode=opcode;
	frame.payload=payload;
	frame.size=(size_t)length;
	rbuf.pos+=header+(size_t)length;
	return PARSED;
}

bool WebSocket::failed(int code, const char *reason, bool notify, Message &message) {
	if(notify){
		try {
			send_close(code, reason, strlen(reason));
		} catch(const Exception&) {
			// the connection is broken as well
		}
	}
	closed=true;
	msg_opcode=0;
	msg.size=0;

	message.set(CLOSE, reason, strlen(reason), code);
	return true;
}

bool WebSocket::receive(int timeout_ms, Message &message) {
	if(closed)
		throw Exception("websocket.closed", 0, "connection is closed");

	for(;;){
		Frame frame;
		switch(parse(frame)){
			case INCOMPLETE:
				if(!fill(timeout_ms))
					return false;
				if(eof)
					return failed(ABNORMAL, "connection is closed by the client", false, message);
				continue;
			case INVALID:
				return failed(error_code, error_reason, true, message);
			case PARSED:
				break;
		}

		switch(frame.opcode){
			case PING:
				write_frame(PONG, frame.payload, frame.size);
				continue;

			case PONG:
				continue;

			case CLOSE: {
				const unsigned char *payload=(const unsigned char *)frame.payload;
				int code=NO_STATUS;
				const char *reason="";
				size_t reason_size=0;

				if(frame.size==1)
					return failed(PROTOCOL_ERROR, "close frame is too short", true, message);

				if(frame.size>=2){
					code=(payload[0]<<8) | payload[1];
					reason=frame.payload+2;
					reason_size=frame.size-2;
					if(!valid_status(code))
						return failed(PROTOCOL_ERROR, "invalid status of close frame", true, message);
					if(!valid_utf8(reason, reason_size))
						return failed(INVALID_DATA, "invalid UTF-8 in close frame", true, message);
				}

				if(!close_sent)
					send_close(code, reason, reason_size);
				closed=true;

				message.set(CLOSE, reason, reason_size, code);
				return true;
			}
		}

		// TEXT, BINARY or continuation
		if(frame.opcode==0){
			if(!msg_opcode)
				return failed(PROTOCOL_ERROR, "unexpected continuation frame", true, message);
		} else if(msg_opcode)
			return failed(PROTOCOL_ERROR, "new message before the end of the fragmented one", true, message);

		if(frame.opcode && frame.fin){ // the message in one frame
			message.set((Opcode)frame.opcode, frame.payload, frame.size);
		} else {
			if(frame.opcode){
				msg_opcode=frame.opcode;
				msg.size=0;
			}
			if(msg.size+frame.size>max_message_size)
				return failed(TOO_BIG, "message is too big", true, message);
			msg.append(frame.payload, frame.size);
			if(!frame.fin)
				continue;
			message.set((Opcode)msg_opcode, msg.data, msg.size);
			msg_opcode=0;
		}

		if(message.opcode==TEXT && !valid_utf8(message.data, message.size))
			return failed(INVALID_DATA, "invalid UTF-8 in text message", true, message);
		return true;
	}
}

void WebSocket::write_frame(int opcode, const char *data, size_t size) {
	unsigned char header[10];
	size_t header_size=0;

	header[header_size++]=(unsigned char)(0x80 | opcode);
	if(size<126){
		header[header_size++]=(unsigned char)size;
	} else if(size<=0xFFFF){
		header[header_size++]=126;
		header[header_size++]=(unsigned char)(size>>8);
		header[header_size++]=(unsigned char)size;
	} else {
		header[header_size++]=127;
		uint64_t length=size;
		for(int i=7; i>=0; i--)
			header[header_size++]=(unsigned char)(length>>(i*8));
	}

	// the header and the payload go in one packet
	char *frame=(char *)pa_malloc_atomic(header_size+size);
	memcpy(frame, header, header_size);
	if(size)
		memcpy(frame+header_size, data, size);
	connection.send_body(frame, header_size+size);
}

void WebSocket::send(Opcode opcode, const char *data, size_t size) {
	if(closed || close_sent)
		throw Exception("websocket.closed", 0, "connection is closed");
	write_frame(opcode, data, size);
}

void WebSocket::send_close(int code, const char *reason, size_t size) {
	close_sent=true;
	if(code==NO_STATUS){
		write_frame(CLOSE, 0, 0);
		return;
	}
	if(size>MAX_CONTROL_PAYLOAD-2)
		size=MAX_CONTROL_PAYLOAD-2;
	char payload[MAX_CONTROL_PAYLOAD];
	payload[0]=(char)(code>>8);
	payload[1]=(char)code;
	memcpy(payload+2, reason, size);
	write_frame(CLOSE, payload, size+2);
}

void WebSocket::close(int code, const char *reason, size_t reason_size) {
	if(closed)
		return;
	if(!close_sent)
		send_close(code, reason, reason_size);

	// the closing handshake: waiting for Close of the client as long as for the request, the rest is ignored
	for(;;){
		Frame frame;
		Parsed parsed;
		while((parsed=parse(frame))==PARSED)
			if(frame.opcode==CLOSE){
				closed=true;
				return;
			}
		if(parsed==INVALID || !fill(min(pa_httpd_timeout, 60)*1000) || eof)
			break;
	}
	closed=true;
}

WebSocket::~WebSocket() {
	if(accepted && !closed && !close_sent){
		try {
			send_close(INTERNAL_ERROR, "", 0);
		} catch(const Exception&) {
			// the connection is broken as well
		}
	}
}
