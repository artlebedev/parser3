/** @file
	Parser: Transcoder impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_transcoder.C,v 1.3 2001/12/14 15:25:50 paf Exp $
*/

#include "pa_common.h"
#include "pa_transcoder.h"
#include "pa_exception.h"

static int sort_cmp_Trans_rec_intCh(const void *a, const void *b) {
	return 
		static_cast<const Transcoder_TransRec *>(a)->intCh-
		static_cast<const Transcoder_TransRec *>(b)->intCh;
}

void Transcoder::sort_ToTable() {
	_qsort(toTable, toTableSize, sizeof(*toTable), 
		sort_cmp_Trans_rec_intCh);
	//FILE *f=fopen("c:\\temp\\a", "wb");
	//fwrite(toTable, toTableSize, sizeof(*toTable), f);
	//fclose(f);
}

XMLByte Transcoder::xlatOneTo(const XMLCh toXlat) const {
    unsigned int    lowOfs = 0;
    unsigned int    hiOfs = toTableSize - 1;
    XMLByte         curByte = 0;
    do {
        // Calc the mid point of the low and high offset.
        const unsigned int midOfs = ((hiOfs - lowOfs) / 2) + lowOfs;

        //  If our test char is greater than the mid point char, then
        //  we move up to the upper half. Else we move to the lower
        //  half. If its equal, then its our guy.
        if (toXlat > toTable[midOfs].intCh)
            lowOfs = midOfs;
		else if (toXlat < toTable[midOfs].intCh)
			hiOfs = midOfs;
		else
			return toTable[midOfs].extCh;
	} while(lowOfs + 1 < hiOfs);

    return 0;
}

