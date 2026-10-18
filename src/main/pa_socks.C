/**	@file
	Parser: socks initialization/finalization.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

volatile const char * IDENT_PA_SOCKS_C="$Id: pa_socks.C,v 1.40 2026/10/18 18:15:24 moko Exp $";

#include "pa_config_includes.h"
#include "pa_socks.h"

#ifdef _MSC_VER

#include "pa_exception.h"
#include "pa_string.h"

#include <windows.h>

WSADATA wsaData;

void pa_socks_init() {
	WORD wVersionRequested;
	int err; 
	wVersionRequested = MAKEWORD( 1, 1 ); 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		throw Exception(0, 0, "cannot WSAStartup, err=%d", err);
	} 
}

void pa_socks_done() {
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */ 
	if ( LOBYTE( wsaData.wVersion ) == 2 || HIBYTE( wsaData.wVersion ) == 2 ) {
		WSACleanup();
		return;
	} 
}

const char* pa_socks_strerr(int no) {
	char buf[MAX_STRING];
	buf[0]=0;
	size_t error_size=FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		no,
		0, // Default language
		(LPTSTR) &buf,
		sizeof(buf),
		NULL 
	);
	if(error_size>3) // ".\r\n"
		buf[error_size-3]=0;
	return buf[0]? pa_strdup(buf): "unknown error";
}
int pa_socks_errno() {
	return WSAGetLastError();
}

bool pa_socks_is_reset(int no) {
	return no==WSAECONNRESET || no==WSAECONNABORTED;
}

bool pa_socks_is_interrupted(int no) {
	return no==WSAEINTR;
}

// there are no signals, calls are not restarted for the time left
typedef int sock_time;

static sock_time sock_now() {
	return 0;
}

static bool sock_ready_within_remaining_timeout(SOCKET, bool, sock_time) {
	return false;
}

int pa_sock_ready(SOCKET sock, bool for_write, int timeout_ms) {
	WSAPOLLFD fd={sock, (SHORT)(for_write ? POLLWRNORM : POLLRDNORM), 0};
	return WSAPoll(&fd, 1, timeout_ms);
}

#else

#include <poll.h>

void pa_socks_init() {}
void pa_socks_done() {}

const char* pa_socks_strerr(int no) {
	return strerror(no);
}
int pa_socks_errno() {
	return errno;
}

bool pa_socks_is_reset(int no) {
	return no==ECONNRESET;
}

bool pa_socks_is_interrupted(int no) {
	return no==EINTR;
}

typedef struct timeval sock_time;

static sock_time sock_now() {
	sock_time now;
	gettimeofday(&now, 0);
	return now;
}

static long long timeval_ms(const struct timeval &tv) {
	return (long long)tv.tv_sec*1000+tv.tv_usec/1000;
}

int pa_sock_ready(SOCKET sock, bool for_write, int timeout_ms) {
	struct pollfd fd={sock, (short)(for_write ? POLLOUT : POLLIN), 0};
	sock_time start=sock_now();

	int left=timeout_ms;
	int result;
	// GC stops the threads with signals: continue poll for the time left
	while((result=poll(&fd, 1, left))<0 && pa_socks_is_interrupted(errno)) {
		if(timeout_ms>0){
			long long passed=timeval_ms(sock_now()) - timeval_ms(start);
			if(passed>=timeout_ms)
				return 0;
			left=passed<0 ? timeout_ms : timeout_ms-(int)passed;
		}
	}
	return result;
}

// the system restarts recv/send with the whole socket timeout again, under load it would never expire: wait for the remaining time
static bool sock_ready_within_remaining_timeout(SOCKET sock, bool for_write, sock_time start) {
	struct timeval socket_timeout;
	socklen_t timeout_size=sizeof(socket_timeout);
	if(getsockopt(sock, SOL_SOCKET, for_write ? SO_SNDTIMEO : SO_RCVTIMEO, &socket_timeout, &timeout_size)<0 || !(socket_timeout.tv_sec || socket_timeout.tv_usec))
		return true; // no timeout

	long long passed=timeval_ms(sock_now())-timeval_ms(start);
	long long left=timeval_ms(socket_timeout)-passed;
	int ready=left>0 ? pa_sock_ready(sock, for_write, left<INT_MAX ? (int)left : INT_MAX) : 0;
	if(ready==0)
		errno=EAGAIN;
	return ready>0;
}

#endif

ssize_t pa_sock_recv(SOCKET sock, char *buf, size_t len) {
	sock_time start=sock_now();
	ssize_t result;
	while((result=recv(sock, buf, len, 0)) < 0 && pa_socks_is_interrupted(pa_socks_errno()) && sock_ready_within_remaining_timeout(sock, false, start));
	return result;
}

ssize_t pa_sock_send(SOCKET sock, const char *buf, size_t len) {
	sock_time start=sock_now();
	ssize_t result;
	while((result=send(sock, buf, len, 0)) < 0 && pa_socks_is_interrupted(pa_socks_errno()) && sock_ready_within_remaining_timeout(sock, true, start));
	return result;
}
