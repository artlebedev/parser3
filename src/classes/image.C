/** @file
	Parser: @b image parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: image.C,v 1.4 2001/04/10 14:18:28 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef WIN32
#	include "smtp/smtp.h"
#endif

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_vimage.h"

// global var

VStateless_class *image_class;

// helpers

class Measure_reader {
public:
	enum { READ_CHUNK_SIZE=0x400 }; // 1K
	typedef size_t (*Func)(void *& buf, size_t limit, void *info);

	Measure_reader(Func afunc, void *ainfo) : 
		func(afunc), info(ainfo), 
		chunk(0), offset(0), size(0) {
	}

	size_t read(unsigned char *& buf, size_t limit) {
		if(offset+limit>size) // nothing left
			if(offset==0 || limit==1) { // only one-byte continuations allowed
				size=(*func)(chunk, READ_CHUNK_SIZE, info);
				offset=0;
			} else
				return 0; // as if EOF
		if(!size) // EOF
			return 0;
			
		// something left
		size_t read_size=min(offset+limit, size)-offset;
		buf=((unsigned char *)chunk)+offset;
		offset+=read_size;
		return read_size;
	}

private:
	Func func;
	void *info;

	void *chunk;
	size_t offset;
	size_t size;
};

// GIF
struct GIF_Header {
	char       type[3];          // 'GIF'
	char       version[3];
	unsigned char       width[2];
	unsigned char       height[2];
	char       dif;
	char       fonColor;
	char       nulls;
};

// JPEG
struct JFIF_Header {
	char length[2];               // length of JFIF segment marker
	char identifier[5];           // JFIF identifier
	char version[2];              // version
	char units;                   // units X of Y pixel density
	char xdensity[2];             // X pixel density
	char ydensity[2];             // X pixel density
	char xthumbnails;             // width of thumbnails
	char ythumbnails;             // height of thumbnails
	char reserved;                // reserved
};
struct JPG_Frame {
	char length[2];                // length of image marker
	char data;                     // data precision of bits/sample
	char height[2];                // image height
	char width[2];                 // image width
	char numComponents;            // number of color components
};

//

inline short bytes_to_int(unsigned char HI, unsigned char LO) {
	return (short)((HI<<8) + LO);
}

void measure_gif(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, int& width, int& height) {

	unsigned char *buf;
	const int head_size=sizeof(GIF_Header);
	if(reader.read(buf, head_size)<head_size)
		PTHROW(0, 0, 
			origin_string, 
			"broken GIF header - file size is less then %d bytes", head_size);

	GIF_Header& screenD=*reinterpret_cast<GIF_Header *>(buf);
	if(strncmp(screenD.type, "GIF", 3)!=0)
		PTHROW(0, 0, 
			origin_string, 
			"bad image file - GIF signature not found");	

	width=bytes_to_int(screenD.width[1], screenD.width[0]);
	height=bytes_to_int(screenD.height[1], screenD.height[0]);
}

void measure_jpeg(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, int& width, int& height) {
	// JFIF format markers
	const unsigned char SOI=0xD8;
	const unsigned char EOI=0xD9;
	const unsigned char APP0=0xE0;
	const unsigned char SOF0=0xC0;
	const unsigned char SOF2=0xC2;
	const unsigned char COM=0xFE;

	unsigned char *screenD_buf;
	unsigned char *h_buf=0;
	
	bool flag=false;
	
	unsigned char *prefix;
	const prefix_size=2;
	if(reader.read(prefix, prefix_size)<prefix_size)
		PTHROW(0, 0, 
			origin_string, 
			"broken JPEG file - size is less then %d bytes", prefix_size);
	
	if(((unsigned char *)prefix)[1]!=SOI) 
		PTHROW(0, 0, 
			origin_string, 
			"broken JPEG file - second byte of header is not 0x%02X", SOI);
		
	unsigned char zero=0;
	unsigned char *marker=&zero;

	do {
		while((*marker)!=0xFF)
			if(reader.read(marker, sizeof(char))<=0) break;
		if(reader.read(marker, sizeof(char))<=0) break;
		switch(*marker) {
		case EOI:
			marker=&zero;
			break;
		case APP0:
			if(!flag) {
				flag=true;
				if(reader.read(screenD_buf, sizeof(JFIF_Header)) < sizeof(JFIF_Header))
					break;
				JFIF_Header& screenD=*reinterpret_cast<JFIF_Header *>(screenD_buf);
				if((bytes_to_int(screenD.length[0], screenD.length[1]) < 16) ||
					strcasecmp(screenD.identifier, "JFIF")) flag=false;
			}
			break;
		case SOF0:
		case SOF2:
			if(reader.read(h_buf, sizeof(JPG_Frame))<sizeof(JPG_Frame))
				flag=false;
			break;
		default: break;
		}
	} while(*marker!=EOI);
	
	if(flag && h_buf) {
		JPG_Frame& h=*reinterpret_cast<JPG_Frame *>(h_buf);
		width=bytes_to_int(h.width[0], h.width[1]);
		height=bytes_to_int(h.height[0], h.height[1]);
	} else
		PTHROW(0, 0,
			origin_string,
			"broken JPEG file - APP0 frame not found");			
}

// measure center

void measure(Pool& pool, const String& file_name, 
			 Measure_reader& reader, int& width, int& height) {
	if(const char *cext=strrchr(file_name.cstr(), '.')) {
		cext++;
		if(strcasecmp(cext, "GIF")==0)
			measure_gif(pool, &file_name, reader, width, height);
		else if(strcasecmp(cext, "JPG")==0 || strcasecmp(cext, "JPEG")==0) 
			measure_jpeg(pool, &file_name, reader, width, height);
		else
			PTHROW(0, 0, 
				&file_name, 
				"unhandled image file name extension '%s'", cext);
	} else
		PTHROW(0, 0, 
			&file_name, 
			"can not determine image type - no file name extension");
}

// read from somewhere

struct Read_mem_info {
	unsigned char *ptr;
	unsigned char *eof;
};
static size_t read_mem(void*& buf, size_t limit, void *info) {
	Read_mem_info& rmi=*static_cast<Read_mem_info *>(info);
	buf=rmi.ptr;
	size_t read_size=min(limit, (size_t)(rmi.eof-rmi.ptr));
	rmi.ptr+=read_size;
	return read_size;
}

struct Read_disk_info {
	const String *file_spec;
	size_t offset;
};
static size_t read_disk(void*& buf, size_t limit, void *info) {
	Read_disk_info& rdi=*static_cast<Read_disk_info *>(info);
	Pool& pool=rdi.file_spec->pool();

	size_t read_size;
	file_read(pool, *rdi.file_spec, 
			   buf, read_size, 
			   false/*as_text*/, 
			   true/*fail_on_read_problem*/, 
			   rdi.offset, limit);

	rdi.offset+=read_size;
	return read_size;
}

