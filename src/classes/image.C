/** @file
	Parser: @b image parser class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: image.C,v 1.22 2001/04/28 08:43:47 paf Exp $
*/

#include "pa_config_includes.h"

#include "gif.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_vimage.h"

// defines

#define IMAGE_CLASS_NAME "image"

// class

class MImage : public Methoded {
public: // VStateless_class
	
	Value *create_new_value(Pool& pool) { return new(pool) VImage(pool); }

public:
	MImage(Pool& pool);
	bool used_directly() { return true; }

};

// helpers

/// simple buffered reader[from memory/file], used in _measure
class Measure_reader {
public:
	enum { READ_CHUNK_SIZE=0x400 };// 1K
	typedef size_t(*Func)(void *& buf, size_t limit, void *info);

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
				return 0;// as if EOF
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

/// GIF file header
struct GIF_Header {
	char       type[3];         // 'GIF'
	char       version[3];
	unsigned char       width[2];
	unsigned char       height[2];
	char       dif;
	char       fonColor;
	char       nulls;
};

/// JPEG file header
struct JFIF_Header {
	char length[2];              // length of JFIF segment marker
	char identifier[5];          // JFIF identifier
	char version[2];             // version
	char units;                  // units X of Y pixel density
	char xdensity[2];            // X pixel density
	char ydensity[2];            // X pixel density
	char xthumbnails;            // width of thumbnails
	char ythumbnails;            // height of thumbnails
	char reserved;               // reserved
};
/// JPEG frame header
struct JPG_Frame {
	char length[2];               // length of image marker
	char data;                    // data precision of bits/sample
	char height[2];               // image height
	char width[2];                // image width
	char numComponents;           // number of color components
};

//

inline short bytes_to_int(unsigned char HI, unsigned char LO) {
	return(short)((HI<<8) + LO);
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
	const size_t prefix_size=2;
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

/// used by image: _measure / read_mem
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

/// used by image: _measure / read_disk
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
static void _measure(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& data=params->get_no_junction(0, "data must not be code");

	void *info;Measure_reader::Func read_func;
	Read_mem_info read_mem_info;
	Read_disk_info read_disk_info;
	const String *file_name;
	if(data.is_string()) {
		file_name=data.get_string();
		read_disk_info.file_spec=&r.absolute(*file_name);
		read_disk_info.offset=0;
		info=&read_disk_info;read_func=read_disk;
	} else {
		const VFile& vfile=*data.as_vfile();
		file_name=&static_cast<Value *>(vfile.fields().get(*name_name))->as_string();
		read_mem_info.ptr=(unsigned char *)vfile.value_ptr();
		read_mem_info.eof=read_mem_info.ptr+vfile.value_size();
		info=&read_mem_info;read_func=read_mem;
	}

	Measure_reader reader(read_func, info);
	int width, height;
	measure(pool, *file_name, reader, width, height);

	static_cast<VImage *>(r.self)->set(file_name, width, height);
}

/// used by image: _html / append_attrib_pair
struct Attrib_info {
	String *tag; ///< html tag being constructed
	Hash *skip; ///< tag attributes not to append to tag string [to skip]
};
static void append_attrib_pair(const Hash::Key& key, Hash::Val *val, void *info) {
	Attrib_info& ai=*static_cast<Attrib_info *>(info);

	if(ai.skip && ai.skip->get(key))
		return;

	Value& value=*static_cast<Value *>(val);
	// src="a.gif" width=123 ismap[=-1]
	*ai.tag << " " << key;
	if(value.is_string() || value.as_double()>=0)
		*ai.tag << "=\"" << value.as_string() << "\"";
}
/**	^image.html[]
	^image.html[hash]
*/
static void _html(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	String tag(pool);
	tag << "<img";

	const Hash& fields=static_cast<VImage *>(r.self)->fields();
	Hash *attribs=0;

	if(params->size())
		if(attribs=params->get(0).get_hash()) {
			Attrib_info attrib_info={&tag, 0};
			attribs->for_each(append_attrib_pair, &attrib_info);
		} else
			PTHROW(0, 0, 
				&method_name, 
				"attributes must be must be hash");

	Attrib_info attrib_info={&tag, attribs};
	fields.for_each(append_attrib_pair, &attrib_info);
	tag << " />";
	r.write_pass_lang(tag);
}

static gdImage *load(Request& r, const String& method_name, 
					 const String& file_name){
	Pool& pool=r.pool();

	const char *file_name_cstr=r.absolute(file_name).cstr(String::UL_FILE_NAME);
	if(FILE *f=fopen(file_name_cstr, "rb")) {
		gdImage& image=*new(pool) gdImage(pool);
		image.CreateFromGif(f);
		fclose(f);
		return &image;
	} else {
		PTHROW(0, 0, 
			&method_name, 
			"can not open '%s'", file_name_cstr);
		return 0;
	}
}


/// ^image.load[background.gif]
static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vfile_name=params->get_no_junction(0, "file name must not be code");
	const String& file_name=vfile_name.as_string();

	gdImage& image=*load(r, method_name, file_name);
	int width=image.SX();
	int height=image.SY();
	static_cast<VImage *>(r.self)->set(&file_name, width, height, &image);
}

/**	^image.create[width;height] bgcolor=white
	^image.create[width;height;bgcolor]
*/
static void _create(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int width=(int)r.process(params->get(0)).as_double();
	int height=(int)r.process(params->get(1)).as_double();
	int bgcolor_value=0xffFFff;
	if(params->size()>2)
		bgcolor_value=(int)r.process(params->get(2)).as_double();
	gdImage& image=*new(pool) gdImage(pool);
	image.Create(width, height);
	image.FilledRectangle(0, 0, width-1, height-1, image.Color(bgcolor_value));
	static_cast<VImage *>(r.self)->set(0, width, height, &image);
}

/// ^image.gif[]
static void _gif(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	// could _ but don't thing it's wise to use $image.src for vfile.name

	String out(pool); image->Gif(out);
	
	VFile& vfile=*new(pool) VFile(pool);
	String& image_gif=*new(pool) String(pool, "image/gif");
	vfile.set(false/*not tainted*/, 
		out.cstr(String::UL_AS_IS), out.size(), 0, &image_gif);

	r.write_no_lang(vfile);
}

/// ^image.line(x0;y0;x1;y1;color)
static void _line(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	image->Line(
		(int)r.process(params->get(0)).as_double(), 
		(int)r.process(params->get(1)).as_double(), 
		(int)r.process(params->get(2)).as_double(), 
		(int)r.process(params->get(3)).as_double(), 
		image->Color((int)r.process(params->get(4)).as_double()));
}

/// ^image.fill(x;y;color)
static void _fill(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	image->Fill(
		(int)r.process(params->get(0)).as_double(), 
		(int)r.process(params->get(1)).as_double(), 
		image->Color((int)r.process(params->get(2)).as_double()));
}

/// ^image.rectangle(x0;y0;x1;y1;color)
static void _rectangle(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	image->Rectangle(
		(int)r.process(params->get(0)).as_double(), 
		(int)r.process(params->get(1)).as_double(), 
		(int)r.process(params->get(2)).as_double(), 
		(int)r.process(params->get(3)).as_double(), 
		image->Color((int)r.process(params->get(4)).as_double()));
}

/// ^image.bar(x0;y0;x1;y1;color)
static void _bar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	image->FilledRectangle(
		(int)r.process(params->get(0)).as_double(), 
		(int)r.process(params->get(1)).as_double(), 
		(int)r.process(params->get(2)).as_double(), 
		(int)r.process(params->get(3)).as_double(), 
		image->Color((int)r.process(params->get(4)).as_double()));
}

