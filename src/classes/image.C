/** @file
	Parser: @b image parser class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_IMAGE_C="$Id: image.C,v 1.73 2002/08/01 11:26:43 paf Exp $";

/*
	jpegsize: gets the width and height (in pixels) of a jpeg file
	Andrew Tong, werdna@ugcs.caltech.edu           February 14, 1995
	modified slightly by alex@ed.ac.uk
	and further still by rjray@uswest.com
	optimization and general re-write from tmetro@vl.com
	from perl by paf@design.ru
*/

#include "pa_config_includes.h"

#include "gif.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_vimage.h"

// class

class MImage : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VImage(pool); }

public:
	MImage(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }

};

// helpers

#ifndef DOXYGEN
class Measure_reader {
public:
	enum { READ_CHUNK_SIZE=0x400*20 };// 20K
	typedef size_t(*Func)(void *& buf, size_t limit, void *info);

	Measure_reader(Func afunc, void *ainfo) : 
		func(afunc), info(ainfo), 
		chunk(0), offset(0), size(0) {
	}

	size_t read(void *&buf, size_t limit) {
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
#endif

/// PNG file header
struct PNG_Header {
	char dummy[12];
	char signature[4]; //< must be "IHDR"
	unsigned char high_width[2]; //< image width high bytes [we ignore for now]
	unsigned char width[2]; //< image width low bytes
	unsigned char high_height[2]; //< image height high bytes [we ignore for now]
	unsigned char height[4]; //< image height
};

/// GIF file header
struct GIF_Header {
	char       signature[3];         // 'GIF'
	char       version[3];
	unsigned char       width[2];
	unsigned char       height[2];
	char       dif;
	char       fonColor;
	char       nulls;
};

/// JPEG record head
struct JPG_Segment_head {
	unsigned char marker;
	unsigned char code;
	unsigned char length[2];
};
/// JPEG frame header
struct JPG_Size_segment_body {
	char data;                    //< data precision of bits/sample
	unsigned char height[2];               //< image height
	unsigned char width[2];                //< image width
	char numComponents;           //< number of color components
};

//

inline short x_endian_to_int(unsigned char L, unsigned char H) {
	return(short)((H<<8) + L);
}

inline short big_endian_to_int(unsigned char b[2]) {
	return x_endian_to_int(b[1], b[0]);
}

inline short little_endian_to_int(unsigned char b[2]) {
	return x_endian_to_int(b[0], b[1]);
}

void measure_gif(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, int& width, int& height) {

	void *buf;
	const int head_size=sizeof(GIF_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception("image.format", 
			origin_string, 
			"not GIF file - too small");
	GIF_Header *head=(GIF_Header *)buf;

	if(strncmp(head->signature, "GIF", 3)!=0)
		throw Exception("image.format", 
			origin_string, 
			"not GIF file - wrong signature");	

	width=little_endian_to_int(head->width);
	height=little_endian_to_int(head->height);
}

/// @test remove ugly mech in reader - 20K limit
void measure_jpeg(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, int& width, int& height) {
	// JFIF format markers
	const unsigned char MARKER=0xFF;
	const unsigned char CODE_SIZE_FIRST=0xC0;
	const unsigned char CODE_SIZE_LAST=0xC3;

	void *buf;
	const size_t prefix_size=2;
	if(reader.read(buf, prefix_size)<prefix_size)
		throw Exception("image.format", 
			origin_string, 
			"not JPEG file - too small");
	unsigned char *signature=(unsigned char *)buf;
	
	if(!(signature[0]==0xFF && signature[1]==0xD8)) 
		throw Exception("image.format", 
			origin_string, 
			"not JPEG file - wrong signature");

	bool found=false;
	while(true) {
		void *buf;
        // Extract the segment header.
		if(reader.read(buf, sizeof(JPG_Segment_head))<sizeof(JPG_Segment_head))
			break;		
		JPG_Segment_head *head=(JPG_Segment_head *)buf;

        // Verify that it's a valid segment.
		if(head->marker!=MARKER)
			break;

		if(head->code >= CODE_SIZE_FIRST && head->code  <= CODE_SIZE_LAST) {
            // Segments that contain size info
			if(reader.read(buf, sizeof(JPG_Size_segment_body))<sizeof(JPG_Size_segment_body))
				break;
			JPG_Size_segment_body *body=(JPG_Size_segment_body *)buf;
			
			width=big_endian_to_int(body->width);
			height=big_endian_to_int(body->height);
			found=true;
			break;
		} else {
            // Dummy read to skip over data
            size_t limit=big_endian_to_int(head->length) - 2;
			if(reader.read(buf, limit)<limit)
				break;
        }
	}

	if(!found)
		throw Exception("image.format", 
			origin_string, 
			"broken JPEG file - size frame not found");
}

void measure_png(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, int& width, int& height) {

	void *buf;
	const int head_size=sizeof(PNG_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception("image.format", 
			origin_string, 
			"not PNG file - too small");
	PNG_Header *head=(PNG_Header *)buf;

	if(strncmp(head->signature, "IHDR", 4)!=0)
		throw Exception("image.format", 
			origin_string, 
			"not PNG file - wrong signature");	

	width=big_endian_to_int(head->width);
	height=big_endian_to_int(head->height);
}

// measure center

void measure(Pool& pool, const String& file_name, 
			 Measure_reader& reader, int& width, int& height) {
	if(const char *cext=strrchr(file_name.cstr(String::UL_FILE_SPEC), '.')) {
		cext++;
		if(strcasecmp(cext, "GIF")==0)
			measure_gif(pool, &file_name, reader, width, height);
		else if(strcasecmp(cext, "JPG")==0 || strcasecmp(cext, "JPEG")==0) 
			measure_jpeg(pool, &file_name, reader, width, height);
		else if(strcasecmp(cext, "PNG")==0)
			measure_png(pool, &file_name, reader, width, height);
		else
			throw Exception("image.format", 
				&file_name, 
				"unhandled image file name extension '%s'", cext);
	} else
		throw Exception("image.format", 
			&file_name, 
			"can not determine image type - no file name extension");
}

#ifndef DOXYGEN
struct Read_mem_info {
	unsigned char *ptr;
	unsigned char *eof;
};
#endif
static size_t read_mem(void*& buf, size_t limit, void *info) {
	Read_mem_info& rmi=*static_cast<Read_mem_info *>(info);
	buf=rmi.ptr;
	size_t read_size=min(limit, (size_t)(rmi.eof-rmi.ptr));
	rmi.ptr+=read_size;
	return read_size;
}

#ifndef DOXYGEN
struct Read_disk_info {
	const String *file_spec;
	size_t offset;
};
#endif
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

static void _measure(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& data=params->as_no_junction(0, "data must not be code");

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

#ifndef DOXYGEN
struct Attrib_info {
	String *tag; ///< html tag being constructed
	Hash *skip; ///< tag attributes not to append to tag string [to skip]
};
#endif
static void append_attrib_pair(const Hash::Key& key, Hash::Val *val, void *info) {
	Attrib_info& ai=*static_cast<Attrib_info *>(info);

	// skip user-specified and internal(starting with "line-") attributes 
	if(ai.skip && ai.skip->get(key) || key.pos("line-")==0)
		return;

	Value& value=*static_cast<Value *>(val);
	// src="a.gif" width=123 ismap[=-1]
	*ai.tag << " " << key;
	if(value.is_string() || value.as_int()>=0)
		*ai.tag << "=\"" << value.as_string() << "\"";
}
static void _html(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	String tag(pool);
	tag << "<img";

	const Hash& fields=static_cast<VImage *>(r.self)->fields();
	Hash *attribs=0;

	if(params->size()) {
		// for backward compatibility: someday was ^html{}
		Value &vattribs=r.process_to_value(params->get(0),
			/*0/*no name* /,*/
			false/*don't intercept string*/);
		if(vattribs.is_defined()) // allow 'void'
			if(attribs=vattribs.get_hash(&method_name)) {
				Attrib_info attrib_info={&tag, 0};
				attribs->for_each(append_attrib_pair, &attrib_info);
			} else
				throw Exception("parser.runtime", 
					&method_name, 
					"attributes must be hash");
	}

	Attrib_info attrib_info={&tag, attribs};
	fields.for_each(append_attrib_pair, &attrib_info);
	tag << " />";
	r.write_pass_lang(tag);
}

/// @test wrap FILE to auto-object
static gdImage *load(Request& r, const String& method_name, 
					 const String& file_name){
	Pool& pool=r.pool();

	const char *file_name_cstr=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	if(FILE *f=fopen(file_name_cstr, "rb")) {
		gdImage& image=*new(pool) gdImage(pool);
		bool ok=image.CreateFromGif(f);
		fclose(f);
		if(!ok)
			throw Exception("image.format", 
				&file_name,
				"is not in GIF format");
		return &image;
	} else {
		throw Exception("file.missing", 
			&method_name, 
			"can not open '%s'", file_name_cstr);
		return 0;
	}
}


static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& file_name=params->as_string(0, "file name must not be code");

	gdImage& image=*load(r, method_name, file_name);
	int width=image.SX();
	int height=image.SY();
	static_cast<VImage *>(r.self)->set(&file_name, width, height, &image);
}

static void _create(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int width=params->as_int(0, "width must be int", r);
	int height=params->as_int(1, "height must be int", r);
	int bgcolor_value=0xffFFff;
	if(params->size()>2)
		bgcolor_value=params->as_int(2, "color must be int", r);
	gdImage& image=*new(pool) gdImage(pool);
	image.Create(width, height);
	image.FilledRectangle(0, 0, width-1, height-1, image.Color(bgcolor_value));
	static_cast<VImage *>(r.self)->set(0, width, height, &image);
}

static void _gif(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	// could _ but don't thing it's wise to use $image.src for vfile.name

	String out(pool); image->Gif(out);
	
	VFile& vfile=*new(pool) VFile(pool);
	Value *content_type=new(pool) VString(*new(pool) String(pool, "image/gif"));
	vfile.set(false/*not tainted*/, 
		out.cstr(), out.size(), 0, content_type);

	r.write_no_lang(vfile);
}

static void _line(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->Line(
		params->as_int(0, "x0 must be int", r), 
		params->as_int(1, "y0 must be int", r), 
		params->as_int(2, "x1 must be int", r), 
		params->as_int(3, "y1 must be int", r), 
		image->Color(params->as_int(4, "color must be int", r)));
}

static void _fill(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->Fill(
		params->as_int(0, "x must be int", r), 
		params->as_int(1, "y must be int", r), 
		image->Color(params->as_int(2, "color must be int", r)));
}

static void _rectangle(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->Rectangle(
		params->as_int(0, "x0 must be int", r), 
		params->as_int(1, "y0 must be int", r), 
		params->as_int(2, "x1 must be int", r), 
		params->as_int(3, "y1 must be int", r), 
		image->Color(params->as_int(4, "color must be int", r)));
}

static void _bar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->FilledRectangle(
		params->as_int(0, "x0 must be int", r), 
		params->as_int(1, "y0 must be int", r), 
		params->as_int(2, "x1 must be int", r), 
		params->as_int(3, "y1 must be int", r), 
		image->Color(params->as_int(4, "color must be int", r)));
}

#ifndef DOXYGEN
static void add_point(Array::Item *value, void *info) {
	Array& row=*static_cast<Array *>(value);
	gdImage::Point **p=static_cast<gdImage::Point **>(info);
	
	(**p).x=row.get_string(0)->as_int();
	(**p).y=row.get_string(1)->as_int();
	(*p)++;
}
#endif
static void _replace(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	Table *table=params->as_no_junction(2, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			&method_name,
			"coordinates must be table");

	gdImage::Point *all_p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*table->size());
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image->FilledPolygonReplaceColor(all_p, table->size(), 
		image->Color(params->as_int(0, "src color must be int", r)),
		image->Color(params->as_int(1, "dest color must be int", r)));
}

static void _polyline(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	Table *table=params->as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			&method_name,
			"coordinates must be table");

	gdImage::Point *all_p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*table->size());
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image->Polygon(all_p, table->size(), 
		image->Color(params->as_int(0, "color must be int", r)),
		false/*not closed*/);
}

