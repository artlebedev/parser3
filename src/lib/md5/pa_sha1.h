/** @file
	Parser: SHA-1 implementation based on the reference code published in RFC 3174.
	http://www.rfc-editor.org/rfc/rfc3174
*/

#ifndef PA_SHA1_H
#define PA_SHA1_H

#define IDENT_PA_SHA1_H "$Id: pa_sha1.h,v 1.1 2026/10/16 13:55:16 moko Exp $"

struct PA_SHA1_CTX {
	unsigned Message_Digest[5], Length_Low, Length_High;
	unsigned int Message_Block[64];
	int Message_Block_Index, Computed, Corrupted;
};

void pa_SHA1Reset(PA_SHA1_CTX *context);
void pa_SHA1Input(PA_SHA1_CTX *context, const unsigned char *message_array, unsigned length);
/// writes 20 bytes of the digest to buf, throws if it cannot be computed
void pa_SHA1ReadDigest(void *buf, PA_SHA1_CTX *c);

#endif