/// ^image.replace(color-source;color-dest)(x;y)... point coord pairs
static void _replace(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	if((params->size()-2)%2) // I see your thoughts, but that's more readable
		PTHROW(0, 0, 
			&method_name, 
			"y coordinate missing");

	int n=(params->size()-2)/2;
	
	gdImage::Point *p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*n);
	for(int i=0; i<n; i++) {
		p[i].x=(int)r.process(params->get(2+i*2+0)).as_double();
		p[i].y=(int)r.process(params->get(2+i*2+1)).as_double();
	}
	image->FilledPolygonReplaceColor(p, n, 
		image->Color((int)r.process(params->get(0)).as_double()), // src color
		image->Color((int)r.process(params->get(1)).as_double()));// dest color
}

/// ^image.polygon(color)(x;y)... point coord pairs
static void _polygon(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	if((params->size()-1)%2) // [I see..] see now?
		PTHROW(0, 0, 
			&method_name, 
			"y coordinate missing");

	int n=(params->size()-1)/2;
	
	gdImage::Point *p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*n);
	for(int i=0; i<n; i++) {
		p[i].x=(int)r.process(params->get(1+i*2+0)).as_double();
		p[i].y=(int)r.process(params->get(1+i*2+1)).as_double();
	}
	image->Polygon(p, n, 
		image->Color((int)r.process(params->get(0)).as_double()));
}

/// ^image.polybar(color)(x;y)... point coord pairs
static void _polybar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");

	if((params->size()-1)%2) // [I see..] see now?
		PTHROW(0, 0, 
			&method_name, 
			"y coordinate missing");

	int n=(params->size()-1)/2;
	
	gdImage::Point *p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*n);
	for(int i=0; i<n; i++) {
		p[i].x=(int)r.process(params->get(1+i*2+0)).as_double();
		p[i].y=(int)r.process(params->get(1+i*2+1)).as_double();
	}
	image->FilledPolygon(p, n, 
		image->Color((int)r.process(params->get(0)).as_double()));
}

// font

#define Y(y)(y+index*height+1)

/// simple gdImage-based font storage & text output 
class Font : public Pooled {
public:
	
	int height;	    /* Font heigth */
	int space;	    /* Default char width */
	gdImage& ifont;
	const String& alphabet;
	
