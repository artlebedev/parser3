/**	@file
	Parser: @b form class.
	based on The CGI_C library, by Thomas Boutell.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vform.C,v 1.36 2001/09/18 16:05:43 parser Exp $
*/

#include "pa_sapi.h"
#include "pa_vform.h"
#include "pa_vstring.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_common.h"
#include "pa_vtable.h"

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

static const char *searchAttribute(const char *data, const char *attr, size_t len){
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

char *VForm::strpart(const char *str, size_t len) {
    char *result=(char *)malloc(len+1);
    if (!result) return NULL;
    memcpy(result, str, len);
    result[len]=0;
    return result;
}

char *VForm::getAttributeValue(const char *data, char *attr, size_t len) {
    const char *value=searchAttribute(data, attr, len);
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

void VForm::ParseGetFormInput(const char *query_string) {
	ParseFormInput(query_string, strlen(query_string));
}

void VForm::ParseFormInput(const char *data, size_t length) {
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
		const char *attr=unescape_chars(pool(), data+start, len);
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
		const char *value=unescape_chars(pool(), data+start, len);
		/* OK, we have a new pair, add it to the list. */
		AppendFormEntry(attr, value);
		if(!foundAmp)
			break;
	}
}

void VForm::ParseMimeInput(const char *content_type, 
						   const char *data, size_t length) {
/* Scan for mime-presented pairs, storing them as they are found. */
	const char 
		*boundary=getAttributeValue(content_type, "boundary=", strlen(content_type)), 
	    *lastData=&data[length];
	if(!boundary) 
		THROW(0, 0, 
			0, 
			"VForm::ParseMimeInput no boundary attribute of Content-Type");

	while(true) {
		const char 
			*dataStart=searchAttribute(data, boundary, lastData-data), 
			*dataEnd=searchAttribute(dataStart, boundary, lastData-dataStart);
		size_t headerSize=getHeader(dataStart, lastData-dataStart);

		if(!dataStart|!dataEnd|!headerSize) break;
		if(searchAttribute(dataStart, "content-disposition: form-data", headerSize)){
			size_t valueSize=(dataEnd-dataStart)-headerSize-5-strlen(boundary);
			char *attr=getAttributeValue(dataStart, " name=", headerSize), 
			     *fName=getAttributeValue(dataStart, " filename=", headerSize);

			if(attr && valueSize){
				/* OK, we have a new pair, add it to the list. */
				AppendFormEntry(attr, &dataStart[headerSize+1], valueSize, fName);
			}
		}
		data=(dataEnd-strlen(boundary));
	}
}

void VForm::AppendFormEntry(const char *aname, 
							const char *value_ptr, size_t value_size, 
							const char *file_name) {
	String& sname=*NEW String(pool(), aname);

	Value *value;
	if(file_name) {
		VFile *vfile=NEW VFile(pool());
		vfile->set(true/*tainted*/, value_ptr, value_size, file_name);
		value=vfile;
	} else {
		String& string=*NEW String(pool());
		string.APPEND_TAINTED(value_ptr, value_size, "form", 0);

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
	if(request.info.query_string)
		ParseGetFormInput(request.info.query_string);
	// parsing POSTed data
	if(request.info.method) {
		if(const char *content_type=request.info.content_type)
			if(StrEqNc(request.info.method, "post", true)) {
				if(StrEqNc(content_type, "application/x-www-form-urlencoded", true)) 
					ParseFormInput(request.post_data, request.post_size);
				else if(StrEqNc(content_type, "multipart/form-data", 0))
					ParseMimeInput(content_type, request.post_data, request.post_size);
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

