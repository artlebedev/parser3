/**	@file
	Parser: @b form class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

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

volatile const char * IDENT_PA_VFORM_C="$Id: pa_vform.C,v 1.122 2020/12/15 17:10:40 moko Exp $" IDENT_PA_VFORM_H;

// defines

//#define DEBUG_POST

// parse helper funcs

static size_t getHeader(const char* data, size_t len){
	size_t i;
	int enter=-1;
	if (data) {
		for (i=0;i<len;i++)
			if (data[i]=='\n'){
				if (enter>=0) enter++;
				if (enter>1) return i;
			} else {
				if (data[i]!='\r') enter=0;
			}
	}
	return 0;
}

static const char* searchAttribute(const char* data, 
							 const char* attr,  //< expected to be lowercased
							 size_t len){
	size_t i;
	if (data)
		for (i=0;i<len;i++)
			if (tolower((unsigned char)data[i])==*attr){
				size_t j;
				for (j=i+1;j<=len;j++)
					if (!attr[j-i]) return &data[j];
					else {
						if (j==len) break;
						if (attr[j-i]!=tolower((unsigned char)data[j])) break;
					}
			}

	return NULL;
}

// VForm

extern Methoded* form_class;

VForm::VForm(Request_charsets& acharsets, Request_info& arequest_info): VStateless_class(form_class),
	fcharsets(acharsets),
	frequest_info(arequest_info),

	can_have_body(arequest_info.can_have_body()),
	charset_detected(false),
	post_content_type(UNKNOWN),

	filled_source(0),
	filled_client(0),
	fpost_charset(0)
{
	if(can_have_body && arequest_info.content_type) {
		if(pa_strncasecmp(arequest_info.content_type, HTTP_CONTENT_TYPE_FORM_URLENCODED)==0) {
			post_content_type=FORM_URLENCODED;
		} else if(pa_strncasecmp(arequest_info.content_type, HTTP_CONTENT_TYPE_MULTIPART_FORMDATA)==0) {
			post_content_type=MULTIPART_FORMDATA;
		}
	}
}

char *VForm::strpart(const char* str, size_t len) {
	char *result=new(PointerFreeGC) char[len+1];
	memcpy(result, str, len);
	result[len]=0;
	return result;
}

char *VForm::getAttributeValue(const char* data, const char *attr, size_t len) {
	const char* value=searchAttribute(data, attr, len);
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

String::C VForm::transcode(const char* client, size_t client_size, Charset* client_charset) {
	return Charset::transcode(
				String::C(pa_strdup(client, client_size), client_size),
				client_charset?*client_charset:fcharsets.client(),
				fcharsets.source());
}

void VForm::ParseGetFormInput(const char* query_string, size_t length) {
	ParseFormInput(query_string, length);
}


static int atoi(const char* data, size_t alength) {
	char buf[MAX_STRING];
	size_t length=min(alength, sizeof(buf)-1);
	memcpy(buf, data, length); buf[length]=0;
	return atoi(buf);
}

void VForm::ParseFormInput(const char* data, size_t length, Charset* client_charset) {
	// cut out ?image_map_tail
	{
		for(size_t pos=0; pos<length; pos++) {
			if(data[pos]=='?') {
				size_t start=pos+1;
				size_t aftercomma=start;
				size_t lookingcomma=start;
				for(; lookingcomma<length; lookingcomma++)
					if(data[lookingcomma]==',') {
						aftercomma=++lookingcomma;
						break;
					}
				
				if(aftercomma>start) { // ?x,y
					int x=atoi(data+start, aftercomma-1-start);
					int y=atoi(data+aftercomma, length-aftercomma);
					imap.put(String("x"), new VInt(x));
					imap.put(String("y"), new VInt(y));
				} else { // ?qtail
					AppendFormEntry("qtail", data+start, length-start, client_charset);
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
		for(; pos<length; pos++)
			if(data[pos]=='&') {
				finish=pos++;
				break;
			}

		size_t aftereq=start;
		size_t lookingeq=start;
		for(; lookingeq<finish; lookingeq++)
			if(data[lookingeq]=='=') {
				aftereq=++lookingeq;
				break;
			}

		const char* attr=(aftereq>start)?unescape_chars(data+start, aftereq-1-start, &fcharsets.client()):"nameless";
		char *value=unescape_chars(data+aftereq, finish-aftereq, &fcharsets.client());
		AppendFormEntry(attr, value, strlen(value), client_charset);
	}
}

static char* pa_tolower(char *str){
	if(!str)
		return 0;
	for(char *p=str; *p; p++)
		*p=(char)tolower((unsigned char)*p);
	return str;
}

void VForm::ParseMimeInput(
						char *content_type, 
						const char* data, size_t length, Charset* client_charset) {
/* Scan for mime-presented pairs, storing them as they are found. */
	const char* boundary=pa_tolower(getAttributeValue(content_type, "boundary=", strlen(content_type)));
	if(!boundary)
		throw Exception(0, 
			0, 
			"VForm::ParseMimeInput no boundary attribute of Content-Type");

	const char* lastData=&data[length];

	while(true) {
		const char 
			*dataStart=searchAttribute(data, boundary, lastData-data), 
			*dataEnd=searchAttribute(dataStart, boundary, lastData-dataStart);

		size_t headerSize=getHeader(dataStart, lastData-dataStart);

		if(!dataStart || !dataEnd || !headerSize) break;
		if(searchAttribute(dataStart, "content-disposition: form-data", headerSize)) {
			size_t valueSize=(dataEnd-dataStart)-headerSize-5-strlen(boundary);
			char *attr=getAttributeValue(dataStart, " name=", headerSize), 
			     *fName=getAttributeValue(dataStart, " filename=", headerSize);

			if(attr) {
				/* OK, we have a new pair, add it to the list. */
				// fName checks are because MSIE passes unassigned <input type=file> as filename="" and empty body 
				if( fName && (strlen(fName) || valueSize) ){
					AppendFormFileEntry(attr,
						valueSize? &dataStart[headerSize+1]: "",
						valueSize,
						fName,
						client_charset);
				} else {
					AppendFormEntry(attr,
						valueSize? &dataStart[headerSize+1]: "",
						valueSize,
						client_charset);
				}
			}
		}
		data=(dataEnd-strlen(boundary));
	}
}

