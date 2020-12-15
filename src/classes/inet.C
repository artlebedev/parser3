/** @file
	Parser: @b inet parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

#include "pa_vmethod_frame.h"
#include "pa_request.h"
#include "pa_vtable.h"

#ifdef _MSC_VER
#include "winsock2.h"
#include "ws2tcpip.h"
#endif

volatile const char * IDENT_INET_C="$Id: inet.C,v 1.16 2020/12/15 17:10:28 moko Exp $";

class MInet: public Methoded {
public:
	MInet();
};

// global variables

DECLARE_CLASS_VAR(inet, new MInet);


static void _ntoa(Request& r, MethodParams& params){
	unsigned long l=(unsigned long)trunc(params.as_double(0, "parameter must be expression", r));
	static const int ip_cstr_bufsize=3*4+3+1/*zero-teminator*/+1/*for faulty snprintfs*/;
	char* ip_cstr=new(PointerFreeGC) char[ip_cstr_bufsize];
	snprintf(ip_cstr, ip_cstr_bufsize, "%u.%u.%u.%u", (l>>24) & 0xFF, (l>>16) & 0xFF, (l>>8) & 0xFF, l & 0xFF);
	r.write(*new String(ip_cstr));
}

static void _aton(Request& r, MethodParams& params){
	const String ip=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(ip.is_empty())
		throw Exception(PARSER_RUNTIME, 0, "IP address must not be empty.");

	const char* ip_cstr=ip.cstr();
	ulong result=0;
	uint byte_value=0;
	uint dot_cnt=0;
	bool byte_start=true;
	bool err=false;
	const char* p=ip_cstr;
	while(char c=*p++){
		int digit=(int)(c-'0');	// assume ascii
		if(digit>=0 && digit<=9){
			byte_start=false;
			if((byte_value=byte_value*10+(uint)digit) > 255){
				err=true;
				break;
			}
		} else if(c=='.'){
			if(byte_start){ // two dots in row or IP started with dot
				err=true;
				break;
			} else {
				byte_start=true;
				dot_cnt++;
				result=(result << 8)+(ulong)byte_value;
				byte_value=0;
			}
		} else { // invalid char
			err=true;
			break;
		}
	}

	if(err || dot_cnt!=3 || byte_start){
		throw Exception(PARSER_RUNTIME, 0, "Invalid IP address '%s' specified.", ip_cstr);
	} else {
		result=(result << 8)+(ulong)byte_value;
		r.write(*new VDouble(result));
	}
}

int ipv_format(const String &value){
	if(value == "4") return AF_INET;
	if(value == "6") return AF_INET6;
	if(value == "any") return AF_UNSPEC;
	throw Exception(PARSER_RUNTIME, &value, "ipv option value must be 4 or 6 or any");
}

static void _ip2name(Request& r, MethodParams& params){
	const String sip=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(sip.is_empty())
		throw Exception(PARSER_RUNTIME, 0, "IP address must not be empty.");

	const char* ip_cstr=sip.cstr();

	struct addrinfo hints, *info=0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_NUMERICHOST; // to disable DNS lookup
	
	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)){
			int valid_options=0;
			if(Value* value=options->get("ipv")){
				hints.ai_family=ipv_format(r.process(*value).as_string());
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	int error=getaddrinfo(ip_cstr, 0, &hints, &info);
	if(error==EAI_NONAME)
		throw Exception(PARSER_RUNTIME, 0, "Invalid IP address '%s' specified", ip_cstr);
	if(error)
		throw Exception(PARSER_RUNTIME, 0, "Invalid IP address '%s': %s", ip_cstr, gai_strerror(error));

	char hbuf[NI_MAXHOST];
	error=getnameinfo(info->ai_addr, info->ai_addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD);

	freeaddrinfo(info);

	if(!error){
		r.write(*new String(pa_idna_decode(pa_strdup(hbuf), r.charsets.source()), String::L_TAINTED));
	} else if(error!=EAI_NONAME){
		throw Exception(PARSER_RUNTIME, 0, "Can't resolve IP address '%s': %s", ip_cstr, gai_strerror(error));
	}
}
static void _name2ip(Request& r, MethodParams& params){
	const String sname=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(sname.is_empty())
		throw Exception(PARSER_RUNTIME, 0, "Domain name must not be empty.");

	const char* name_cstr=pa_idna_encode(sname.cstr(), r.charsets.source());

	struct addrinfo hints, *info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;

	Table *table=NULL;

	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)){
			int valid_options=0;
			if(Value* value=options->get("ipv")){
				hints.ai_family=ipv_format(r.process(*value).as_string());
				valid_options++;
			}
			if(Value* value=options->get("table")){
				if(r.process(*value).as_bool()){
					Table::columns_type columns(new ArrayString);
					static const String sip("ip"), sversion("version");
					*columns+=&sip; *columns+=&sversion;
					table=new Table(columns);
				}
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	int error=getaddrinfo(name_cstr, NULL, &hints, &info);

	if(error)
		throw Exception(PARSER_RUNTIME, 0, "Can't resolve domain name '%s': %s", name_cstr, gai_strerror(error));

	char hbuf[INET6_ADDRSTRLEN];

	for(struct addrinfo *cur=info; cur; cur=cur->ai_next) {
		if(error=getnameinfo(cur->ai_addr, cur->ai_addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NUMERICHOST))
			throw Exception(PARSER_RUNTIME, 0, "Can't translate address: %s", gai_strerror(error));
		String *saddr=new String(pa_strdup(hbuf), String::L_TAINTED);
		if(table){
			Table::element_type row(new ArrayString());
			static const String sv4("4"), sv6("6"), sunknown("unknown");
			*row+=saddr;
			*row+=cur->ai_family == AF_INET ? &sv4 : cur->ai_family == AF_INET6 ? &sv6 : &sunknown;
			*table+=row;
		} else {
			r.write(*saddr);
			break;
		}
	}

	if(table)
		r.write(*new VTable(table));

	freeaddrinfo(info);
}

// constructor
MInet::MInet(): Methoded("inet") {
	add_native_method("ntoa", Method::CT_STATIC, _ntoa, 1, 1);
	add_native_method("aton", Method::CT_STATIC, _aton, 1, 1);
	// ^inet:ip2name[ip; $.ipv[4|6|any] ]
	add_native_method("ip2name", Method::CT_STATIC, _ip2name, 1, 2);
	// ^inet:name2ip[name; $.ipv[4|6|any] $.table(true) ]
	add_native_method("name2ip", Method::CT_STATIC, _name2ip, 1, 2);
}
