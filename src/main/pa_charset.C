/** @file
	Parser: Charset connection implementation.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan<paf@design.ru>(http://paf.design.ru)
*/

static const char* IDENT_CHARSET_C="$Date: 2002/09/10 08:41:00 $";

#include "pa_charset.h"

#ifdef XML
#include "libxml/encoding.h"
#endif

// globals


// consts

#define MAX_CHARSET_UNI_CODES 500

// helpers

inline void prepare_case_tables(unsigned char *tables) {
	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	for(int i=0; i<0x100; i++)
		lcc_table[i]=fcc_table[i]=i;
}
inline void cstr2ctypes(unsigned char *tables, const unsigned char *cstr, 
						unsigned char bit) {
	unsigned char *ctypes_table=tables+ctypes_offset;
	ctypes_table[0]=bit;
	for(; *cstr; cstr++) {
		unsigned char c=*cstr;
		ctypes_table[c]|=bit;
	}
}
inline unsigned int to_wchar_code(const char *cstr) {
	if(!cstr || !*cstr)
		return 0;
	if(cstr[1]==0)
		return(unsigned int)(unsigned char)cstr[0];

	char *error_pos;
	return(unsigned int)strtol(cstr, &error_pos, 0);
}
inline bool to_bool(const char *cstr) {
	return cstr && *cstr!=0;
}
static void element2ctypes(unsigned char c, bool belongs,
						   unsigned char *tables,  unsigned char bit, int group_offset=-1) {
	if(!belongs)
		return;

	unsigned char *ctypes_table=tables+ctypes_offset;

	ctypes_table[c]|=bit;
	if(group_offset>=0)
		tables[cbits_offset+group_offset+c/8] |= 1<<(c%8);
}
static void element2case(unsigned char from, unsigned char to,
						 unsigned char *tables) {
	if(!to) 
		return;

	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	lcc_table[from]=to;
	fcc_table[from]=to; fcc_table[to]=from;
}

// methods

extern "C" unsigned char pcre_default_tables[]; // pcre/chartables.c
Charset::Charset(Pool& apool, const String& aname, const String *request_file_spec): Pooled(apool),
	fname(aname) {

	char *name_cstr=fname.cstr();
    for(char *c=name_cstr; *c; c++)
        *c = toupper(*c);

	if(request_file_spec) {
		fisUTF8=false;
		loadDefinition(*request_file_spec);
#ifdef XML
		addEncoding(name_cstr);
#endif
	} else {
		fisUTF8=true;
		// grab default onces [for UTF-8 so to be able to make a-z =>A-Z
		memcpy(pcre_tables, pcre_default_tables, sizeof(pcre_tables));
	}

#ifdef XML
	initTranscoder(&aname, name_cstr);
#endif
}

Charset::~Charset() {
#ifdef XML
	// not deleting transcoder, that's not our business
#endif
}

void Charset::loadDefinition(const String& request_file_spec) {
	// pcre_tables
	// lowcase, flipcase, bits digit+word+whitespace, masks

	// must not move this inside of prepare_case_tables
	// don't know the size there
	memset(pcre_tables, 0, sizeof(pcre_tables)); 
	prepare_case_tables(pcre_tables);
	cstr2ctypes(pcre_tables,(const unsigned char *)"*+?{^.$|()[", ctype_meta);

	// charset
	memset(tables.fromTable, 0, sizeof(tables.fromTable));
	tables.toTable=(Charset_TransRec *)calloc(sizeof(Charset_TransRec)*MAX_CHARSET_UNI_CODES);
	tables.toTableSize=0;
	// strangly vital
	tables.toTable[tables.toTableSize].intCh=0;
	tables.toTable[tables.toTableSize].extCh=(XMLByte)0;
	tables.toTableSize++;

	// loading text
	char *data=file_read_text(pool(), request_file_spec);

	// ignore header
	getrow(&data);

	// parse cells
	char *row;
	while(row=getrow(&data)) {
		// remove empty&comment lines
		if(!*row || *row=='#')
			continue;

		// char	white-space	digit	hex-digit	letter	word	lowercase	unicode1	unicode2	
		unsigned int c=0;
		char *cell;
		for(int column=0; cell=lsplit(&row, '\t'); column++) {
			switch(column) {
			case 0: c=to_wchar_code(cell); break;
			// pcre_tables
			case 1: element2ctypes(c, to_bool(cell), pcre_tables, ctype_space, cbit_space); break;
			case 2: element2ctypes(c, to_bool(cell), pcre_tables, ctype_digit, cbit_digit); break;
			case 3: element2ctypes(c, to_bool(cell), pcre_tables, ctype_xdigit); break;
			case 4: element2ctypes(c, to_bool(cell), pcre_tables, ctype_letter); break;
			case 5: element2ctypes(c, to_bool(cell), pcre_tables, ctype_word, cbit_word); break;
			case 6: element2case(c, to_wchar_code(cell), pcre_tables); break;
			case 7:
			case 8:
				// charset
				if(tables.toTableSize>MAX_CHARSET_UNI_CODES)
					throw Exception("parser.runtime",
						&request_file_spec,
						"charset must contain not more then %d unicode values", MAX_CHARSET_UNI_CODES);

				XMLCh unicode=(XMLCh)to_wchar_code(cell);
				if(!unicode && column==7/*unicode1 column*/)
					unicode=(XMLCh)c;
				if(unicode) {
					if(!tables.fromTable[c])
						tables.fromTable[c]=unicode;
					tables.toTable[tables.toTableSize].intCh=unicode;
					tables.toTable[tables.toTableSize].extCh=(XMLByte)c;
					tables.toTableSize++;
				}
				break;
			}
		}
	};

	// sort by the Unicode code point
	sort_ToTable();
}

