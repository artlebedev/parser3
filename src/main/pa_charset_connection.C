/** @file
	Parser: Charset connection implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_charset_connection.C,v 1.8 2001/10/05 10:36:43 parser Exp $
*/

#include "pa_charset_connection.h"
//#include "pa_exception.h"
//#include "pa_common.h"
//#include "pa_threads.h"

#ifdef XML
#	include <util/XercesDefs.hpp>
#	include <util/TransENameMap.hpp>
#	include <util/XML256TableTranscoder.hpp>
#	include <util/PlatformUtils.hpp>
#	include <PlatformSupport/XalanTranscodingServices.hpp>
#endif


// globals


// consts

#define MAX_CHARSET_UNI_CODES 500

//

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
		return (unsigned int)(unsigned char)cstr[0];

	char *error_pos;
	return (unsigned int)strtol(cstr, &error_pos, 0);
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
		tables[cbits_offset+group_offset+c/8] |= 1 << (c%8);
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

#ifdef XML

static int sort_cmp_Trans_rec_intCh(const void *a, const void *b) {
	return 
		static_cast<const XMLTransService::TransRec *>(a)->intCh-
		static_cast<const XMLTransService::TransRec *>(b)->intCh;
}

template <class TType> class ENameMapFor2 : public ENameMap
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    ENameMapFor2(
		const XMLCh* const encodingName
        , const XMLCh* const                        fromTable
        , const XMLTransService::TransRec* const    toTable
        , const unsigned int                        toTableSize
		) : ENameMap(encodingName),
		ffromTable(fromTable),
		ftoTable(toTable),
		ftoTableSize(toTableSize) {}

    // -----------------------------------------------------------------------
    //  Implementation of virtual factory method
    // -----------------------------------------------------------------------
    virtual XMLTranscoder* makeNew(const unsigned int blockSize) const {
		return new TType(
			getKey(), 
			blockSize,
			ffromTable,
			ftoTable, ftoTableSize);
	}
private:
	const XMLCh* const                        ffromTable;
	const XMLTransService::TransRec* const    ftoTable;
	const unsigned int                        ftoTableSize;

private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    ENameMapFor2();
    ENameMapFor2(const ENameMapFor2<TType>&);
    void operator=(const ENameMapFor2<TType>&);
};

class XML256TableTranscoder2 : public XML256TableTranscoder
{
public :
    XML256TableTranscoder2(
        const   XMLCh* const                        encodingName
        , const unsigned int                        blockSize
        , const XMLCh* const                        fromTable
        , const XMLTransService::TransRec* const    toTable
        , const unsigned int                        toTableSize
		) : XML256TableTranscoder(encodingName, blockSize, fromTable, toTable, toTableSize) {}

private :
    XML256TableTranscoder2();
    XML256TableTranscoder2(const XML256TableTranscoder2&);
    void operator=(const XML256TableTranscoder2&);
};
#endif

void Charset_connection::load(Pool& pool, time_t new_disk_time) {
	// pcre_tables
	// lowcase, flipcase, bits digit+word+whitespace, masks
	memset(fpcre_tables, 0, sizeof(fpcre_tables));
	prepare_case_tables(fpcre_tables);
	cstr2ctypes(fpcre_tables, (const unsigned char *)"*+?{^.$|()[", ctype_meta);

#ifdef XML
	// transcoder
	XMLCh *fromTable=(XMLCh *)calloc(sizeof(XMLCh)*0x100);
	XMLTransService::TransRec *toTable=(XMLTransService::TransRec *)calloc(
			sizeof(XMLTransService::TransRec)*MAX_CHARSET_UNI_CODES);
	unsigned int toTableSz=0;
	// strangly vital
	toTable[toTableSz].intCh=0;
	toTable[toTableSz].extCh=(XMLByte)0;
	toTableSz++;
#endif

	// loading text
	char *data=file_read_text(pool, ffile_spec);

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
			// fpcre_tables
			case 1: element2ctypes(c, to_bool(cell), fpcre_tables, ctype_space, cbit_space); break;
			case 2: element2ctypes(c, to_bool(cell), fpcre_tables, ctype_digit, cbit_digit); break;
			case 3: element2ctypes(c, to_bool(cell), fpcre_tables, ctype_xdigit); break;
			case 4: element2ctypes(c, to_bool(cell), fpcre_tables, ctype_letter); break;
			case 5: element2ctypes(c, to_bool(cell), fpcre_tables, ctype_word, cbit_word); break;
			case 6: element2case(c, to_wchar_code(cell), fpcre_tables); break;
#ifdef XML
			case 7:
			case 8:
				// transcoder
				if(toTableSz>MAX_CHARSET_UNI_CODES)
					PTHROW(0, 0,
						&ffile_spec,
						"charset must contain not more then %d unicode values", MAX_CHARSET_UNI_CODES);

				XMLCh unicode=(XMLCh)to_wchar_code(cell);
				if(!unicode && column==7/*unicode1 column*/)
					unicode=(XMLCh)c;
				if(unicode) {
					if(!fromTable[c])
						fromTable[c]=unicode;
					toTable[toTableSz].intCh=unicode;
					toTable[toTableSz].extCh=(XMLByte)c;
					toTableSz++;
				}
				break;
#endif
			}
		}
	};

#ifdef XML
	// sort by the Unicode code point
	_qsort(toTable, toTableSz, sizeof(*toTable), 
		sort_cmp_Trans_rec_intCh);
	//FILE *f=fopen("c:\\temp\\a", "wb");
	//fwrite(toTable, toTableSz, sizeof(*toTable), f);
	//fclose(f);


	// addEncoding
	XalanDOMString sencoding(fname.cstr());
	const XMLCh* const auto_encoding_cstr=sencoding.c_str();
	int size=sizeof(XMLCh)*(sencoding.size()+1);
	XMLCh* pool_encoding_cstr=(XMLCh*)malloc(size);
	memcpy(pool_encoding_cstr, auto_encoding_cstr, size);
    XMLString::upperCase(pool_encoding_cstr);

    XMLPlatformUtils::fgTransService->addEncoding(
		pool_encoding_cstr, 
		new ENameMapFor2<XML256TableTranscoder2>(
			pool_encoding_cstr
			, fromTable
			, toTable
			, toTableSz
		));
#endif

	prev_disk_time=new_disk_time;
}
