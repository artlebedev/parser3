/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#define IDENT_PA_SOCKS_H "$Id: pa_socks.h,v 1.29 2026/10/18 18:15:24 moko Exp $"

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

/// true if the error means that the connection is reset or aborted
bool pa_socks_is_reset(int no);

/// true if the error means that the call is interrupted by a signal
bool pa_socks_is_interrupted(int no);

/// recv and send, restarted if interrupted by a signal: GC stops the threads with signals, the socket timeout (SO_RCVTIMEO, SO_SNDTIMEO) is not restarted
ssize_t pa_sock_recv(SOCKET sock, char *buf, size_t len);
ssize_t pa_sock_send(SOCKET sock, const char *buf, size_t len);

/// waits up to timeout_ms (negative is forever) for the socket to be ready for reading (or writing): >0 ready, 0 timeout, <0 error
int pa_sock_ready(SOCKET sock, bool for_write, int timeout_ms);

#endif
