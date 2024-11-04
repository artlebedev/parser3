/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#define IDENT_PA_SOCKS_H "$Id: pa_socks.h,v 1.23 2024/11/04 03:53:25 moko Exp $"

void pa_socks_init();
void pa_socks_done();

int pa_socks_errno();
const char* pa_socks_strerr(int no);

#endif