static void _polygon(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	Table *table=params->as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			&method_name,
			"coordinates must be table");

	gdImage::Point *all_p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*table->size());
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image->Polygon(all_p, table->size(), 
		image->Color(params->as_int(0, "color must be int", r)));
}

static void _polybar(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	Table *table=params->as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception("parser.runtime",
			&method_name,
			"coordinates must be table");

	gdImage::Point *all_p=(gdImage::Point *)pool.malloc(sizeof(gdImage::Point)*table->size());
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image->FilledPolygon(all_p, table->size(), 
		image->Color(params->as_int(0, "color must be int", r)));
}

// font

#define Y(y)(y+index*height+1)

/// simple gdImage-based font storage & text output 
class Font : public Pooled {
public:
	
	const static int letter_spacing;
	int height;	    ///< Font heigth
	int monospace;	    ///< Default char width
	int spacebarspace; ///< spacebar width
	gdImage& ifont;
	const String& alphabet;
	
	Font(Pool& pool, 
		const String& aalphabet, 
		gdImage& aifont, int aheight, int amonospace, int aspacebarspace) : Pooled(pool), 
		alphabet(aalphabet), 
		height(aheight), monospace(amonospace),  spacebarspace(aspacebarspace),
		ifont(aifont) {
	}
	
	/* ******************************** char ********************************** */
	
