/** @file
	Parser: Transcoder decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_transcoder.h,v 1.2 2001/12/14 15:25:50 paf Exp $
*/

#ifndef PA_TRANSCODER_H
#define PA_TRANSCODER_H

namespace PCRE {
#include "pcre.h"
#	include "internal.h"
}

#include "pa_config_includes.h"
#include "pa_types.h"

// defines

#ifdef XML
#	include <util/XercesDefs.hpp>
#	include <util/TransService.hpp>
#	define Transcoder_TransRec XMLTransService::TransRec
#else
#	ifndef XMLCh 
#		define XMLCh unsigned int
#	endif
#	ifndef XMLByte
#		define XMLByte unsigned char
#	endif

	struct Transcoder_TransRec {
		XMLCh intCh;
		XMLByte extCh;
	};
#endif


/**	contains byte->unicode & unicode->byte tables
*/
struct Transcoder {
	unsigned char pcre_tables[tables_length];

	XMLCh fromTable[0x100];
	Transcoder_TransRec *toTable;
	uint toTableSize;

	void sort_ToTable();

	XMLByte xlatOneTo(const XMLCh toXlat) const;

	void transcodeToUTF8(Pool& pool,
									 const void *source_body, size_t source_content_length,
									 const void *& dest_body, size_t& dest_content_length) const;
	void transcodeFromUTF8(Pool& pool,
									   const void *source_body, size_t source_content_length,
									   const void *& dest_body, size_t& dest_content_length) const;
	void transcodeToTranscoder(Pool& pool,
										   const Transcoder& dest_transcoder,
										   const void *source_body, size_t source_content_length,
										   const void *& dest_body, size_t& dest_content_length) const;
};

void transcode(Pool& pool,
	const Transcoder *source_transcoder, const void *source_body, size_t source_content_length,
	const Transcoder *dest_transcoder, const void *& dest_body, size_t& dest_content_length
);

#endif