static int sort_cmp_Trans_rec_intCh(const void *a, const void *b) {
	return 
		static_cast<const Charset_TransRec *>(a)->intCh-
		static_cast<const Charset_TransRec *>(b)->intCh;
}

void Charset::sort_ToTable() {
	_qsort(tables.toTable, tables.toTableSize, sizeof(*tables.toTable), 
		sort_cmp_Trans_rec_intCh);
	//FILE *f=fopen("c:\\temp\\a", "wb");
	//fwrite(tables.toTable, tables.toTableSize, sizeof(*tables.toTable), f);
	//fclose(f);
}

static XMLByte xlatOneTo(const XMLCh toXlat,
						 const Charset::Tables& tables,
						 XMLByte not_found) {
    unsigned int    lowOfs = 0;
    unsigned int    hiOfs = tables.toTableSize - 1;
    XMLByte         curByte = 0;
    do {
        // Calc the mid point of the low and high offset.
        const unsigned int midOfs =((hiOfs - lowOfs) / 2)+lowOfs;

        //  If our test char is greater than the mid point char, then
        //  we move up to the upper half. Else we move to the lower
        //  half. If its equal, then its our guy.
        if(toXlat>tables.toTable[midOfs].intCh)
            lowOfs = midOfs;
		else if(toXlat<tables.toTable[midOfs].intCh)
			hiOfs = midOfs;
		else
			return tables.toTable[midOfs].extCh;
	} while(lowOfs+1<hiOfs);

    return not_found;
}

