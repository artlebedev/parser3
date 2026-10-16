/** @file
	Parser: WebSocket (RFC 6455) support: handshake and frames.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_WEBSOCKET_H
#define PA_WEBSOCKET_H

#define IDENT_PA_WEBSOCKET_H "$Id: pa_websocket.h,v 1.1 2026/10/16 13:55:16 moko Exp $"

#include "pa_http.h"

/// checks the request of the handshake, returns the value of the Sec-WebSocket-Accept header, throws websocket.handshake
const char* pa_websocket_accept_key(const char* method, const char* upgrade, const char* connection, const char* key, const char* version);

/// messages are received and sent one at a time
class WebSocket : public PA_Allocated {
public:

	enum Opcode {
		TEXT=0x1,
		BINARY=0x2,
		CLOSE=0x8,
		PING=0x9,
		PONG=0xA
	};

	/// the status codes of the Close frame
	enum Status {
		NORMAL=1000,
		PROTOCOL_ERROR=1002,
		NO_STATUS=1005,
		ABNORMAL=1006,
		INVALID_DATA=1007,
		TOO_BIG=1009,
		INTERNAL_ERROR=1011
	};

	struct Message {
		Opcode opcode;
		const char *data;	/// the payload (the reason for CLOSE), valid until the next call
		size_t size;
		int code;		/// the status of CLOSE

		void set(Opcode aopcode, const char *adata, size_t asize, int acode=0);
	};

	/// the status codes that can be sent in Close
	static bool valid_status(int code);

	/// the handshake is done
	bool accepted;
	/// the closing handshake is over: nothing can be received or sent
	bool closed;

	WebSocket(HTTPD_Connection &aconnection);
	/// sends Close with INTERNAL_ERROR if the connection is not closed: the request has ended with an exception
	~WebSocket();

	/// the handshake response is sent, messages up to max_message_size are received
	void start(size_t amax_message_size);

	/// waits up to timeout_ms for the next message, false on timeout; the pings are answered here, the broken connection is the CLOSE message
	bool receive(int timeout_ms, Message &message);

	void send(Opcode opcode, const char *data, size_t size);

	/// sends Close and waits for the answer
	void close(int code=NORMAL, const char *reason="", size_t reason_size=0);

private:

	struct Frame {
		bool fin;
		int opcode;
		char *payload;
		size_t size;
	};

	enum Parsed { INCOMPLETE, PARSED, INVALID };

	/// the growing array of bytes, data[pos..size) are not processed yet
	struct Buffer {
		char *data;
		size_t pos, size, allocated;

		Buffer(): data(0), pos(0), size(0), allocated(0) {}

		size_t unread() const { return size-pos; }
		void fit(size_t room);
		void append(const char *bytes, size_t length);
	};

	HTTPD_Connection &connection;
	size_t max_message_size;

	Buffer rbuf;			/// received bytes
	size_t need;			/// bytes of the incomplete frame
	bool eof;

	Buffer msg;			/// the fragments of the message
	int msg_opcode;			/// 0 if there is no fragmented message

	bool close_sent;
	int error_code;
	const char *error_reason;

	bool fill(int timeout_ms);
	Parsed parse(Frame &frame);
	Parsed invalid(int code, const char *reason);
	bool failed(int code, const char *reason, bool notify, Message &message);
	void write_frame(int opcode, const char *data, size_t size);
	void send_close(int code, const char *reason, size_t size);
};

#endif