	int index_of(char ch) {
		if(ch==' ') return -1;
		return alphabet.pos(&ch, 1);
	}
	
	int index_width(int index) {
		if(index<0)
			return spacebarspace;
		int tr=ifont.GetTransparent();
		for(int x=ifont.SX()-1; x>=0; x--) {
			for(int y=0; y<height-1; y++)
				if(ifont.GetPixel(x, Y(y))!=tr) 
					return x+1;
		}
		return 0;
	}
	
	void index_display(gdImage& image, int x, int y, int index){
		if(index>=0) 
			ifont.Copy(image, x, y, 0, Y(0), index_width(index), height-1);
	}
	
	/* ******************************** string ********************************** */
	
	int string_width(const String& s){
		const char *cstr=s.cstr();
		int result=0;
		for(; *cstr; cstr++)
			result+=index_width(index_of(*cstr));
		return result;
	}
	
	void string_display(gdImage& image, int x, int y, const String& s){
		const char *cstr=s.cstr();
		if(cstr) for(; *cstr; cstr++) {
			int index=index_of(*cstr);
			index_display(image, x, y, index);
			x+=letter_spacing + (monospace ? monospace : index_width(index));
		}
	}
	
};
const int Font::letter_spacing=1;

static void _font(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& alphabet=params->as_string(0, "alphabet must not be code");
	gdImage& image=*load(r, method_name, params->as_string(1, "file_name must not be code"));
	int spacebar_width=params->as_int(2, "spacebar_width must be int", r);
	int monospace_width;
	if(params->size()>3) {
		monospace_width=params->as_int(3, "monospace_width must be int", r);
		if(!monospace_width)
			monospace_width=image.SX();
	} else
		monospace_width=0;

	if(!alphabet.size())
		throw Exception("parser.runtime",
			&method_name,
			"alphabet must not be empty");
	
	static_cast<VImage *>(r.self)->font=new(pool) Font(pool, 
		alphabet, 
		image, 
		image.SY() / alphabet.size(), monospace_width, spacebar_width);
}

