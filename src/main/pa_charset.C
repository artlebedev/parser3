/** @file
	Parser: Charset connection implementation.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan<paf@design.ru>(http://paf.design.ru)
*/

static const char * const IDENT_CHARSET_C="$Date: 2009/04/22 21:29:17 $";

#include "pa_charset.h"
#include "pa_charsets.h"

#ifdef XML
#include "libxml/encoding.h"
#endif

//#define PA_PATCHED_LIBXML_BACKWARD

// reduce memory usage by pre-calculation utf-8 string length
#define PRECALCULATE_DEST_LENGTH

// globals

Charset::UTF8CaseTable::Rec UTF8CaseToUpperRecords[]={
#include "utf8-to-upper.inc"
};
Charset::UTF8CaseTable UTF8CaseToUpper={
	sizeof(UTF8CaseToUpperRecords)/sizeof(Charset::UTF8CaseTable::Rec),
	UTF8CaseToUpperRecords};

Charset::UTF8CaseTable::Rec UTF8CaseToLowerRecords[]={
#include "utf8-to-lower.inc"
};
Charset::UTF8CaseTable UTF8CaseToLower={
	sizeof(UTF8CaseToLowerRecords)/sizeof(Charset::UTF8CaseTable::Rec),
	UTF8CaseToLowerRecords};

// helpers

