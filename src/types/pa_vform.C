/*
	Parser
	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vform.C,v 1.8 2001/03/19 19:17:47 paf Exp $
*/

/**
	\todo based on cgic by ?
*/

#include <ctype.h>
#include <string.h>

#include "pa_vform.h"
#include "pa_vstring.h"
#include "pa_globals.h"
#include "pa_request.h"

// parse helper funcs

static bool StrEqNc(const char *s1, const char *s2, bool strict) {
	while(true) {
		if(!(*s1)) {
			if(!(*s2))
				return true;
			else
				return !strict;
		} else if(!(*s2))
			return !strict;
		if(isalpha(*s1)) {
			if(tolower(*s1) !=tolower(*s2))
				return false;
		} else if((*s1) !=(*s2))
			return false;
		s1++;
		s2++;
	}
}

static int getHeader(const char *data,int len){
    int i,enter=-1;
    if (data)
	for (i=0;i<len;i++)
	    if (data[i]=='\n'){
		if (enter>=0) enter++;
		if (enter>1) return i;
	    } else if (data[i]!='\r') enter=0;
    return 0;
}

static const char *searchAttribute(const char *data,const char *attr,int len){
    int i;
    if (data)
	for (i=0;i<len;i++)
	    if (tolower(data[i])==*attr){
		int j;
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

char *VForm::strpart(const char *str, int len) {
    char *result=(char *)malloc(len+1);
    if (!result) return NULL;
    memcpy(result,str,len);
    result[len]=0;
    return result;
}

char *VForm::getAttributeValue(const char *data,char *attr,int len) {
    const char *value=searchAttribute(data,attr,len);
    if (value){
	int i;
	if (!(len-=value-data)) return NULL;
	if (*value=='"') {
	    for (i=1;i<len;i++)	if (value[i]=='"') break;
	    return strpart(&value[1],i-1);
	} else {
	    for (i=0;i<len;i++)	if (strchr(" ;\"\n\r",value[i])) break;
	    return strpart(value,i);
	}
    }
    return NULL;
}

void VForm::ParseGetFormInput(const char *query_string) {
	ParseFormInput(query_string, strlen(query_string));
}

void VForm::ParsePostFormInput(const char *content_type, int post_size, 
							   bool mime_mode) {
	char *input;
	if(!post_size) 
		return;

	input=(char *) malloc(post_size);
	int read_size=(*service_funcs.read_post)(input, post_size);
	if(read_size !=post_size)
		THROW(0, 0,
			0,
			"ParsePostFormInput: post_size(%d)!=read_size(%d)", 
				post_size, read_size);

	if(mime_mode)
		ParseMimeInput(content_type, input, post_size);
	else
		ParseFormInput(input, post_size);
}

void VForm::ParseFormInput(const char *data, int length) {
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos=0;
	while(pos !=length) {
		int foundEq=0;
		int foundAmp=0;
		int start=pos;
		int len=0;
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
						   const char *data, int length) {
/* Scan for mime-presented pairs, storing them as they are found. */
	const char 
		*boundary=getAttributeValue(content_type,"boundary=",strlen(content_type)),
	    *lastData=&data[length];
	if(!boundary) 
		THROW(0,0,
			0,
			"VForm::ParseMimeInput no boundary attribute of Content-Type");

	while(true) {
		const char 
			*dataStart=searchAttribute(data,boundary,lastData-data),
			*dataEnd=searchAttribute(dataStart,boundary,lastData-dataStart);
		int headerSize=getHeader(dataStart,lastData-dataStart);

		if(!dataStart|!dataEnd|!headerSize) break;
		if(searchAttribute(dataStart,"content-disposition: form-data",headerSize)){
			int valueSize=(dataEnd-dataStart)-headerSize-5-strlen(boundary);
			char *attr=getAttributeValue(dataStart," name=",headerSize),
			     *fName=getAttributeValue(dataStart," filename=",headerSize);

			if(attr && valueSize){
				/* OK, we have a new pair, add it to the list. */
				AppendFormEntry(attr, &dataStart[headerSize+1], valueSize, fName);
			}
		}
		data=(dataEnd-strlen(boundary));
	}
}

void VForm::AppendFormEntry(const char *aname, 
							const char *value_ptr, int value_size,
							const char *file_name) {
	String& sname=*NEW String(pool(), aname);

	Value *value;
	if(file_name)
		value=0; //TODO NEW VFile(...)
	else {
		String& string=*NEW String(pool());
		string.APPEND_TAINTED(value_ptr, value_size, "form", 0);
		value=NEW VString(string);
	}

	fields.put(sname, value);
}

void VForm::fill_fields(Request& request, int post_max_size) {
	// parsing QS [GET and ?name=value from uri rewrite)]
	if(request.info.query_string)
		ParseGetFormInput(request.info.query_string);
	// parsing POSTed data
	if(request.info.method) {
		if(const char *content_type=request.info.content_type)
			if(StrEqNc(request.info.method, "post",true)) {
				int post_size=max(0, min(request.info.content_length, post_max_size));
				if(StrEqNc(content_type, "application/x-www-form-urlencoded",true)) 
					ParsePostFormInput(content_type, post_size, false);
				else if(StrEqNc(content_type, "multipart/form-data",0))
					ParsePostFormInput(content_type, post_size, true);
			}
	} else
		; // TODO: разобрать пришедшее письмо, если какой ключик выставлен?
}