static void _text(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int x=params->as_int(0, "x must be int", r);
	int y=params->as_int(1, "y must be int", r);
	const String& s=params->as_string(2, "text must not be code");

	VImage& vimage=*static_cast<VImage *>(r.self);
	if(vimage.image)
		if(vimage.font)
			vimage.font->string_display(*vimage.image, x, y, s);
		else
			throw Exception("parser.runtime",
				&method_name,
				"set the font first");
	else
		throw Exception(0, 
			&method_name, 
			"does not contain an image");
}

static void _length(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& s=params->as_string(0, "text must not be code");

	VImage& vimage=*static_cast<VImage *>(r.self);
	if(vimage.image)
		if(vimage.font) {
			r.write_no_lang(*new(pool) VInt(pool, vimage.font->string_width(s)));
		} else
			throw Exception("parser.runtime",
				&method_name,
				"set the font first");
	else
		throw Exception(0, 
			&method_name, 
			"does not contain an image");
}

static void _arc(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->Arc(
		params->as_int(0, "center_x must be int", r), 
		params->as_int(1, "center_y must be int", r), 
		params->as_int(2, "width must be int", r), 
		params->as_int(3, "height must be int", r), 
		params->as_int(4, "start degrees must be int", r), 
		params->as_int(5, "end degrees must be int", r), 
		image->Color(params->as_int(6, "cx must be int", r)));
}

static void _sector(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	image->Sector(
		params->as_int(0, "center_x must be int", r), 
		params->as_int(1, "center_y must be int", r), 
		params->as_int(2, "width must be int", r), 
		params->as_int(3, "height must be int", r), 
		params->as_int(4, "start degrees must be int", r), 
		params->as_int(5, "end degrees must be int", r), 
		image->Color(params->as_int(6, "color must be int", r)));
}

static void _circle(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.self)->image;
	if(!image)
		throw Exception(0, 
			&method_name, 
			"does not contain an image");

	int size=params->as_int(2, "radius must be int", r)*2;
	image->Arc(
		params->as_int(0, "center_x must be int", r), 
		params->as_int(1, "center_y must be int", r), 
		size, //w
		size, //h
		0, //s
		360, //e
		image->Color(params->as_int(3, "color must be int", r)));
}

gdImage& as_image(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	Value& value=params->as_no_junction(index, msg);

	if(strcmp(value.type(), VIMAGE_TYPE)!=0)
		throw Exception("parser.runtime", 
			&method_name, 
			msg);

	gdImage *src=static_cast<VImage *>(&value)->image;
	if(!src)
		throw Exception("parser.runtime", 
			&method_name, 
			msg);

	return *src;
}

