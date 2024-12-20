/** @file
	Parser: request charsets class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_REQUEST_CHARSETS_H
#define PA_REQUEST_CHARSETS_H

#define IDENT_PA_REQUEST_CHARSETS_H "$Id: pa_request_charsets.h,v 1.11 2024/11/04 03:53:25 moko Exp $"

class Request_charsets {
	friend class Temp_client_charset;

	Charset *fsource;
	Charset *fclient;
	Charset *fmail;
public:
	Request_charsets(
		Charset& asource, 
		Charset& aclient,
		Charset& amail):
		fsource(&asource),
		fclient(&aclient),
		fmail(&amail) {}

	Charset& source() const { return *fsource; } void set_source(Charset& asource) { fsource=&asource; }
	Charset& client() const { return *fclient; } void set_client(Charset& aclient) { fclient=&aclient; }
	Charset& mail() const { return *fmail; } void set_mail(Charset& amail) { fmail=&amail; }
};

class Temp_client_charset {
	Request_charsets& fcharsets;
	Charset &fclient;
public:

	Temp_client_charset(Request_charsets& acharsets, Charset& aclient):
		fcharsets(acharsets),
		fclient(acharsets.client()) {
		fcharsets.set_client(aclient);
	}
	~Temp_client_charset(){
		fcharsets.set_client(fclient);
	}
};

#endif
