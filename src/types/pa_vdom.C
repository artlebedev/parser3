/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vdom.C,v 1.3 2001/09/26 10:32:26 parser Exp $
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vdom.C,v 1.3 2001/09/26 10:32:26 parser Exp $"; 

#include "pa_vdom.h"

void VDom_cleanup(void *vdom) {
	//_asm int 3;
	static_cast<VDom *>(vdom)->cleanup();
}
#endif
