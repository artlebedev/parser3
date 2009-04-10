/** @file
	Parser: @b inet parser class.

	Copyright(c) 2001-2009 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_INET_C="$Date: 2009/04/10 11:31:06 $";

#include "pa_vmethod_frame.h"
#include "pa_request.h"

class MInet: public Methoded {
public:
	MInet();

public: // Methoded
	bool used_directly() { return true; }
};

// global variables

DECLARE_CLASS_VAR(inet, new MInet, 0);


static void _ntoa(Request& r, MethodParams& params){
	unsigned long l=(unsigned long)trunc(params.as_double(0, "parameter must be expression", r));
	static const int ip_cstr_bufsize=3*4+3+1+1;
	char* ip_cstr=new(PointerFreeGC) char[ip_cstr_bufsize];

	snprintf(ip_cstr, ip_cstr_bufsize, "%u.%u.%u.%u",
				(l>>24) & 0xFF,
				(l>>16) & 0xFF,
				(l>>8) & 0xFF,
				l & 0xFF);

	r.write_no_lang(*new String(ip_cstr));
}

static void _aton(Request& r, MethodParams& params){
	const String ip=params.as_string(0, PARAMETER_MUST_BE_STRING);
	if(ip.is_empty())
		throw Exception(PARSER_RUNTIME,
			0,
			"IP address must not be empty.");

	const char* ip_cstr=ip.cstr();
	ulong result=0;
	uint byte_value=0;
	uint dot_cnt=0;
	bool byte_start=true;
	bool err=false;
	const char* p=ip_cstr;
	while(char c=*p++){
		uint digit=(uint)(c-'0');	// assume ascii
		if(digit>=0 && digit<=9){
			byte_start=false;
			if((byte_value=byte_value*10+digit) > 255){
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
		throw Exception(PARSER_RUNTIME,
			0,
			"Invalid IP address '%s' specified.", ip_cstr);
	} else {
		result=(result << 8)+(ulong)byte_value;
		r.write_no_lang(*new VDouble(result));
	}
}

// constructor
MInet::MInet(): Methoded("inet") {
	add_native_method("ntoa", Method::CT_STATIC, _ntoa, 1, 1);
	add_native_method("aton", Method::CT_STATIC, _aton, 1, 1);

	// @todo: gethostbyname, gethostbyaddr
}
