/** @file
	Parser: "pack" attribute, use just before structures to be packed.
	@see pa_pragma_pack_end.h

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#if _MSC_VER
// used #pragma pack to change alignment
#pragma warning(disable:4103)
#endif

#if defined(HAVE_PRAGMA_PACK)
#pragma pack(1)
#endif
