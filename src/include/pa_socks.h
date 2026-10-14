/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#define IDENT_PA_SOCKS_H "$Id: pa_socks.h,v 1.25 2026/10/14 00:32:44 moko Exp $"

#ifdef _MSC_VER
#define socklen_t int
#else
#define closesocket close
#define INVALID_SOCKET -1
typedef int SOCKET;
#endif

void pa_socks_init();
void pa_socks_done();

int pa_socks_errno();
const char* pa_socks_strerr(int no);

/// waits up to timeout_ms for the socket to be ready for reading (or writing): >0 ready, 0 timeout, <0 error
int pa_sock_ready(SOCKET sock, bool for_write, int timeout_ms);

#endif
