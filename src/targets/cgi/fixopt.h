/** @file
	Parser: CGI: fixing command line options decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef FIXOPT_H
#define FIXOPT_H

static const char* IDENT_FIXOPT_H="$Date: 2002/08/14 11:38:07 $";

void fixopt(
	int beforec, char **beforev, 
	int& afterc, char **& afterv);

#endif