inline void prepare_case_tables(unsigned char *tables) {
	unsigned char *lcc_table=tables+lcc_offset;
	unsigned char *fcc_table=tables+fcc_offset;
	for(int i=0; i<0x100; i++)
		lcc_table[i]=fcc_table[i]=(unsigned char)i;
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
inline unsigned int to_wchar_code(const char* cstr) {
	if(!cstr || !*cstr)
		return 0;
	if(cstr[1]==0)
		return(unsigned int)(unsigned char)cstr[0];

	char *error_pos;
	return(unsigned int)strtol(cstr, &error_pos, 0);
}
inline bool to_bool(const char* cstr) {
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

Charset::Charset(Request_charsets* charsets, const String::Body ANAME, const String* afile_spec): 
	FNAME(ANAME),
	FNAME_CSTR(ANAME.cstrm()) {

	if(afile_spec) {
		fisUTF8=false;
		load_definition(*charsets, *afile_spec);
#ifdef XML
		addEncoding(FNAME_CSTR);
#endif
	} else {
		fisUTF8=true;
		// grab default onces [for UTF-8 so to be able to make a-z =>A-Z
		memcpy(pcre_tables, _pcre_default_tables, sizeof(pcre_tables));
	}

#ifdef XML
	initTranscoder(FNAME, FNAME_CSTR);
#endif
}

void Charset::load_definition(Request_charsets& charsets, const String& afile_spec) {
	// pcre_tables
	// lowcase, flipcase, bits digit+word+whitespace, masks

	// must not move this inside of prepare_case_tables
	// don't know the size there
	memset(pcre_tables, 0, sizeof(pcre_tables)); 
	prepare_case_tables(pcre_tables);
	cstr2ctypes(pcre_tables,(const unsigned char *)"*+?{^.$|()[", ctype_meta);

	// charset
	memset(&tables, 0, sizeof(tables));

	// loading text
	char *data=file_read_text(charsets, afile_spec);

	// ignore header
	getrow(&data);

	// parse cells
	char *row;
	while((row=getrow(&data))) {
		// remove empty&comment lines
		if(!*row || *row=='#')
			continue;

		// char	white-space	digit	hex-digit	letter	word	lowercase	unicode1	unicode2	
		unsigned char c=0;
		char *cell;
		for(int column=0; (cell=lsplit(&row, '\t')); column++) {
			switch(column) {
			case 0: c=(unsigned char)to_wchar_code(cell); break;
			// pcre_tables
			case 1: element2ctypes(c, to_bool(cell), pcre_tables, ctype_space, cbit_space); break;
			case 2: element2ctypes(c, to_bool(cell), pcre_tables, ctype_digit, cbit_digit); break;
			case 3: element2ctypes(c, to_bool(cell), pcre_tables, ctype_xdigit); break;
			case 4: element2ctypes(c, to_bool(cell), pcre_tables, ctype_letter); break;
			case 5: element2ctypes(c, to_bool(cell), pcre_tables, ctype_word, cbit_word); break;
			case 6: element2case(c, (unsigned char)to_wchar_code(cell), pcre_tables); break;
			case 7:
			case 8:
				// charset
				if(tables.toTableSize>MAX_CHARSET_UNI_CODES)
					throw Exception(PARSER_RUNTIME,
						&afile_spec,
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
		static_cast<const Charset::Tables::Rec *>(a)->intCh-
		static_cast<const Charset::Tables::Rec *>(b)->intCh;
}

void Charset::sort_ToTable() {
	_qsort(tables.toTable, tables.toTableSize, sizeof(*tables.toTable), 
		sort_cmp_Trans_rec_intCh);
	//FILE *f=fopen("c:\\temp\\a", "wb");
	//fwrite(tables.toTable, tables.toTableSize, sizeof(*tables.toTable), f);
	//fclose(f);
}

// @todo: precache for spedup searching
static XMLByte xlatOneTo(const XMLCh toXlat,
			 const Charset::Tables& tables,
			 XMLByte not_found) {
	int    lo = 0;
	int    hi = tables.toTableSize - 1;
	while(lo<=hi) {
		// Calc the mid point of the low and high offset.
		const unsigned int i = (lo + hi) / 2;

		XMLCh cur=tables.toTable[i].intCh;
		if(toXlat==cur)
			return tables.toTable[i].extCh;
		if(toXlat>cur)
			lo = i+1;
		else
			hi = i-1;
	}
	
	return not_found;
}

String::C Charset::transcode(const String::C src,
	const Charset& source_charset, 
	const Charset& dest_charset) {
	if(!src.length)
		return String::C("", 0);

	switch((source_charset.isUTF8()?0x10:0x00)|(dest_charset.isUTF8()?0x01:0x00)) {
		default: // 0x00
			return source_charset.transcodeToCharset(src, dest_charset);
		case 0x01:
			return source_charset.transcodeToUTF8(src);
		case 0x10:
			return dest_charset.transcodeFromUTF8(src);
		case 0x11:
			return src;
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

static int transcodeToUTF8(const XMLByte* srcData, int& srcLen,
				XMLByte *toFill, int& toFillLen,
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
static int transcodeFromUTF8(const XMLByte* srcData, int& srcLen,
				XMLByte* toFill, int& toFillLen,
				const Charset::Tables& tables) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte* outEnd=toFill+toFillLen;

	//  We now loop until we either run out of input data, or room to store
	while ((srcPtr < srcEnd) && (outPtr < outEnd)) {
		// Get the next leading byte out
		const XMLByte firstByte =* srcPtr;
		
		// Special-case ASCII, which is a leading byte value of<= 127
		if(firstByte<=127) {
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
				"transcodeFromUTF8 error: wrong trailingBytes value(%d)", trailingBytes); // never
		}
		tmpVal-=gUTFOffsets[trailingBytes];
		
		//  If it will fit into a single char, then put it in. Otherwise
		//  fail [*encode it as a surrogate pair. If its not valid, use the
		//  replacement char.*]
		if(!(tmpVal & 0xFFFF0000)) {
			if(XMLByte xlat=xlatOneTo(tmpVal, tables, 0))
				*outPtr++=xlat;
			else {
				outPtr+=sprintf((char *)outPtr, "&#%u;", tmpVal); // &#decimal;
			}
		} else {
			const XMLByte* recoverPtr=srcPtr-trailingBytes-1;
			for(uint i=0; i<=trailingBytes; i++)
				outPtr+=sprintf((char*)outPtr, "%%%02X", *recoverPtr++);
		}
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

static bool isEscaped(XMLByte c){
	return
		!(
			(c<=127)
			&& (
				((c>='0') && (c<='9'))
				|| ((c>='A') && (c<='Z'))
				|| ((c>='a') && (c<='z'))
				|| strchr("*@-_+./", c)!=0
			)
		);
}

// read one UTF8 char and return length of this char (in bytes)
static unsigned int readUTF8Char(const XMLByte*& srcPtr, const XMLByte* srcEnd, XMLByte& firstByte, XMLCh& UTF8Char){
	if(!srcPtr || !*srcPtr || srcPtr>=srcEnd)
		return 0;

	firstByte=*srcPtr;

	if(firstByte<=127){
		UTF8Char=firstByte;
		srcPtr++;
		return 1;
	}

	unsigned int trailingBytes=gUTFBytes[firstByte];

	if(srcPtr+trailingBytes>=srcEnd){
		return 0; // not enough bytes in source string for reading
	}

	uint tmpVal=0;
	switch(trailingBytes){
		case 5: tmpVal+=*srcPtr++; tmpVal<<=6;
		case 4: tmpVal+=*srcPtr++; tmpVal<<=6;
		case 3: tmpVal+=*srcPtr++; tmpVal<<=6;
		case 2: tmpVal+=*srcPtr++; tmpVal<<=6;
		case 1: tmpVal+=*srcPtr++; tmpVal<<=6;
		case 0: tmpVal+=*srcPtr++;
	}

	tmpVal-=gUTFOffsets[trailingBytes];
	UTF8Char=tmpVal;

	return trailingBytes+1;
}

// skip UTF8 char and return length of this char (in bytes)
static unsigned int skipUTF8Char(const XMLByte*& srcPtr, const XMLByte* srcEnd){
	if(!srcPtr || !*srcPtr || srcPtr>=srcEnd)
		return 0;

	unsigned int trailingBytes=gUTFBytes[*srcPtr]+1;
	srcPtr+=trailingBytes;

	return trailingBytes;
}

// read non-UTF8 char, and return number of bytes needed for store this char in UTF8
static unsigned int readChar(const XMLByte*& srcPtr, const XMLByte* srcEnd, XMLByte& firstByte, XMLCh& UTF8Char, const Charset::Tables& tables){
	if(!srcPtr || !*srcPtr || srcPtr>=srcEnd)
		return 0;

	firstByte=*srcPtr++;
	UTF8Char=tables.fromTable[firstByte];

	if(UTF8Char<0x80)
		return 1;
	else if(UTF8Char<0x800)
		return 2;
	else if(UTF8Char<0x10000)
		return 3;
	else if(UTF8Char<0x200000)
		return 4;
	else if(UTF8Char<0x4000000)
		return 5;
	else if(UTF8Char<= 0x7FFFFFFF)
		return 6;

	// will use the replacement character '?'
	firstByte=0;
	return 1;
}

static int escapeUTF8(const XMLByte* srcData, size_t& srcLen,
			     XMLByte* toFill, size_t& toFillLen) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte* outEnd=toFill+toFillLen;
	XMLByte firstByte;
	XMLCh UTF8Char;
	uint charSize;

	// loop until we either run out of input data, or room to store
	while((outPtr < outEnd) && (charSize=readUTF8Char(srcPtr, srcEnd, firstByte, UTF8Char))){
		if(charSize==1){
			if(isEscaped(firstByte)) // %XX
				outPtr+=sprintf((char*)outPtr, "%%%02X", firstByte);
			else
				*outPtr++=firstByte;
		} else
			outPtr+=sprintf((char*)outPtr, "%%u%04X", UTF8Char); // %uXXXX
	}
	
	// Update the bytes eaten
	srcLen=srcPtr-srcData;
	
	// Return the characters read
	toFillLen=outPtr-toFill;

	return 0;
}

static int escape(const XMLByte* srcData, size_t& srcLen,
			   XMLByte *toFill, size_t& toFillLen,
			   const Charset::Tables& tables) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte firstByte;
	XMLCh UTF8Char;
	uint charSize;

	while(charSize=readChar(srcPtr, srcEnd, firstByte, UTF8Char, tables)){
		if(charSize==1){
			if(firstByte){
				if(isEscaped(firstByte)) // %XX
					outPtr+=sprintf((char*)outPtr, "%%%02X", firstByte);
				else
					*outPtr++=firstByte;
			} else // add replacement char '?'
				*outPtr++='?';
		} else
			outPtr+=sprintf((char*)outPtr, "%%u%04X", UTF8Char); // %uXXXX
	}

	// Update the bytes eaten
	srcLen = srcPtr - srcData;
	
	// Return the characters read
	toFillLen = outPtr - toFill;
	
	return 0;
}


String::C Charset::escape(const String::C src, const Charset& source_charset){
	size_t src_length=src.length;
	if(!src_length)
		return String::C("", 0);

#ifdef PRECALCULATE_DEST_LENGTH
	size_t dest_length=0;
	const XMLByte* srcPtr=(XMLByte*)src.str;
	const XMLByte* srcEnd=srcPtr+src_length;
	XMLByte firstByte;
	XMLCh UTF8Char;

	if(source_charset.isUTF8()){
		while(uint charSize=readUTF8Char(srcPtr, srcEnd, firstByte, UTF8Char)){
			if(charSize==1)
				dest_length+=!isEscaped(firstByte)?1:3/*%XX*/;
			else
				dest_length+=6; // '%uXXXX'
		}
	} else {
		while(uint charSize=readChar(srcPtr, srcEnd, firstByte, UTF8Char, source_charset.tables)){
			if(charSize==1)
				dest_length+=(!firstByte/*replacement char '?'*/ || !isEscaped(firstByte))?1:3/*'%XX'*/;
			else
				dest_length+=6; // '%uXXXX'
		}
	}
#else
	size_t dest_length=src_length*6; // enough for %uXXXX but too memory-hungry
#endif

	//throw Exception(0,0,"%u",dest_length);

#ifndef NDEBUG
	size_t saved_dest_length=dest_length;
#endif
	XMLByte *dest_body=new(PointerFreeGC) XMLByte[dest_length+1/*for terminator*/];

	int status;
	if(source_charset.isUTF8()){
		status=::escapeUTF8((XMLByte *)src.str, src_length, dest_body, dest_length);
	} else {
		status=::escape((XMLByte *)src.str, src_length, dest_body, dest_length, source_charset.tables);
	}

	if(status<0)
		throw Exception(0,
			0,
			"Charset::escape buffer overflow");

	assert(dest_length<=saved_dest_length);
	dest_body[dest_length]=0; // terminator
	return String::C((char*)dest_body, dest_length);
}
	
String::Body Charset::escape(const String::Body src, const Charset& source_charset) {
	const char *src_ptr=src.cstr();
	size_t src_size=strlen(src_ptr);

	String::C dest=Charset::escape(String::C(src_ptr, src_size), source_charset);

	return String::Body(dest.str, dest.length);
}

String& Charset::escape(const String& src, const Charset& source_charset) {
	if(!src.length())
		return *new String("", 0, false);

	return *new String(escape((String::Body)src, source_charset), String::L_CLEAN);
}

const String::C Charset::transcodeToUTF8(const String::C src) const {
	int src_length=src.length;

#ifdef PRECALCULATE_DEST_LENGTH
	int dest_length=0;
	const XMLByte* srcPtr=(XMLByte*)src.str;
	const XMLByte* srcEnd=srcPtr+src_length;
 	XMLByte firstByte;
 	XMLCh UTF8Char;
	while(uint charSize=readChar(srcPtr, srcEnd, firstByte, UTF8Char, tables))
		dest_length+=charSize;
#else
	int dest_length=src_length*6; // so that surly enough (max utf8 seq len=6) but too memory-hyngry
#endif

	//throw Exception(0,0,"%u",dest_length);

#ifndef NDEBUG
	int saved_dest_length=dest_length;
#endif
	XMLByte *dest_body=new(PointerFreeGC) XMLByte[dest_length+1/*for terminator*/];

	if(::transcodeToUTF8(
		(XMLByte *)src.str, src_length,
		dest_body, dest_length,
		tables)<0)
		throw Exception(0,
			0,
			"Charset::transcodeToUTF8 buffer overflow");

	assert(dest_length<=saved_dest_length);
	dest_body[dest_length]=0; // terminator
	return String::C((char*)dest_body, dest_length);
}

static XMLCh change_case_UTF8(const XMLCh src, const Charset::UTF8CaseTable& table) {
	int    lo = 0;
	int    hi = table.size - 1;
	while(lo<=hi) {
		// Calc the mid point of the low and high offset.
		const unsigned int i = (lo + hi) / 2;

		XMLCh cur=table.records[i].from;
		if(src==cur)
			return table.records[i].to;
		if(src>cur)
			lo = i+1;
		else
			hi = i-1;
	}

	// not found
	return src;
}

static void store_UTF8(XMLCh src, XMLByte*& outPtr){
	if(!src) {
		// use the replacement character
		*outPtr++= '?';
		return;
	}

	// Figure out how many bytes we need
	unsigned int encodedBytes;
	if(src<0x80)
		encodedBytes = 1;
	else if(src<0x800)
		encodedBytes = 2;
	else if(src<0x10000)
		encodedBytes = 3;
	else if(src<0x200000)
		encodedBytes = 4;
	else if(src<0x4000000)
		encodedBytes = 5;
	else if(src<= 0x7FFFFFFF)
		encodedBytes = 6;
	else {
		// use the replacement character
		*outPtr++= '?';
		return;
	}

	//  And spit out the bytes. We spit them out in reverse order
	//  here, so bump up the output pointer and work down as we go.
	outPtr+= encodedBytes;
	switch(encodedBytes) {
	case 6: *--outPtr = XMLByte((src | 0x80UL) & 0xBFUL);
		src>>= 6;
	case 5: *--outPtr = XMLByte((src | 0x80UL) & 0xBFUL);
		src>>= 6;
	case 4: *--outPtr = XMLByte((src | 0x80UL) & 0xBFUL);
		src>>= 6;
	case 3: *--outPtr = XMLByte((src | 0x80UL) & 0xBFUL);
		src>>= 6;
	case 2: *--outPtr = XMLByte((src | 0x80UL) & 0xBFUL);
		src>>= 6;
	case 1: *--outPtr = XMLByte(src | gFirstByteMark[encodedBytes]);
	}
	
	// Add the encoded bytes back in again to indicate we've eaten them
	outPtr+= encodedBytes;
}

static void change_case_UTF8(XMLCh src, XMLByte*& outPtr, 
						const Charset::UTF8CaseTable& table) {
	store_UTF8(change_case_UTF8(src, table), outPtr);
};
void change_case_UTF8(const XMLByte* srcData, size_t srcLen,
					  XMLByte* toFill, size_t toFillLen,
					  const Charset::UTF8CaseTable& table) {
	const XMLByte* srcPtr=srcData;
	const XMLByte* srcEnd=srcData+srcLen;
	XMLByte* outPtr=toFill;
	XMLByte* outEnd=toFill+toFillLen;

	//  We now loop until we either run out of input data, or room to store
	while ((srcPtr < srcEnd) && (outPtr < outEnd)) {
		// Get the next leading byte out
		const XMLByte firstByte =* srcPtr;

		if(firstByte<=127) {
			change_case_UTF8(firstByte, outPtr, table);
			srcPtr++;
			continue;
		}
		
		// See how many trailing src bytes this sequence is going to require
		const unsigned int trailingBytes = gUTFBytes[firstByte];
		
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
				"change_case_UTF8 error: wrong trailingBytes value(%d)", trailingBytes);
		}
		tmpVal-=gUTFOffsets[trailingBytes];
		
		//  If it will fit into a single char, then put it in. Otherwise
		//  fail [*encode it as a surrogate pair. If its not valid, use the
		//  replacement char.*]
		if(!(tmpVal & 0xFFFF0000))
			change_case_UTF8(tmpVal, outPtr, table);
		else
			throw Exception(0,
				0,
				"change_case_UTF8 error: too big tmpVal(0x%08X)", tmpVal);
	}
	
	if(srcPtr!=outPtr)
		throw Exception(0,  
			0,
			"change_case_UTF8 error: end pointers do not match");
}

static size_t getDecNumLength(XMLCh UTF8Char){
	return
		(UTF8Char < 100)
			?2
			:(UTF8Char < 1000)
				?3
				:(UTF8Char < 10000)
					?4
					:5;
}

const String::C Charset::transcodeFromUTF8(const String::C src) const {
	int src_length=src.length;

#ifdef PRECALCULATE_DEST_LENGTH
	int dest_length=0;
	const XMLByte* srcPtr=(XMLByte*)src.str;
	const XMLByte* srcEnd=srcPtr+src_length;
	XMLByte firstByte;
	XMLCh UTF8Char;
	while(uint charSize=readUTF8Char(srcPtr, srcEnd, firstByte, UTF8Char)){
		if(charSize==1)
			dest_length++;
		else
			dest_length+=(UTF8Char & 0xFFFF0000)
							?charSize*3	// '%XX' for each byte
							:(xlatOneTo(UTF8Char, tables, 0)!=0)
								?1		// can convert it to single char
								:getDecNumLength(UTF8Char)+3;		// &#XX; - &#XXXXX;
	}
#else
	// so that surly enough, "&#XXX;" has max ratio (huh? 8 bytes needed for '&#XXXXX;')
	int dest_length=src_length*6;
#endif

#ifndef NDEBUG
	int saved_dest_length=dest_length;
#endif
	XMLByte *dest_body=new(PointerFreeGC) XMLByte[dest_length+1/*for terminator*/];

	if(::transcodeFromUTF8(
		(XMLByte *)src.str, src_length,
		dest_body, dest_length,
		tables)<0)
		throw Exception(0, 
			0,
			"Charset::transcodeFromUTF8 buffer overflow");

	assert(dest_length<=saved_dest_length);
	dest_body[dest_length]=0; // terminator
	return String::C((char*)dest_body, dest_length);
}

/// transcode using both charsets
const String::C Charset::transcodeToCharset(const String::C src, 
					    const Charset& dest_charset) const {
	if(&dest_charset==this) 
		return src;
	else {
		size_t dest_length=src.length;
		XMLByte* dest_body=new(PointerFreeGC) XMLByte[dest_length+1/*for terminator*/];

		XMLByte* output=dest_body;
		const XMLByte* input=(XMLByte *)src.str;
		while(XMLCh c=*input++) {
			XMLCh curVal = tables.fromTable[c];
			*output++=curVal?
				xlatOneTo(curVal, dest_charset.tables, '?') // OK
				:'?'; // use the replacement character
		}

		dest_body[dest_length]=0; // terminator
		return String::C((char*)dest_body, dest_length);
	}
}			

void Charset::store_Char(XMLByte*& outPtr, XMLCh src, XMLByte not_found){
	if(isUTF8())
		store_UTF8(src, outPtr);
	else if(char ch=xlatOneTo(src, tables, not_found))
			*outPtr++=ch;
}

#ifdef XML

static const Charset::Tables* tables[MAX_CHARSETS];

#ifdef PA_PATCHED_LIBXML_BACKWARD

#define declareXml256ioFuncs(i) \
	static int xml256CharEncodingInputFunc##i( \
		unsigned char *out, int *outlen, \
		const unsigned char *in, int *inlen, void*) { \
		return transcodeToUTF8( \
			in, *inlen, \
			out, *outlen, \
			*tables[i]); \
	} \
	static int xml256CharEncodingOutputFunc##i( \
		unsigned char *out, int *outlen, \
		const unsigned char *in, int *inlen, void*) { \
		return transcodeFromUTF8( \
			in, *inlen, \
			out, *outlen, \
			*tables[i]); \
	}

#else

#define declareXml256ioFuncs(i) \
	static int xml256CharEncodingInputFunc##i( \
		unsigned char *out, int *outlen, \
		const unsigned char *in, int *inlen) { \
		return transcodeToUTF8( \
			in, *inlen, \
			out, *outlen, \
			*tables[i]); \
	} \
	static int xml256CharEncodingOutputFunc##i( \
		unsigned char *out, int *outlen, \
		const unsigned char *in, int *inlen) { \
		return transcodeFromUTF8( \
			in, *inlen, \
			out, *outlen, \
			*tables[i]); \
	}

#endif


declareXml256ioFuncs(0)	declareXml256ioFuncs(1)
declareXml256ioFuncs(2)	declareXml256ioFuncs(3)
declareXml256ioFuncs(4)	declareXml256ioFuncs(5)
declareXml256ioFuncs(6)	declareXml256ioFuncs(7)
declareXml256ioFuncs(8)	declareXml256ioFuncs(9)

static xmlCharEncodingInputFunc inputFuncs[MAX_CHARSETS]={
	xml256CharEncodingInputFunc0,	xml256CharEncodingInputFunc1,
	xml256CharEncodingInputFunc2,	xml256CharEncodingInputFunc3,
	xml256CharEncodingInputFunc4,	xml256CharEncodingInputFunc5,
	xml256CharEncodingInputFunc6,	xml256CharEncodingInputFunc7,
	xml256CharEncodingInputFunc8,	xml256CharEncodingInputFunc9
};
static xmlCharEncodingOutputFunc outputFuncs[MAX_CHARSETS]={
	xml256CharEncodingOutputFunc0,	xml256CharEncodingOutputFunc1,
	xml256CharEncodingOutputFunc2,	xml256CharEncodingOutputFunc3,
	xml256CharEncodingOutputFunc4,	xml256CharEncodingOutputFunc5,
	xml256CharEncodingOutputFunc6,	xml256CharEncodingOutputFunc7,
	xml256CharEncodingOutputFunc8,	xml256CharEncodingOutputFunc9
};
static size_t handlers_count=0;

void Charset::addEncoding(char *name_cstr) {
	if(handlers_count==MAX_CHARSETS)
		throw Exception(0,
			0,
			"already allocated %d handlers, no space for new encoding '%s'",
				MAX_CHARSETS, name_cstr);

	xmlCharEncodingHandler* handler=new(UseGC) xmlCharEncodingHandler;
	{
		handler->name=name_cstr;
		handler->input=inputFuncs[handlers_count]; 
		handler->output=outputFuncs[handlers_count]; 
		::tables[handlers_count]=&tables;
		handlers_count++;
	}
	
	xmlRegisterCharEncodingHandler(handler);

}

void Charset::initTranscoder(const String::Body NAME, const char* name_cstr) {
	ftranscoder=xmlFindCharEncodingHandler(name_cstr);
	transcoder(NAME); // check right way
}

xmlCharEncodingHandler& Charset::transcoder(const String::Body NAME) {
	if(!ftranscoder)
		throw Exception(PARSER_RUNTIME,
			new String(NAME, String::L_TAINTED),
			"unsupported encoding");
	return *ftranscoder;
}

String::C Charset::transcode_cstr(const xmlChar* s) {
	if(!s)
		return String::C("", 0);

	int inlen=strlen((const char*)s);
	int outlen=inlen*6/*strlen("&#255;")*/; // max
#ifndef NDEBUG
	int saved_outlen=outlen;
#endif
	char *out=new(PointerFreeGC) char[outlen+1];
	
	int error;
	if(xmlCharEncodingOutputFunc output=transcoder(FNAME).output) {
		error=output(
			(unsigned char*)out, &outlen,
			(const unsigned char*)s, &inlen
#ifdef PA_PATCHED_LIBXML_BACKWARD
			,0
#endif
			);
	} else {
		memcpy(out, s, outlen=inlen);
		error=0;
	}
	if(error<0)
		throw Exception(0,
			0,
			"transcode_cstr failed (%d)", error);

	assert(outlen<=saved_outlen); out[outlen]=0;
	return String::C(out, outlen);
}
const String& Charset::transcode(const xmlChar* s) { 
	String::C cstr=transcode_cstr(s);
	return *new String(cstr.str, cstr.length, true);
}

/// @test less memory using -maybe- xmlParserInputBufferCreateMem
xmlChar* Charset::transcode_buf2xchar(const char* buf, size_t buf_size) {
	xmlChar* out;
	int outlen;
	int error;
#ifndef NDEBUG
	int saved_outlen;
#endif
	if(xmlCharEncodingInputFunc input=transcoder(FNAME).input) {
		outlen=buf_size*6/*max UTF8 bytes per char*/;
#ifndef NDEBUG
		saved_outlen=outlen;
#endif
		out=(xmlChar*)xmlMalloc(outlen+1);
		error=input(
			out, &outlen,
			(const unsigned char*)buf, (int*)&buf_size
#ifdef PA_PATCHED_LIBXML_BACKWARD
			,0
#endif
			);
	} else {
		outlen=buf_size;
#ifndef NDEBUG
		saved_outlen=outlen;
#endif
		out=(xmlChar*)xmlMalloc(outlen+1);
		memcpy(out, buf, outlen);
		error=0;
	}
	
	if(error<0)
		throw Exception(0,
			0,
			"transcode_buf failed (%d)", error);

	assert(outlen<=saved_outlen); out[outlen]=0;
	return out;
}
xmlChar* Charset::transcode(const String& s) { 
	const char* cstr=s.cstr(String::L_UNSPECIFIED);

	return transcode_buf2xchar(cstr, strlen(cstr)); 
}
xmlChar* Charset::transcode(const String::Body s) { 
	const char* cstr=s.cstr();

	return transcode_buf2xchar(cstr, s.length()); 
}
#endif

String::Body Charset::transcode(const String::Body src, 
	const Charset& source_transcoder, 
	const Charset& dest_transcoder) {

	const char *src_ptr=src.cstr();
	size_t src_size=strlen(src_ptr);

	String::C dest=Charset::transcode(String::C(src_ptr, src_size),
		source_transcoder,
		dest_transcoder);

	return String::Body(dest.str, dest.length);
}

String& Charset::transcode(const String& src, 
	const Charset& source_transcoder, 
	const Charset& dest_transcoder) {
	if(!src.length())
		return *new String("", 0, false);

	return *new String(transcode((String::Body)src, source_transcoder, dest_transcoder), String::L_CLEAN);
}

void Charset::transcode(ArrayString& src,
	const Charset& source_transcoder, 
	const Charset& dest_transcoder) {
	for(size_t i=0; i<src.count(); i++)
		src.put(i, &transcode(*src[i], source_transcoder, dest_transcoder));
}

#ifndef DOXYGEN
struct Transcode_pair_info {
	const Charset* source_transcoder;
	const Charset* dest_transcoder;
};
#endif
static void transcode_pair(const String::Body /*akey*/, 
			 String::Body& avalue, 
			 Transcode_pair_info* info) {
	avalue=Charset::transcode(avalue,
		*info->source_transcoder, 
		*info->dest_transcoder);
}

void Charset::transcode(HashStringString& src,
	const Charset& source_transcoder, 
	const Charset& dest_transcoder) {
	Transcode_pair_info info={&source_transcoder, &dest_transcoder};
	src.for_each_ref<Transcode_pair_info*>(transcode_pair, &info);
}

size_t getUTF8BytePos(const XMLByte* srcBegin, const XMLByte* srcEnd, size_t charPos){
	const XMLByte* ptr=srcBegin;
	while(charPos-- && skipUTF8Char(ptr, srcEnd));

	return ptr-srcBegin;
}

size_t getUTF8CharPos(const XMLByte* srcBegin, const XMLByte* srcEnd, size_t bytePos){
	size_t charPos=0;
	const XMLByte* ptr=srcBegin;
	const XMLByte* ptrEnd=srcBegin+bytePos;
	while(skipUTF8Char(ptr, srcEnd)){
		if(ptr>ptrEnd)
			return charPos;
		charPos++;
	}

	// scan till end but position in bytes still too low
	throw Exception(0,
					0,
					"Error convertion byte pos to char pos");
}

size_t lengthUTF8(const XMLByte* srcBegin, const XMLByte* srcEnd){
	size_t size=0;
	while(skipUTF8Char(srcBegin, srcEnd))
		size++;

	return size;
}
