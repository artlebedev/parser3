/** @file
	Parser: "pack" attribute, use just after structures to be packed.
	@see pa_pragma_pack_begin.h

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pragma_pack_end.h,v 1.1 2001/10/30 07:41:29 paf Exp $
*/

#include "pa_config_includes.h"

#if defined(HAVE_PRAGMA_PACK)
#pragma pack()
#endif
