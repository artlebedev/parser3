/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vxdoc.C,v 1.1 2001/09/26 11:24:07 parser Exp $
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vxdoc.C,v 1.1 2001/09/26 11:24:07 parser Exp $"; 

#include "pa_vxdoc.h"

void VXdoc_cleanup(void *vxdoc) {
	//_asm int 3;
	static_cast<VXdoc *>(vxdoc)->cleanup();
}
#endif
