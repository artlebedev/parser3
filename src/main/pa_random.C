/** @file
	Parser: random related functions.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

// includes

#include "pa_common.h"
#include "pa_random.h"
#include "pa_exception.h"
#include "pa_threads.h"

volatile const char * IDENT_PA_RANDOM_C="$Id: pa_random.C,v 1.11 2021/01/13 21:28:15 moko Exp $" IDENT_PA_RANDOM_H;

#ifdef _MSC_VER
#include <windows.h>

class Random_provider {
	HCRYPTPROV fhProv;
	
	void acquire() {
		SYNCHRONIZED;

		if(fhProv)
			return;

		if(!CryptAcquireContext(&fhProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
			throw Exception(0, 0, "CryptAcquireContext failed");
	}
	void release() {
		if(fhProv)
			CryptReleaseContext(fhProv, 0);
	}
	
public:
	Random_provider(): fhProv(0) {}
	~Random_provider() { release(); }
	void generate(void *buffer, size_t size) {
		acquire();

		if(!CryptGenRandom(fhProv, size, (BYTE*)buffer))
			throw Exception(0, 0, "CryptGenRandom failed");
	}
}
	random_provider;

#else

/// from gen_uuid.c
static int get_random_fd(void)
{
        struct timeval  tv;
        static int      fd = -2;
        int             i;

        if (fd == -2) {
                gettimeofday(&tv, 0);
                fd = open("/dev/urandom", O_RDONLY);
                if (fd == -1)
                        fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
                srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);
        }
        /* Crank the random number generator a few times */
        gettimeofday(&tv, 0);
        for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
                rand();
        return fd;
}


/*
 * Generate a series of random bytes.  Use /dev/urandom if possible,
 * and if not, use srandom/random.
 */
static void get_random_bytes(void *buf, int nbytes)
{
        int i, fd = get_random_fd();
        int lose_counter = 0;
        char *cp = (char *) buf;

        if (fd >= 0) {
                while (nbytes > 0) {
                        i = read(fd, cp, nbytes);
                        if (i <= 0) {
                                if (lose_counter++ > 16)
                                        break;
                                continue;
                        }
                        nbytes -= i;
                        cp += i;
                        lose_counter = 0;
                }
        }

        /* XXX put something better here if no /dev/random! */
        for (i = 0; i < nbytes; i++)
                *cp++ = rand() & 0xFF;
        return;
}


#endif

void random(void *buffer, size_t size) {
#ifdef _MSC_VER
	random_provider.generate(buffer, size);
#else
	get_random_bytes(buffer, size);
#endif
}

/// to hell with extra bytes on 64bit platforms
struct uuid {
	unsigned int	time_low;
	unsigned short	time_mid;
	unsigned short	time_hi_and_version;
	unsigned short	clock_seq;
	unsigned char	node[6];
};

static uuid get_uuid() {
	// random
	uuid uuid;
	random(&uuid, sizeof(uuid));

	// http://www.opengroup.org/onlinepubs/9629399/apdxa.htm#tagtcjh_35
	// ~
	// version = DCE Security version, with embedded POSIX UIDs.  
	// variant = DCE
	//
	// DCE=Distributed Computing Environment
	// http://www.opengroup.org/dce/
	//
	// they say this influences comparison&such,
	// but could not figure out how, hence structure layout specified strictly
	// anyhow, uuidgen on Win32 yield those values
	// 
	// xxxxxxxx-xxxx-4xxx-{8,9,A,B}xxx-xxxxxxxxxxxx
	uuid.clock_seq = (uuid.clock_seq & 0x3FFF) | 0x8000;
        uuid.time_hi_and_version = (uuid.time_hi_and_version & 0x0FFF) | 0x4000;

	return uuid;
}

char *get_uuid_cstr(bool lower, bool solid) {
	uuid uuid=get_uuid();

	const size_t bufsize=36+1/*zero-teminator*/+1/*for faulty snprintfs*/;
	char* cstr=new(PointerFreeGC) char[bufsize];

	const char *format[] = {
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x"
	};

	snprintf(cstr, bufsize,
		format[(lower ? 1:0) + (solid ? 2:0)],
		uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
		uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
		uuid.node[0], uuid.node[1], uuid.node[2],
		uuid.node[3], uuid.node[4], uuid.node[5]);
	return cstr;
}

char *get_uuid_boundary() {
	uuid uuid=get_uuid();

	const int boundary_bufsize=10+32+1/*for zero-teminator*/+1/*for faulty snprintfs*/;
	char* boundary=new(PointerFreeGC) char[boundary_bufsize];

	snprintf(boundary, boundary_bufsize,
		"----------%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		uuid.time_low, uuid.time_mid, uuid.time_hi_and_version,
		uuid.clock_seq >> 8, uuid.clock_seq & 0xFF,
		uuid.node[0], uuid.node[1], uuid.node[2],
		uuid.node[3], uuid.node[4], uuid.node[5]);
	return boundary;
}
