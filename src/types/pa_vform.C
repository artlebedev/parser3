/**	@file
	Parser: @b form class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	based on The CGI_C library, by Thomas Boutell.
*/

static const char* IDENT_VFORM_C="$Date: 2002/11/19 09:03:20 $";

#include "pa_sapi.h"
#include "pa_vform.h"
#include "pa_vstring.h"
#include "pa_globals.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_common.h"
#include "pa_vtable.h"
#include "pa_charset.h"

// defines

//#define DEBUG_POST

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

static const char *searchAttribute(const char *data, 
							 const char *attr,  //< expected to be lowercased
							 size_t len){
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

extern Methoded *form_base_class;

VForm::VForm(Pool& apool) : VStateless_class(apool, 0, form_base_class),
	fields(apool),
	tables(apool),
	imap(apool),
	filled(false) {
}

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

void VForm::transcode(
	const void *client_body, size_t client_content_length,
	const void *& source_body, size_t& source_content_length) {
	Charset::transcode(pool(),
		pool().get_client_charset(), client_body, client_content_length,
		pool().get_source_charset(), source_body, source_content_length);
}

void VForm::ParseGetFormInput(const char *query_string, size_t length) {
	ParseFormInput(query_string, length);
}


static int atoi(const char *data, size_t alength) {
	char buf[MAX_STRING];
	size_t length=min((int)alength, MAX_STRING-1);
	strncpy(buf, data, length);
	return atoi(buf);
}
void VForm::ParseFormInput(const char *data, size_t length) {
	// cut out ?image_map_tail
	{
		for(size_t pos=0; pos<length; pos++) {
			if(data[pos]=='?') {
				size_t start=pos+1;
				size_t aftercomma=start;
				size_t lookingcomma=start;
				for(; lookingcomma<length; lookingcomma++) {
					if(data[lookingcomma]==',') {
						aftercomma=++lookingcomma;
						break;
					}
				}				
				
				if(aftercomma>start) { // ?x,y
					int x=atoi(data+start, aftercomma-1-start);
					int y=atoi(data+aftercomma, length-aftercomma);
					imap.put(*NEW String(pool(), "x"), NEW VInt(pool(), x));
					imap.put(*NEW String(pool(), "y"), NEW VInt(pool(), y));
				} else { // ?qtail
					AppendFormEntry("qtail", data+start, length-start);
				}
				// cut tail
				length=pos;
				break;
			}
		}
	}
	// Scan for pairs, unescaping and storing them as they are found
	for(size_t pos=0; pos<length; ) {
		size_t start=pos;
		size_t finish=length;
		for(; pos<length; pos++) {
			if(data[pos]=='&') {
				finish=pos++;
				break;
			}
		}

		size_t aftereq=start;
		size_t lookingeq=start;
		for(; lookingeq<finish; lookingeq++) {
			if(data[lookingeq]=='=') {
				aftereq=++lookingeq;
				break;
			}
		}

		const char *attr=aftereq>start?unescape_chars(pool(), data+start, aftereq-1-start):"nameless";
		char *value=unescape_chars(pool(), data+aftereq, finish-aftereq);
		AppendFormEntry(attr, value, strlen(value));
	}
}

static char *pa_tolower(char *s) {
	for(char *p=s; *p; p++)
		*p=tolower(*p);
	return s;
}
void VForm::ParseMimeInput(
						   char *content_type, 
						   const char *data, size_t length) {
/* Scan for mime-presented pairs, storing them as they are found. */
	const char 
		*boundary=pa_tolower(getAttributeValue(content_type, "boundary=", strlen(content_type))), 
	    *lastData=&data[length];
	if(!boundary) 
		throw Exception(0, 
			0, 
			"VForm::ParseMimeInput no boundary attribute of Content-Type");

	while(true) {
		const char 
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
							const char *raw_cvalue_ptr, const size_t raw_cvalue_size, 
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
		// maybe transcode text/* files?
		vfile->set(true/*tainted*/, raw_cvalue_ptr, raw_cvalue_size, file_name);
		value=vfile;
	} else {
		size_t cvalue_size;
		char *cvalue_ptr=
			(char *)copy(raw_cvalue_ptr, cvalue_size=raw_cvalue_size); 
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

void VForm::fill_fields_and_tables(Request& request) {
	// parsing QS [GET and ?name=value from uri rewrite)]
	if(request.info.query_string) {
		size_t length=strlen(request.info.query_string);
		char *buf=(char *)malloc(length);
		memcpy(buf, request.info.query_string, length);
		ParseGetFormInput(buf, length);
	}

#ifdef DEBUG_POST
	request.info.method="POST";
	void *data;
	file_read(pool(), *NEW String(pool(), "opera.stdin"),  //"ie.stdin"), //
			   data, request.post_size, 
			   false/*as_text*/);	
	request.post_data=(char*)data;
	request.info.content_type="multipart/form-data; boundary=----------mcqY2UDNcdEAoN1mLmne2i";
	//request.info.content_type="multipart/form-data; boundary=---------------------------7d23111f44403a4";

#endif

	// parsing POST data
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
	}

	filled=true;
}

Value *VForm::get_element(const String& aname, Value& aself, bool looking_up) {
	if(!filled)
		throw Exception("parser.runtime",
			&aname,
			"not determined yet");

	// $fields
	if(aname==FORM_FIELDS_ELEMENT_NAME)
		return NEW VHash(pool(), Hash(fields));

	// $tables
	if(aname==FORM_TABLES_ELEMENT_NAME)
		return NEW VHash(pool(), Hash(tables));

	if(aname==FORM_IMAP_ELEMENT_NAME)
		return NEW VHash(pool(), Hash(imap));

	// $method
	if(Value *result=VStateless_class::get_element(aname, aself, looking_up))
		return result;

	// $element
	return static_cast<Value *>(fields.get(aname));
}