void transcode(Pool& pool,
	const Transcoder *source_transcoder, const void *source_body, size_t source_content_length,
	const Transcoder *dest_transcoder, const void *& dest_body, size_t& dest_content_length
	) {
	switch((source_transcoder?0x10:0x00)|(dest_transcoder?0x01:0x00)) {
		case 0x00:
			dest_body=source_body;
			dest_content_length=source_content_length;
			break;
		case 0x10:
			source_transcoder->transcodeToUTF8(pool,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
		case 0x01:
			dest_transcoder->transcodeFromUTF8(pool,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
		default: // 0x11
			source_transcoder->transcodeToTranscoder(pool, *dest_transcoder,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
	}
}

// ---------------------------------------------------------------------------
//  Local static data
//
//  gUTFBytes
//      A list of counts of trailing bytes for each initial byte in the input.
//
//  gUTFOffsets
//      A list of values to offset each result char type, according to how
//      many source bytes when into making it.
//
//  gFirstByteMark
//      A list of values to mask onto the first byte of an encoded sequence,
//      indexed by the number of bytes used to create the sequence.
// ---------------------------------------------------------------------------
static const XMLByte gUTFBytes[0x100] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    ,   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    ,   3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const uint gUTFOffsets[6] = {
    0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080
};

static const XMLByte gFirstByteMark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

/// @todo not so memory-hungry with prescan
void Transcoder::transcodeToUTF8(Pool& pool,
								 const void *source_body, size_t source_content_length,
								 const void *& adest_body, size_t& adest_content_length) const {
	
	size_t dest_content_length=0;
	XMLByte *dest_body=(XMLByte*)pool.malloc(source_content_length*6/*so that surly enough*/);

	const XMLByte* srcPtr=(const XMLByte*)source_body;
	const XMLByte* srcEnd=(const XMLByte*)source_body+source_content_length;
	XMLByte* outPtr=dest_body;

    while (srcPtr < srcEnd) {
        uint curVal = fromTable[*srcPtr];

        // Figure out how many bytes we need
        unsigned int encodedBytes;
        if (curVal < 0x80)
            encodedBytes = 1;
        else if (curVal < 0x800)
            encodedBytes = 2;
        else if (curVal < 0x10000)
            encodedBytes = 3;
        else if (curVal < 0x200000)
            encodedBytes = 4;
        else if (curVal < 0x4000000)
            encodedBytes = 5;
        else if (curVal <= 0x7FFFFFFF)
            encodedBytes = 6;
        else {
            // use the replacement character
            *outPtr++ = '?';
            srcPtr ++;
            continue;
        }

        //  If we cannot fully get this char into the output buffer,
		// never

        // We can do it, so update the source index
        srcPtr++;

        //  And spit out the bytes. We spit them out in reverse order
        //  here, so bump up the output pointer and work down as we go.
        outPtr += encodedBytes;
        switch(encodedBytes) {
            case 6 : *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal >>= 6;
            case 5 : *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal >>= 6;
            case 4 : *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal >>= 6;
            case 3 : *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal >>= 6;
            case 2 : *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal >>= 6;
            case 1 : *--outPtr = XMLByte(curVal | gFirstByteMark[encodedBytes]);
        }

        // Add the encoded bytes back in again to indicate we've eaten them
        outPtr += encodedBytes;
    }

	// return
	adest_body=dest_body;
	adest_content_length=outPtr-dest_body;
}
void Transcoder::transcodeFromUTF8(Pool& pool,
								   const void *source_body, size_t source_content_length,
								   const void *& adest_body, size_t& adest_content_length) const {
	size_t dest_content_length=0;
	XMLByte *dest_body=(XMLByte*)pool.malloc(source_content_length/*surly enough*/);

	const XMLByte* srcPtr=(const XMLByte*)source_body;
	const XMLByte* srcEnd=(const XMLByte*)source_body+source_content_length;
	XMLByte* outPtr=dest_body;

    //  We now loop until we either run out of input data
    while (srcPtr < srcEnd) {
        // Get the next leading byte out
        const XMLByte firstByte = *srcPtr;

        // Special-case ASCII, which is a leading byte value of <= 127
        if (firstByte <= 127) {
            *outPtr++ = firstByte;
            srcPtr++;
            continue;
        }

        // See how many trailing src bytes this sequence is going to require
        const unsigned int trailingBytes = gUTFBytes[firstByte];

        //  If there are not enough source bytes to do this one, then we
        //  are done. Note that we done >= here because we are implicitly
        //  counting the 1 byte we get no matter what.
        if (srcPtr + trailingBytes >= srcEnd)
            break;

        // Looks ok, so lets build up the value
        uint tmpVal=0;
        switch(trailingBytes) {
            case 5: tmpVal+=*srcPtr++; tmpVal<<=6;
            case 4: tmpVal+=*srcPtr++; tmpVal<<=6;
            case 3: tmpVal+=*srcPtr++; tmpVal<<=6;
            case 2: tmpVal+=*srcPtr++; tmpVal<<=6;
            case 1: tmpVal+=*srcPtr++; tmpVal<<=6;
            case 0: tmpVal+=*srcPtr++;
                     break;

            default:
                throw Exception(0, 0,
					0,
					"transcodeFromUTF8 error: wrong trailingBytes value (%d)", trailingBytes);
        }
        tmpVal-=gUTFOffsets[trailingBytes];

        //  If it will fit into a single char, then put it in. Otherwise
        //  fail [*encode it as a surrogate pair. If its not valid, use the
        //  replacement char.*]
        if (!(tmpVal & 0xFFFF0000))
            *outPtr++ = xlatOneTo(tmpVal);
		else
			throw Exception(0, 0,
				0,
				"transcodeFromUTF8 error: too big tmpVal (0x%08X)", tmpVal);
	}

	// return
	adest_body=dest_body;
	adest_content_length=outPtr-dest_body;
}

/// transcode using both transcoders
void Transcoder::transcodeToTranscoder(Pool& pool,
									   const Transcoder& dest_transcoder,
									   const void *source_body, size_t source_content_length,
									   const void *& adest_body, size_t& dest_content_length) const {
	throw Exception(0, 0,
		0,
		"transcodeToTranscoder not supported(yet)");
/*
	void *dest_body;

	dest_body=pool.malloc(dest_content_length=source_content_length);
	// dummy
	memset(dest_body, '?', dest_content_length);

	adest_body=dest_body;*/
}			
