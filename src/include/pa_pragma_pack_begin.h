/** @file
	Parser: "pack" attribute, use just before structures to be packed.
	@see pa_pragma_pack_end.h

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pragma_pack_begin.h,v 1.1 2001/10/30 07:41:29 paf Exp $
*/

#include "pa_config_includes.h"

#if _MSC_VER
// used #pragma pack to change alignment
#pragma warning(disable:4103)
#endif

#if defined(HAVE_PRAGMA_PACK)
#pragma pack(1)
#endif
