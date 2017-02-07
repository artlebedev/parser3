/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#define IDENT_PA_SOCKS_H "$Id: pa_socks.h,v 1.20 2017/02/07 22:00:36 moko Exp $"

void pa_socks_init();
void pa_socks_done();

int pa_socks_errno();
const char* pa_socks_strerr(int no);

#endif
