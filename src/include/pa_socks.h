/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

static const char * const IDENT_SOCKS_H="$Date: 2004/10/06 10:55:02 $";

void pa_socks_init();
void pa_socks_done();

int pa_socks_errno();
const char* pa_socks_strerr(int no);

#endif