void Charset::transcode(Pool& pool,
	const Charset& source_charset, const void *source_body, size_t source_content_length,
	const Charset& dest_charset, const void *& dest_body, size_t& dest_content_length
	) {
	if(!source_content_length) {
		dest_body=0;
		dest_content_length=0;
		return;
	}

	switch((source_charset.isUTF8()?0x10:0x00)|(dest_charset.isUTF8()?0x01:0x00)) {
		default: // 0x00
			source_charset.transcodeToCharset(pool, dest_charset,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
		case 0x01:
			source_charset.transcodeToUTF8(pool,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
		case 0x10:
			dest_charset.transcodeFromUTF8(pool,
				source_body, source_content_length,
				dest_body, dest_content_length);
			break;
		case 0x11:
			dest_body=source_body;
			dest_content_length=source_content_length;
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

static int transcodeToUTF8(
							const XMLByte* srcData, size_t& srcLen,
							XMLByte *toFill, size_t& toFillLen,
							const Charset::Tables& tables) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte* outEnd=toFill+toFillLen;

    while(srcPtr<srcEnd) {
        uint curVal = tables.fromTable[*srcPtr];
		if(!curVal) {
            // use the replacement character
            *outPtr++= '?';
            srcPtr++;
            continue;
        }

        // Figure out how many bytes we need
        unsigned int encodedBytes;
        if(curVal<0x80)
            encodedBytes = 1;
        else if(curVal<0x800)
            encodedBytes = 2;
        else if(curVal<0x10000)
            encodedBytes = 3;
        else if(curVal<0x200000)
            encodedBytes = 4;
        else if(curVal<0x4000000)
            encodedBytes = 5;
        else if(curVal<= 0x7FFFFFFF)
            encodedBytes = 6;
        else {
            // use the replacement character
            *outPtr++= '?';
            srcPtr++;
            continue;
        }

        //  If we cannot fully get this char into the output buffer
        if (outPtr + encodedBytes > outEnd)
            break;

        // We can do it, so update the source index
        srcPtr++;

        //  And spit out the bytes. We spit them out in reverse order
        //  here, so bump up the output pointer and work down as we go.
        outPtr+= encodedBytes;
        switch(encodedBytes) {
            case 6: *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal>>= 6;
            case 5: *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal>>= 6;
            case 4: *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal>>= 6;
            case 3: *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal>>= 6;
            case 2: *--outPtr = XMLByte((curVal | 0x80UL) & 0xBFUL);
                     curVal>>= 6;
            case 1: *--outPtr = XMLByte(curVal | gFirstByteMark[encodedBytes]);
        }

        // Add the encoded bytes back in again to indicate we've eaten them
        outPtr+= encodedBytes;
    }

    // Update the bytes eaten
    srcLen = srcPtr - srcData;

    // Return the characters read
    toFillLen = outPtr - toFill;

	//return srcPtr==srcEnd?(int)toFillLen:-1;
/*
xmlCharEncodingInputFunc
Returns :
the number of byte written, or -1 by lack of space, or -2 if the transcoding failed. The value of inlen after return is the
number of octets consumed as the return value is positive, else unpredictiable. The value of outlen after return is the number
of ocetes consumed.
*/
	return 0;
}
/// @todo digital entites only when xml/html output [at output in html/xml mode, in html part of a letter]
static size_t transcodeFromUTF8(
								const XMLByte *srcData, size_t& srcLen,
								XMLByte* toFill, size_t& toFillLen,
								const Charset::Tables& tables) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte* outEnd=toFill+toFillLen;

    //  We now loop until we either run out of input data, or room to store
    while ((srcPtr < srcEnd) && (outPtr < outEnd)) {
        // Get the next leading byte out
        const XMLByte firstByte = *srcPtr;

        // Special-case ASCII, which is a leading byte value of<= 127
        if(firstByte<= 127) {
            *outPtr++= firstByte;
            srcPtr++;
            continue;
        }

        // See how many trailing src bytes this sequence is going to require
        const unsigned int trailingBytes = gUTFBytes[firstByte];

        //  If there are not enough source bytes to do this one, then we
        //  are done. Note that we done>= here because we are implicitly
        //  counting the 1 byte we get no matter what.
        if(srcPtr+trailingBytes>= srcEnd)
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
                throw Exception(0,
					0,
					"transcodeFromUTF8 error: wrong trailingBytes value(%d)", trailingBytes);
        }
        tmpVal-=gUTFOffsets[trailingBytes];

        //  If it will fit into a single char, then put it in. Otherwise
        //  fail [*encode it as a surrogate pair. If its not valid, use the
        //  replacement char.*]
        if(!(tmpVal & 0xFFFF0000)) {
			if(XMLByte xlat=xlatOneTo(tmpVal, tables, 0))
				*outPtr++=xlat;
			else 
				outPtr+=sprintf((char *)outPtr, "&#%d;", tmpVal); // &#decimal;
		} else
			throw Exception(0,
				0,
				"transcodeFromUTF8 error: too big tmpVal(0x%08X)", tmpVal);
	}

    // Update the bytes eaten
    srcLen = srcPtr - srcData;

    // Return the characters read
    toFillLen = outPtr - toFill;

	//return srcPtr==srcEnd?(int)toFillLen:-1;
/*
xmlCharEncodingOutputFunc
Returns :
the number of byte written, or -1 by lack of space, or -2 if the transcoding failed. The value of inlen after return is the
number of octets consumed as the return value is positive, else unpredictiable. The value of outlen after return is the number
of ocetes consumed.
*/
	return 0;
}

/// @todo not so memory-hungry with prescan
void Charset::transcodeToUTF8(Pool& pool,
								 const void *source_body, size_t source_content_length,
								 const void *& adest_body, size_t& dest_content_length) const {
	dest_content_length=source_content_length*6/*so that surly enough, max utf8 seq len=6*/;
	XMLByte *dest_body=(XMLByte*)pool.malloc(dest_content_length);

	if(::transcodeToUTF8(
		(XMLByte *)source_body, source_content_length,
		dest_body, dest_content_length,
		tables)<0)
		throw(0, 0,
			0,
			"Charset::transcodeToUTF8 buffer overflow");

	// return
	adest_body=dest_body;
}
void Charset::transcodeFromUTF8(Pool& pool,
								   const void *source_body, size_t source_content_length,
								   const void *& adest_body, size_t& dest_content_length) const {
	dest_content_length=source_content_length*6/*so that surly enough, "&#255;" has max ratio */;
	XMLByte *dest_body=(XMLByte*)pool.malloc(dest_content_length);

	if(::transcodeFromUTF8(
		(XMLByte *)source_body, source_content_length,
		dest_body, dest_content_length,
		tables)<0)
		throw(0, 0,
			0,
			"Charset::transcodeToUTF8 buffer overflow");

	// return
	adest_body=dest_body;
}

/// transcode using both charsets
void Charset::transcodeToCharset(Pool& pool,
									   const Charset& dest_charset,
									   const void *source_body, size_t source_content_length,
									   const void *& adest_body, size_t& adest_content_length) const {
	if(&dest_charset==this) {
		adest_body=source_body;
		adest_content_length=source_content_length;
	} else {
		size_t dest_content_length=source_content_length;
		unsigned char *dest_body=(unsigned char *)pool.malloc(dest_content_length);

		const XMLByte* srcPtr=(XMLByte *)source_body;
		const XMLByte* srcEnd=(XMLByte *)source_body+source_content_length;

		for(XMLByte* outPtr=dest_body; srcPtr<srcEnd; srcPtr++) {
			XMLCh curVal = tables.fromTable[*srcPtr];
			if(curVal) 
				*outPtr++=xlatOneTo(curVal, dest_charset.tables, '?');
			else {
				// use the replacement character
				*outPtr++= '?';
			}	
		}

		adest_body=dest_body;
		adest_content_length=dest_content_length;
	}
}			

#ifdef XML
static int         xml256CharEncodingInputFunc     (
													unsigned char *out,
													int *outlen,
													const unsigned char *in,
													int *inlen, 
													void *info) {
	return transcodeToUTF8(
							in, *(size_t*)inlen,
							out, *(size_t*)outlen,
							*(const Charset::Tables *)info);
}

static int         xml256CharEncodingOutputFunc    (
													unsigned char *out,
													int *outlen,
													const unsigned char *in,
													int *inlen, 
													void *info) {
	return transcodeFromUTF8(
							in, *(size_t*)inlen,
							out, *(size_t*)outlen,
							*(const Charset::Tables *)info);
}


void Charset::addEncoding(char *name_cstr) {
	xmlCharEncodingHandler *handler=
		(xmlCharEncodingHandler *)malloc(sizeof(xmlCharEncodingHandler));
	handler->name=name_cstr;
	handler->input=xml256CharEncodingInputFunc; handler->inputInfo=&tables;
	handler->output=xml256CharEncodingOutputFunc; handler->outputInfo=&tables;
	
	xmlRegisterCharEncodingHandler(handler);
}

void Charset::initTranscoder(const String *source, const char *name_cstr) {
	ftranscoder=xmlFindCharEncodingHandler(name_cstr);
	transcoder(source); // check right way
}

xmlCharEncodingHandler *Charset::transcoder(const String *source) {
	if(!ftranscoder)
		throw Exception("parser.runtime",
			source,
			"unsupported encoding");
	return ftranscoder;
}

const char *Charset::transcode_cstr(xmlChar *s) {
	if(!s)
		return "";

	int inlen=strlen((const char *)s);
	int outlen=inlen+1; // max
	char *out=(char *)malloc(outlen*sizeof(char));
	
	int size;
	if(xmlCharEncodingOutputFunc output=transcoder(0)->output) {
		size=output(
			(unsigned char*)out, &outlen,
			(const unsigned char*)s, &inlen,
			transcoder(0)->outputInfo);
	} else
		memcpy(out, s, size=inlen);
	if(size<0)
		throw Exception(0,
			0,
			"transcode_cstr failed (%d)", size);

	out[size]=0;
	return out;
}
String& Charset::transcode(xmlChar *s) { 
	return *NEW String(pool(), transcode_cstr(s)); 
}
const char *Charset::transcode_cstr(GdomeDOMString *s) { 
	return s?transcode_cstr(BAD_CAST s->str):"";
}
String& Charset::transcode(GdomeDOMString *s) { 
	return *NEW String(pool(), transcode_cstr(s)); 
}

/// @test less memory using -maybe- xmlParserInputBufferCreateMem
xmlChar *Charset::transcode_buf2xchar(const char *buf, size_t buf_size) {
	int outlen=buf_size*6/*max*/+1;
	unsigned char *out=(unsigned char*)malloc(outlen*sizeof(unsigned char));

	int size;
	if(xmlCharEncodingInputFunc input=transcoder(0)->input) {
		size=input(
			out, &outlen,
			(const unsigned char *)buf,  (int *)&buf_size,
			transcoder(0)->inputInfo);
	} else
		memcpy(out, buf, size=buf_size);
	
	if(size<0)
		throw Exception(0,
			0,
			"transcode_buf failed (%d)", size);

	out[size]=0;
	return (xmlChar *)out;
}
GdomeDOMString_auto_ptr Charset::transcode_buf2dom(const char *buf, size_t buf_size) { 
	return GdomeDOMString_auto_ptr((gchar*)transcode_buf2xchar(buf, buf_size));
}
GdomeDOMString_auto_ptr Charset::transcode(const String& s) { 
	const char *cstr=s.cstr(String::UL_UNSPECIFIED);

	return transcode_buf2dom(cstr, strlen(cstr)); 
}
#endif