static void _copy(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *dest=static_cast<VImage *>(r.self)->image;
	if(!dest)
		throw Exception(0, 
			&method_name, 
			"self does not contain an image");

	gdImage& src=as_image(pool, method_name, params, 0, "src must be image");

	int sx=params->as_int(1, "src_x must be int", r);
	int sy=params->as_int(2, "src_y must be int", r);
	int sw=params->as_int(3, "src_w must be int", r);
	int sh=params->as_int(4, "src_h must be int", r);
	int dx=params->as_int(5, "dest_x must be int", r);
	int dy=params->as_int(6, "dest_y must be int", r);
	if(params->size()>1+2+2+2) {
		int dw=params->as_int(1+2+2+2, "dest_w must be int", r);
		int dh=(int)(params->size()>1+2+2+2+1?
			params->as_int(1+2+2+2+1, "dest_h must be int", r):sh*(((double)dw)/((double)sw)));
		int tolerance=params->size()>1+2+2+2+2?
			params->as_int(1+2+2+2+2, "tolerance must be int", r):150;

		src.CopyResampled(*dest, dx, dy, sx, sy, dw, dh, sw, sh, tolerance);
	} else
		src.Copy(*dest, dx, dy, sx, sy, sw, sh);
}


// constructor

MImage::MImage(Pool& apool) : Methoded(apool, "image") {
	// ^image:measure[DATA]
	add_native_method("measure", Method::CT_DYNAMIC, _measure, 1, 1);

	// ^image.html[]
	// ^image.html[hash]
	add_native_method("html", Method::CT_DYNAMIC, _html, 0, 1);

	// ^image.load[background.gif]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^image.create[width;height] bgcolor=white
	// ^image.create[width;height;bgcolor]
	add_native_method("create", Method::CT_DYNAMIC, _create, 2, 3);

	// ^image.gif[]
	add_native_method("gif", Method::CT_DYNAMIC, _gif, 0, 0);

	// ^image.line(x0;y0;x1;y1;color)
	add_native_method("line", Method::CT_DYNAMIC, _line, 5, 5);

	// ^image.fill(x;y;color)
	add_native_method("fill", Method::CT_DYNAMIC, _fill, 3, 3);

	// ^image.rectangle(x0;y0;x1;y1;color)
	add_native_method("rectangle", Method::CT_DYNAMIC, _rectangle, 5, 5);

	// ^image.bar(x0;y0;x1;y1;color)
	add_native_method("bar", Method::CT_DYNAMIC, _bar, 5, 5);

	// ^image.replace(color-source;color-dest)[table x:y]
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 3, 3);

	// ^image.polyline(color)[table x:y]
	add_native_method("polyline", Method::CT_DYNAMIC, _polyline, 2, 2);

	// ^image.polygon(color)[table x:y]
	add_native_method("polygon", Method::CT_DYNAMIC, _polygon, 2, 2);

	// ^image.polybar(color)[table x:y]
	add_native_method("polybar", Method::CT_DYNAMIC, _polybar, 2, 2);

    // ^image.font[alPHAbet;font-file-name.gif](spacebar_width)
    // ^image.font[alPHAbet;font-file-name.gif](spacebar_width;width)
	add_native_method("font", Method::CT_DYNAMIC, _font, 3, 4);

    // ^image.text(x;y)[text]
	add_native_method("text", Method::CT_DYNAMIC, _text, 3, 3);
	
    // ^image.ngth[text]
	add_native_method("length", Method::CT_DYNAMIC, _length, 1, 1);
	
	// ^image.arc(center x;center y;width;height;start in degrees;end in degrees;color)
	add_native_method("arc", Method::CT_DYNAMIC, _arc, 7, 7);

	// ^image.sector(center x;center y;width;height;start in degrees;end in degrees;color)
	add_native_method("sector", Method::CT_DYNAMIC, _sector, 7, 7);

	// ^image.circle(center x;center y;r;color)
	add_native_method("circle", Method::CT_DYNAMIC, _circle, 4, 4);

	// ^image.copy[source](src x;src y;src w;src h;dst x;dst y[;dest w[;dest h[;tolerance]]])
	add_native_method("copy", Method::CT_DYNAMIC, _copy, 1+2+2+2, (1+2+2+2)+2+1);
}

// global variable

Methoded *image_class;

// creator

Methoded *MImage_create(Pool& pool) {
	return image_class=new(pool) MImage(pool);
}