void VForm::AppendFormFileEntry(const char* cname_cstr, 
				const char* raw_cvalue_ptr, const size_t raw_cvalue_size, 
				const char* file_name_cstr, Charset* client_charset){

	const char* fname = pa_strdup(file_name_cstr);
	const String* sfile_name=new String(transcode(fname, strlen(fname), client_charset));

	const String& sname=*new String(transcode(cname_cstr, strlen(cname_cstr), client_charset));
	// maybe transcode text/* files?
	// NO!!! some users want to upload file 'as is' or file encoding can be unknown

	VFile* vfile=new VFile;
	vfile->set_binary(true/*tainted*/, raw_cvalue_ptr, raw_cvalue_size, sfile_name);

	fields.put_dont_replace(sname, vfile);

	// files
	Value* vhash=files.get(sname);
	if(!vhash){
		// first appearence
		vhash=new VHash;
		files.put(sname, vhash);
	}
	HashStringValue& hash=*vhash->get_hash();

	hash.put(String::Body::Format(hash.count()), vfile);
}

void VForm::AppendFormEntry(const char* cname_cstr, const char* raw_cvalue_ptr, const size_t raw_cvalue_size, Charset* client_charset) {
	const String& sname=*new String(transcode(cname_cstr, strlen(cname_cstr), client_charset));

	const char* premature_zero_pos=(const char* )memchr(raw_cvalue_ptr, 0, raw_cvalue_size);
	size_t cvalue_size=premature_zero_pos?premature_zero_pos-(const char* )raw_cvalue_ptr
		:raw_cvalue_size;
	char *cvalue_ptr=pa_strdup(raw_cvalue_ptr, cvalue_size); 
	fix_line_breaks(cvalue_ptr, cvalue_size);
	String& string=*new String(transcode(cvalue_ptr, cvalue_size, client_charset), String::L_TAINTED);

	// tables
	{
		Value* vtable=tables.get(sname);
		if(!vtable) {
			// first appearence
			Table::columns_type columns(new ArrayString(1));
			*columns+=new String("field");

			vtable=new VTable(new Table(columns));
			tables.put(sname, vtable);
		}
		Table& table=*vtable->get_table();

		// this string becomes next row
		Table::element_type row(new ArrayString(1));
		*row+=&string;
		table+=row;
	}

	fields.put_dont_replace(sname, new VString(string));
}


void VForm::refill_fields_tables_and_files() {
	fields.clear();
	tables.clear();
	files.clear();
	imap.clear();

	//frequest_info.query_string="a=123";
	// parsing QS [GET and ?name=value from uri rewrite)]
	if(frequest_info.query_string) {
		size_t length=strlen(frequest_info.query_string);
		char *buf=pa_strdup(frequest_info.query_string, length);
		ParseGetFormInput(buf, length);
	}

#ifdef DEBUG_POST
	frequest_info.method="POST";
	File_read_result file=file_read_binary(*new String("test.stdin"), true /*fail on problem*/);
	frequest_info.post_size=file.length;
	frequest_info.post_data=(char*)file.str;
	frequest_info.content_type="multipart/form-data; boundary=----------mcqY2UDNcdEAoN1mLmne2i";
#endif

	// parsing POST data
	switch(post_content_type){
		case FORM_URLENCODED: {
			detect_post_charset();
			ParseFormInput(frequest_info.post_data, frequest_info.post_size, fpost_charset);
			break;
		}
		case MULTIPART_FORMDATA: {
			ParseMimeInput(pa_strdup(frequest_info.content_type), frequest_info.post_data, frequest_info.post_size);
			break;
		}
		case UNKNOWN: break;
	}

	filled_source=&fcharsets.source();
	filled_client=&fcharsets.client();
}

void VForm::detect_post_charset(){
	if(can_have_body && !charset_detected){
		fpost_charset=detect_charset(frequest_info.content_type);
		charset_detected=true;
	}
}

bool VForm::should_refill_fields_tables_and_files() {
	return &fcharsets.source()!=filled_source || &fcharsets.client()!=filled_client;
}

Value* VForm::get_element(const String& aname) {
	if(should_refill_fields_tables_and_files())
		refill_fields_tables_and_files();

	// $fields
	if(aname==FORM_FIELDS_ELEMENT_NAME)
		return new VHash(fields);

	// $tables
	if(aname==FORM_TABLES_ELEMENT_NAME)
		return new VHash(tables);

	// $files
	if(aname==FORM_FILES_ELEMENT_NAME)
		return new VHash(files);

	// $imap
	if(aname==FORM_IMAP_ELEMENT_NAME)
		return new VHash(imap);

	// methods (if any)
	if(Value* result=VStateless_class::get_element(aname))
		return result;

	// $field
	return fields.get(aname);
}

Charset* VForm::get_body_charset(){
	detect_post_charset();
	return fpost_charset;
}

const VJunction* VForm::put_element(const String& aname, Value* avalue) {
	if(should_refill_fields_tables_and_files())
		refill_fields_tables_and_files();

	fields.put(aname, avalue);
	return 0;
}
