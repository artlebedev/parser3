/**	@file
	Parser: @b form class.
	based on The CGI_C library, by Thomas Boutell.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
	
	$Id: pa_vform.C,v 1.53 2002/02/08 08:32:34 paf Exp $

	based on The CGI_C library, by Thomas Boutell.
*/

#include "pa_sapi.h"
#include "pa_vform.h"
#include "pa_vstring.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_common.h"
#include "pa_vtable.h"
#include "pa_charset.h"

// parse helper funcs

static size_t getHeader(const char *data, size_t len){
    size_t i;
    int enter=-1;
    if (data)
	for (i=0;i<len;i++)
	    if (data[i]=='\n'){
		if (enter>=0) enter++;
		if (enter>1) return i;
	    } else if (data[i]!='\r') enter=0;
    return 0;
}

static char *searchAttribute(char *data, const char *attr, size_t len){
    size_t i;
    if (data)
	for (i=0;i<len;i++)
	    if (tolower(data[i])==*attr){
		size_t j;
		for (j=i+1;j<=len;j++)
		    if (!attr[j-i]) return &data[j];
		    else {
			if (j==len) break;
			if (attr[j-i]!=tolower(data[j])) break;
		    }
	    }
    return NULL;
}

// VForm

VForm::VForm(Pool& apool) : VStateless_class(apool, form_base_class),
	fields(apool),
	tables(apool) {
}

char *VForm::strpart(const char *str, size_t len) {
    char *result=(char *)malloc(len+1);
    if (!result) return NULL;
    memcpy(result, str, len);
    result[len]=0;
    return result;
}

char *VForm::getAttributeValue(char *data, char *attr, size_t len) {
    char *value=searchAttribute(data, attr, len);
    if (value){
		size_t i;
		if (!(len-=value-data)) return NULL;
		if (*value=='"') {
			for (i=1;i<len;i++)	if (value[i]=='"') break;
			return strpart(&value[1], i-1);
		} else {
			for (i=0;i<len;i++)	if (strchr(" ;\"\n\r", value[i])) break;
			return strpart(value, i);
		}
    }
    return NULL;
}

void VForm::transcode(
	const void *client_body, size_t client_content_length,
	const void *& source_body, size_t& source_content_length) {
	Charset::transcode(pool(),
		pool().get_client_charset(), client_body, client_content_length,
		pool().get_source_charset(), source_body, source_content_length);
}

void VForm::ParseGetFormInput(char *query_string, size_t length) {
	ParseFormInput(query_string, length);
}

