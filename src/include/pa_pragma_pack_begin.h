/** @file
	Parser: "pack" attribute, use just before structures to be packed.
	@see pa_pragma_pack_end.h

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pragma_pack_begin.h,v 1.4 2002/02/08 08:30:13 paf Exp $
*/

#include "pa_config_includes.h"

#if _MSC_VER
// used #pragma pack to change alignment
#pragma warning(disable:4103)
#endif

#if defined(HAVE_PRAGMA_PACK)
#pragma pack(1)
#endif
