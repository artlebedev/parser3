/** @file
	Parser: SHA-1 implementation based on the reference code published in RFC 3174.
	http://www.rfc-editor.org/rfc/rfc3174
*/

#include "pa_sha1.h"
#include "pa_exception.h"

volatile const char * IDENT_PA_SHA1_C="$Id: pa_sha1.C,v 1.1 2026/10/16 13:55:16 moko Exp $" IDENT_PA_SHA1_H;

#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF)|((word) >> (32-(bits))))
static void SHA1ProcessMessageBlock(PA_SHA1_CTX *);
static void SHA1PadMessage(PA_SHA1_CTX *);
void pa_SHA1Reset(PA_SHA1_CTX *context) {
	context->Length_Low = context->Length_High = context->Message_Block_Index = 0;
	context->Message_Digest[0]	= 0x67452301;
	context->Message_Digest[1]	= 0xEFCDAB89;
	context->Message_Digest[2]	= 0x98BADCFE;
	context->Message_Digest[3]	= 0x10325476;
	context->Message_Digest[4]	= 0xC3D2E1F0;
	context->Computed = context->Corrupted  = 0;
}

static int SHA1Result(PA_SHA1_CTX *context) {
	if (context->Corrupted)
		return 0;
	if (!context->Computed) {
		SHA1PadMessage(context);
		context->Computed = 1;
	}
	return 1;
}

void pa_SHA1Input(PA_SHA1_CTX *context, const unsigned char *message_array, unsigned length) {
	if (!length)
		return;
	if (context->Computed || context->Corrupted) {
		context->Corrupted = 1;
		return;
	}

	while(length-- && !context->Corrupted) {
		context->Message_Block[context->Message_Block_Index++] = (*message_array & 0xFF);
		context->Length_Low += 8;
		context->Length_Low &= 0xFFFFFFFF;
		if (!context->Length_Low && !(context->Length_High=((1+context->Length_High)&0xFFFFFFFF)))
			context->Corrupted = 1; // too long message
		if (context->Message_Block_Index == 64)
			SHA1ProcessMessageBlock(context);
		message_array++;
	}
}

static void SHA1ProcessMessageBlock(PA_SHA1_CTX *context) {
	const unsigned K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
	int t;
	unsigned    temp, W[80], buf[5];
	unsigned &A=buf[0], &B=buf[1], &C=buf[2], &D=buf[3], &E=buf[4];

	for(t = 0; t < 16; t++)
		W[t] = (((unsigned) context->Message_Block[t * 4]) << 24) | (((unsigned) context->Message_Block[t * 4 + 1]) << 16) | (((unsigned) context->Message_Block[t * 4 + 2]) << 8) | ((unsigned) context->Message_Block[t * 4 + 3]);

	for(t = 16; t < 80; t++)
		W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);

	memcpy (buf, context->Message_Digest, sizeof(buf));
	for(t = 0; t < 20; t++) {
		temp =  (SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 20; t < 40; t++) {
		temp = (SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 40; t < 60; t++) {
		temp = (SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for(t = 60; t < 80; t++) {
		temp = (SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3]) & 0xFFFFFFFF;
		E = D; D = C;
		C = SHA1CircularShift(30,B);
		B = A; A = temp;
	}

	for (t = 0; t < 5; t++)
		context->Message_Digest[t] = (context->Message_Digest[t] + buf[t]) & 0xFFFFFFFF;

	context->Message_Block_Index = 0;
}

static void SHA1PadMessage(PA_SHA1_CTX *context) {
	context->Message_Block[context->Message_Block_Index++] = 0x80;
	if (context->Message_Block_Index > 56) {
		//was 55, one shift
		while(context->Message_Block_Index < 64)
			context->Message_Block[context->Message_Block_Index++] = 0;
		SHA1ProcessMessageBlock(context);
		while(context->Message_Block_Index < 56)
			context->Message_Block[context->Message_Block_Index++] = 0;
	} else
		while(context->Message_Block_Index < 56)
			context->Message_Block[context->Message_Block_Index++] = 0;
	context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
	context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
	context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
	context->Message_Block[59] = (context->Length_High) & 0xFF;
	context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
	context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
	context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
	context->Message_Block[63] = (context->Length_Low) & 0xFF;
	SHA1ProcessMessageBlock(context);
}

#ifdef PA_BIG_ENDIAN
#define SWAP(n) (n)
#else
#define SWAP(n) (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#endif

void pa_SHA1ReadDigest(void *buf, PA_SHA1_CTX *c)
{
	if(!SHA1Result(c))
		throw Exception (PARSER_RUNTIME, 0, "Cannot compute SHA1");

	((uint32_t *)buf)[0] = SWAP(c->Message_Digest[0]);
	((uint32_t *)buf)[1] = SWAP(c->Message_Digest[1]);
	((uint32_t *)buf)[2] = SWAP(c->Message_Digest[2]);
	((uint32_t *)buf)[3] = SWAP(c->Message_Digest[3]);
	((uint32_t *)buf)[4] = SWAP(c->Message_Digest[4]);
}