void VForm::ParseFormInput(char *data, size_t length) {
	/* Scan for pairs, unescaping and storing them as they are found. */
	size_t pos=0;
	while(pos !=length) {
		size_t foundEq=0;
		size_t foundAmp=0;
		size_t start=pos;
		size_t len=0;
		while(pos !=length) {
			if(data[pos]=='=') {
				foundEq=1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if(!foundEq)
			break;
		char *attr=unescape_chars(pool(), data+start, len);
		start=pos;
		len=0;
		while(pos !=length) {
			if(data[pos]=='&') {
				foundAmp=1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		/* The last pair probably won't be followed by a &, but
			that's fine, so check for that after accepting it */
		char *value=unescape_chars(pool(), data+start, len);
		/* OK, we have a new pair, add it to the list. */
		size_t value_size=strlen(value);
		AppendFormEntry(attr, value, value_size);

		if(!foundAmp)
			break;
	}
}

void VForm::ParseMimeInput(
						   char *content_type, 
						   char *data, size_t length) {
/* Scan for mime-presented pairs, storing them as they are found. */
	const char 
		*boundary=getAttributeValue(content_type, "boundary=", strlen(content_type)), 
	    *lastData=&data[length];
	if(!boundary) 
		throw Exception(0, 0, 
			0, 
			"VForm::ParseMimeInput no boundary attribute of Content-Type");

	while(true) {
		char 
			*dataStart=searchAttribute(data, boundary, lastData-data), 
			*dataEnd=searchAttribute(dataStart, boundary, lastData-dataStart);
		size_t headerSize=getHeader(dataStart, lastData-dataStart);

		if(!dataStart|!dataEnd|!headerSize) break;
		if(searchAttribute(dataStart, "content-disposition: form-data", headerSize)) {
			size_t valueSize=(dataEnd-dataStart)-headerSize-5-strlen(boundary);
			char *attr=getAttributeValue(dataStart, " name=", headerSize), 
			     *fName=getAttributeValue(dataStart, " filename=", headerSize);

			if(attr && valueSize) {
				/* OK, we have a new pair, add it to the list. */
				AppendFormEntry(attr, &dataStart[headerSize+1], valueSize, fName);
			}
		}
		data=(dataEnd-strlen(boundary));
	}
}

void VForm::AppendFormEntry(
							const char *cname_cstr, 
							char *cvalue_ptr, size_t cvalue_size, 
							const char *file_name) {
	const void *sname_ptr;
	size_t sname_size;
	transcode(
		cname_cstr, strlen(cname_cstr),
		sname_ptr, sname_size);
	String& sname=*NEW String(pool(), (const char *)sname_ptr, sname_size);

	Value *value;
	if(file_name) {
		VFile *vfile=NEW VFile(pool());
		vfile->set(true/*tainted*/, cvalue_ptr, cvalue_size, file_name);
		value=vfile;
	} else {
		fix_line_breaks(cvalue_ptr, cvalue_size);

		const void *svalue_ptr; size_t svalue_size;
		transcode(
			cvalue_ptr, cvalue_size, // client [from]
			svalue_ptr, svalue_size); // source [to]
		
		String& string=*NEW String(pool());
		string.APPEND_TAINTED((const char*)svalue_ptr, svalue_size, "form", 0);

		// tables
		{
			Value *valready=(Value *)tables.get(sname);
			bool existed=valready!=0;
			Table *table;
			if(existed) {
				// second+ appearence
				table=valready->get_table();
			} else {
				// first appearence
				Array& columns=*NEW Array(pool(), 1);
				columns+=NEW String(pool(), "field");
				table=NEW Table(pool(), 0, &columns);
			}
			// this string becomes next row
			Array& row=*NEW Array(pool(), 1);
			row+=&string;
			*table+=&row;
			// not existed before? add it
			if(!existed)
				tables.put(sname, NEW VTable(pool(), table));
		}
		value=NEW VString(string);
	}

	fields.put_dont_replace(sname, value);
}

/// @todo parse input letter if some switch is on
void VForm::fill_fields_and_tables(Request& request) {
	// parsing QS [GET and ?name=value from uri rewrite)]
	if(request.info.query_string) {
		size_t length=strlen(request.info.query_string);
		char *buf=(char *)malloc(length);
		memcpy(buf, request.info.query_string, length);
		ParseGetFormInput(buf, length);
	}
	
	// parsing POSTed data
	if(request.info.method) {
		if(const char *content_type=request.info.content_type)
			if(StrEqNc(request.info.method, "post", true)) {
				if(StrEqNc(content_type, "application/x-www-form-urlencoded", true)) 
					ParseFormInput(request.post_data, request.post_size);
				else if(StrEqNc(content_type, "multipart/form-data", 0)) {
					size_t length=strlen(content_type)+1;
					char *buf=(char *)malloc(length);
					memcpy(buf, content_type, length);
					ParseMimeInput(buf, request.post_data, request.post_size);
				}
			}
	} else
		; // letter?
}

Value *VForm::get_element(const String& aname) {
	// $fields
	if(aname==FORM_FIELDS_ELEMENT_NAME)
		return NEW VHash(pool(), Hash(fields));

	// $tables
	if(aname==FORM_TABLES_ELEMENT_NAME)
		return NEW VHash(pool(), Hash(tables));

	// $CLASS,$method
	if(Value *result=VStateless_class::get_element(aname))
		return result;

	// $element
	return static_cast<Value *>(fields.get(aname));
}