	Font(Pool& pool, 
		const String& aalphabet, 
		gdImage& aifont, int aheight, int aspace) : Pooled(pool), 
		alphabet(aalphabet), 
		height(aheight), space(aspace), 
		ifont(aifont) {
	}
	
	/* ******************************** char ********************************** */
	
	int index_of(char ch) {
		if(ch==' ') return -1;
		return alphabet.pos(&ch, 1);
	}
	
	int index_width(int index) {
		if(index<0)
			index=index_of('.');
		if(index<0) 
			return 0;
		int tr=ifont.GetTransparent();
		for(int x=ifont.SX()-1; x>0; x--) {
			for(int y=0; y<height-1; y++)
				if(ifont.GetPixel(x, Y(y))!=tr) 
					return x+2;
		}
		return 0;
	}
	
	void index_display(gdImage& image, int x, int y, int index){
		if(index>=0) 
			ifont.Copy(image, x, y, 0, Y(0), index_width(index), height-1);
	}
	
	/* ******************************** string ********************************** */
	/*
	int string_width(const char *cstr){
		int result=0;
		for(; *cstr; cstr++)
			result+=index_width(index_of(*cstr));
		return result;
	}
	*/
	
	void string_display(gdImage& image, int x, int y, const String& s){
		const char *cstr=s.cstr(String::UL_AS_IS);
		if(cstr) for(; *cstr; cstr++) {
			int index=index_of(*cstr);
			index_display(image, x, y, index);
			x+=space ? space : index_width(index);
		}
	}
	
};
/**
	^image.font[alPHAbet;font-file-name.gif](height)
	^image.font[alPHAbet;font-file-name.gif](height;width)
*/
static void _font(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& valphabet=params->get_no_junction(0, "alphabet must not be code");
	Value& file_name=params->get_no_junction(1, "file_name must not be code");
	int height=(int)r.process(params->get(2)).as_double();

	int width=params->size()>3?(int)r.process(params->get(3)).as_double():0;

	static_cast<VImage *>(r.self)->font=new(pool) Font(pool, 
		valphabet.as_string(), 
		*load(r, method_name, file_name.as_string()), 
		height, width);
}

/// ^image.text(x;y)[text]
static void _text(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int x=(int)r.process(params->get(0)).as_double();
	int y=(int)r.process(params->get(1)).as_double();
	const String& s=r.process(params->get(2)).as_string();

	VImage& vimage=*static_cast<VImage *>(r.self);
	if(vimage.image)
		if(vimage.font)
			vimage.font->string_display(*vimage.image, x, y, s);
		else
			PTHROW(0, 0,
				&method_name,
				"set the font first");
	else
		PTHROW(0, 0, 
			&method_name, 
			"does not contain an image");
}

// constructor

MImage::MImage(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), IMAGE_CLASS_NAME));


	// ^image:measure[DATA]
	add_native_method("measure", Method::CT_DYNAMIC, _measure, 1, 1);

	/// ^image.html[]
	/// ^image.html[hash]
	add_native_method("html", Method::CT_DYNAMIC, _html, 0, 1);

	/// ^image.load[background.gif]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	/// ^image.create[width;height] bgcolor=white
	/// ^image.create[width;height;bgcolor]
	add_native_method("create", Method::CT_DYNAMIC, _create, 2, 3);

	/// ^image.gif[]
	add_native_method("gif", Method::CT_DYNAMIC, _gif, 0, 0);

	/// ^image.line(x0;y0;x1;y1;color)
	add_native_method("line", Method::CT_DYNAMIC, _line, 5, 5);

	/// ^image.fill(x;y;color)
	add_native_method("fill", Method::CT_DYNAMIC, _fill, 3, 3);

	/// ^image.rectangle(x0;y0;x1;y1;color)
	add_native_method("rectangle", Method::CT_DYNAMIC, _rectangle, 5, 5);

	/// ^image.bar(x0;y0;x1;y1;color)
	add_native_method("bar", Method::CT_DYNAMIC, _bar, 5, 5);

	/// ^image.replace(color-source;color-dest)(x;y)... point coord pairs
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 2+3*2, 2+100*2);

	/// ^image.polygon(color)(x;y)... point coord pairs
	add_native_method("polygon", Method::CT_DYNAMIC, _polygon, 1+3*2, 1+100*2);

	/// ^image.polybar(color)(x;y)... point coord pairs
	add_native_method("polybar", Method::CT_DYNAMIC, _polybar, 1+3*2, 1+100*2);

    /// ^image.font[alPHAbet;font-file-name.gif](height)
    /// ^image.font[alPHAbet;font-file-name.gif](height;width)
	add_native_method("font", Method::CT_DYNAMIC, _font, 3, 4);

    /// ^image.text(x;y)[text]
	add_native_method("text", Method::CT_DYNAMIC, _text, 3, 3);
	
}

// global variable

Methoded *image_class;

// creator

Methoded *MImage_create(Pool& pool) {
	return image_class=new(pool) MImage(pool);
}
