/**	@file
	Parser: @b form class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	based on The CGI_C library, by Thomas Boutell.
*/

static const char * const IDENT_VFORM_C="$Date: 2007/04/17 18:13:16 $";

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

static size_t getHeader(const char* data, size_t len){
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

extern Methoded* form_base_class;

VForm::VForm(Request_charsets& acharsets, Request_info& arequest_info): VStateless_class(0, form_base_class),
	fcharsets(acharsets),
	frequest_info(arequest_info),
	filled_source(0),
	filled_client(0) {
}

char *VForm::strpart(const char* str, size_t len) {
    char *result=new(PointerFreeGC) char[len+1];
    memcpy(result, str, len);
    result[len]=0;
    return result;
}

char *VForm::getAttributeValue(const char* data, char *attr, size_t len) {
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

String::C VForm::transcode(const char* client, size_t client_size) {
	return Charset::transcode(
		String::C(strdup(client, client_size), client_size),
		fcharsets.client(),
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
void VForm::ParseFormInput(const char* data, size_t length) {
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
					imap.put(String("x"), new VInt(x));
					imap.put(String("y"), new VInt(y));
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

		const char* attr=aftereq>start?unescape_chars(data+start, aftereq-1-start):"nameless";
		char *value=unescape_chars(data+aftereq, finish-aftereq);
		AppendFormEntry(attr, value, strlen(value));
	}
}

static char *pa_tolower(char *s) {
	for(char *p=s; *p; p++)
		*p=(char)tolower((unsigned char)*p);
	return s;
}
void VForm::ParseMimeInput(
						   char *content_type, 
						   const char* data, size_t length) {
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

			if(attr) {
				/* OK, we have a new pair, add it to the list. */
				// fName checks are because MSIE passes unassigned <input type=file> as filename="" and empty body 
				if( fName && (strlen(fName) || valueSize) ){
					AppendFormFileEntry(attr, 
						valueSize? &dataStart[headerSize+1]: "", 
						valueSize, 
						fName); 
				} else {
					AppendFormEntry(attr, 
						valueSize? &dataStart[headerSize+1]: "", 
						valueSize); 
				}
			}
		}
		data=(dataEnd-strlen(boundary));
	}
}

void VForm::AppendFormFileEntry(const char* cname_cstr, 
			    const char* raw_cvalue_ptr, const size_t raw_cvalue_size, 
			    const char* file_name_cstr){

	const String& sname=*new String(transcode(cname_cstr, strlen(cname_cstr)));
	VFile* vfile=new VFile;
	// maybe transcode text/* files?
	const String& sfile_name=*new String(strdup(file_name_cstr));
	vfile->set(true/*tainted*/, raw_cvalue_ptr, raw_cvalue_size, sfile_name.cstr());

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

void VForm::AppendFormEntry(const char* cname_cstr, const char* raw_cvalue_ptr, const size_t raw_cvalue_size) {
	const String& sname=*new String(transcode(cname_cstr, strlen(cname_cstr)));

	const char* premature_zero_pos=(const char* )memchr(raw_cvalue_ptr, 0, raw_cvalue_size);
	size_t cvalue_size=premature_zero_pos?premature_zero_pos-(const char* )raw_cvalue_ptr
		:raw_cvalue_size;
	char *cvalue_ptr=strdup(raw_cvalue_ptr, cvalue_size); 
	fix_line_breaks(cvalue_ptr, cvalue_size);
	String& string=*new String(transcode(cvalue_ptr, cvalue_size), true);

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
		char *buf=strdup(frequest_info.query_string, length);
		ParseGetFormInput(buf, length);
	}

#ifdef DEBUG_POST
	frequest_info.method="POST";
	void *data;
	file_read(*new String("test.stdin"),
			   data, request.post_size, 
			   false/*as_text*/);	
	request.post_data=(char*)data;
	frequest_info.content_type="multipart/form-data; boundary=----------mcqY2UDNcdEAoN1mLmne2i";

#endif

	// parsing POST data
	if(frequest_info.method) {
		if(const char* content_type=frequest_info.content_type)
			if(StrEqNc(frequest_info.method, "post", true)) {
				if(StrEqNc(content_type, "application/x-www-form-urlencoded", true)) 
					ParseFormInput(frequest_info.post_data, frequest_info.post_size);
				else if(StrEqNc(content_type, "multipart/form-data", 0))
					ParseMimeInput(strdup(content_type), 
						frequest_info.post_data, frequest_info.post_size);
			}
	}

	filled_source=&fcharsets.source();
	filled_client=&fcharsets.client();
}

bool VForm::should_refill_fields_tables_and_files() {
	return &fcharsets.source()!=filled_source
		|| &fcharsets.client()!=filled_client;
}

Value* VForm::get_element(const String& aname, Value& aself, bool looking_up) {
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

	// $method
	if(Value* result=VStateless_class::get_element(aname, aself, looking_up))
		return result;

	// $element
	return fields.get(aname);
}
