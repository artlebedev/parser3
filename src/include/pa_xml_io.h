/** @file
	Parser: plugins to xml library, controlling i/o.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML
#ifndef PA_XML_IO_H
#define PA_XML_IO_H

static const char * const IDENT_XML_IO_H="$Date: 2004/02/13 14:01:08 $";

#include "pa_string.h"
#include "pa_hash.h"

void pa_xml_io_init();

typedef Hash<const String::Body, bool> HashStringBool;

void pa_xmlStartMonitoringDependencies();
HashStringBool* pa_xmlGetDependencies();

#endif
#endif
