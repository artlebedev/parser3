/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vdom.C,v 1.2 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_vdom.h"

void VDom_cleanup(void *vdom) {
	//_asm int 3;
	static_cast<VDom *>(vdom)->cleanup();
}
#endif
