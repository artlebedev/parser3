/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vdom.C,v 1.1 2001/09/15 11:48:41 parser Exp $
*/

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "pa_vdom.h"

void VDom_cleanup(void *vdom) {
	//_asm int 3;
	static_cast<VDom *>(vdom)->cleanup();
}