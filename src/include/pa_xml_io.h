/** @file
	Parser: plugins to xml library, controlling i/o.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML
#ifndef PA_XML_IO_H
#define PA_XML_IO_H

#define IDENT_PA_XML_IO_H "$Id: pa_xml_io.h,v 1.10 2020/12/15 17:10:33 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"

void pa_xml_io_init();

typedef HashString<bool> HashStringBool;

HashStringBool* pa_xmlStartMonitoringDependencies();
HashStringBool* pa_xmlGetDependencies();

#endif
#endif