// methods

/// ^image:measure[DATA]
static void _measure(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	Value& data=*static_cast<Value *>(params->get(0));
	// forcing [this body type]
	r.fail_if_junction_(true, data, method_name, "data must not be code");

	void *info; Measure_reader::Func read_func; 
	Read_mem_info read_mem_info;
	Read_disk_info read_disk_info;
	const String *file_name;
	if(data.is_string()) {
		file_name=data.get_string();
		read_disk_info.file_spec=&r.absolute(*file_name);
		read_disk_info.offset=0;
		info=&read_disk_info; read_func=read_disk;
	} else {
		const VFile& vfile=*data.as_vfile();
		file_name=&static_cast<Value *>(vfile.fields().get(*name_name))->as_string();
		read_mem_info.ptr=(unsigned char *)vfile.value_ptr();
		read_mem_info.eof=read_mem_info.ptr+vfile.value_size();
		info=&read_mem_info; read_func=read_mem;
	}

	Measure_reader reader(read_func, info);
	int width, height;
	measure(pool, *file_name, reader, width, height);

	static_cast<VImage *>(r.self)->set(*file_name, width, height);
}

struct Attrib_info {
	String *tag;
	Hash *skip;
};
static void append_attrib_pair(const Hash::Key& key, Hash::Val *val, void *info) {
	Attrib_info& ai=*static_cast<Attrib_info *>(info);

	if(ai.skip && ai.skip->get(key))
		return;

	Value& value=*static_cast<Value *>(val);
	// src="a.gif" width=123 ismap[=-1]
	*ai.tag << " " << key;
	if(value.is_string() || value.as_double()>=0) {
		*ai.tag << "=";
		if(value.is_string())
			*ai.tag << "\"";
		*ai.tag << value.as_string();
		if(value.is_string())
			*ai.tag << "\"";
	}
}
/// ^image.html[]
/// ^image.html[hash]
static void _html(Request& r, const String& method_name, Array *params) {
	Pool& pool=r.pool();

	String tag(pool);
	tag << "<img";

	Hash& fields=static_cast<VImage *>(r.self)->fields();
	Hash *temporary=0;

	if(params->size()) {
		if(temporary=static_cast<Value *>(params->get(0))->get_hash()) {
			Attrib_info attrib_info={&tag, 0};
			temporary->for_each(append_attrib_pair, &attrib_info);
		} else
			PTHROW(0, 0,
				&method_name,
				"parameter must be hash");
	}

	Attrib_info attrib_info={&tag, temporary};
	fields.for_each(append_attrib_pair, &attrib_info);
	tag << ">";
	r.write_pass_lang(tag);
}

// initialize
void initialize_image_class(Pool& pool, VStateless_class& vclass) {
	// ^image:measure[DATA]
	vclass.add_native_method("measure", Method::CT_DYNAMIC, _measure, 1, 1);

	/// ^image.html[]
	/// ^image.html[hash]
	vclass.add_native_method("html", Method::CT_DYNAMIC, _html, 0, 1);
}
