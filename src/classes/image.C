/** @file
	Parser: @b image parser class.

	Copyright(c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_IMAGE_C="$Date: 2003/01/21 15:51:06 $";

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
#include "pa_vdate.h"

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
	virtual size_t read(const void *&buf, size_t limit)=0;
	virtual void seek(long value, int whence)=0;
	virtual long tell()=0;
};

class Measure_file_reader: public Measure_reader {
public:
	Measure_file_reader(Pool& apool, int af, const String& afile_name, const char *afname): 
		pool(apool), file_name(afile_name), fname(afname), f(af) {
	}

	/*override*/size_t read(const void *&abuf, size_t limit) {
		if(limit==0)
			return 0;

		void *lbuf=pool.malloc(limit);
		size_t read_size=(size_t)::read(f, lbuf, limit);  abuf=lbuf;
		if(ssize_t(read_size)<0 || read_size>limit)
			throw Exception(0,
				&file_name, 
				"measure failed: actually read %lu bytes count not in [0..%lu] valid range", 
			read_size, limit);

		return read_size;
	}

	/*override*/void seek(long value, int whence) {
		if(lseek(f, value, whence)<0)
			throw Exception("image.format",
				&file_name, 
				"seek(value=%ld, whence=%d) failed: %s (%d), actual filename '%s'", 
					value, whence, strerror(errno), errno, fname);
	}

	/*override*/long tell() { return lseek(f, 0, SEEK_CUR); }

private:
	Pool& pool;
	const String& file_name; const char *fname;
	int f;
};

class Measure_buf_reader: public Measure_reader {
public:
	Measure_buf_reader(const void *abuf, size_t asize, const String& afile_name): 
		buf(abuf), size(asize), file_name(afile_name), offset(0) {
	}
	
	/*override*/size_t read(const void *&abuf, size_t limit) {
		size_t to_read=min(limit, size-offset);
		abuf=(const char*)buf+offset;
		offset+=to_read;
		return to_read;
	}

	/*override*/void seek(long value, int whence) {
		size_t new_offset;
		switch(whence) {
		case SEEK_CUR: new_offset=offset+value; break;
		case SEEK_SET: new_offset=(size_t)value; break;
		default: throw Exception(0, 0, "whence #%d not supported", 0, whence); break; // never
		}
		
		if((ssize_t)new_offset<0 || new_offset>size)
			throw Exception("image.format",
				&file_name, 
				"seek(value=%l, whence=%d) failed: out of buffer, new_offset>size (%l>%l) or new_offset<0", 
					value, whence, new_offset, size);
		offset=new_offset;
	}

	/*override*/long tell() { return offset; }

private:

	const void *buf; size_t size;
	const String& file_name; 

	size_t offset;
};

#endif

/// PNG file header
struct PNG_Header {
	char dummy[12];
	char signature[4]; //< must be "IHDR"
	uchar high_width[2]; //< image width high bytes [we ignore for now]
	uchar width[2]; //< image width low bytes
	uchar high_height[2]; //< image height high bytes [we ignore for now]
	uchar height[4]; //< image height
};

/// GIF file header
struct GIF_Header {
	char       signature[3];         // 'GIF'
	char       version[3];
	uchar       width[2];
	uchar       height[2];
	char       dif;
	char       fonColor;
	char       nulls;
};

/// JPEG record head
struct JPG_Segment_head {
	uchar marker;
	uchar code;
	uchar length[2];
};
/// JPEG frame header
struct JPG_Size_segment_body {
	char data;                    //< data precision of bits/sample
	uchar height[2];               //< image height
	uchar width[2];                //< image width
	char numComponents;           //< number of color components
};

/// JPEG frame header
struct JPG_Exif_segment_start {
	char signature[6]; // Exif\0\0
};

/// JPEG Exif TIFF Header
struct JPG_Exif_TIFF_header {
	uchar byte_align_identifier[2];
	char dummy[2]; // always 000A [or 0A00]
	uchar first_IFD_offset[4]; // Usually the first IFD starts immediately next to TIFF header, so this offset has value '0x00000008'.
};

// JPEG Exif IFD start
struct JPG_Exif_IFD_start {
	uchar directory_entry_count[2]; // the number of directory entry contains in this IFD
};

// TTTT ffff NNNNNNNN DDDDDDDD 
struct JPG_Exif_IFD_entry {
	uchar tag[2]; // Tag number, this shows a kind of data
	uchar format[2]; // data format
	uchar components_count[4]; // number of components
	uchar value_or_offset_to_it[4]; // data value or offset to data value
};

#define JPG_IFD_TAG_EXIF_OFFSET 0x8769

#define JPEG_EXIF_DATE_CHARS 20

//

inline ushort x_endian_to_ushort(uchar b0, uchar b1) {
	return (ushort)((b1<<8) + b0);
}

inline uint x_endian_to_uint(uchar b0, uchar b1, uchar b2, uchar b3) {
	return (uint)(((((b3<<8) + b2)<<8)+b1)<<8)+b0;
}

inline ushort endian_to_ushort(bool is_big, const uchar *b/* [2] */) {
	return is_big?x_endian_to_ushort(b[1], b[0]):
		x_endian_to_ushort(b[0], b[1]);
}

inline uint endian_to_uint(bool is_big, const uchar *b /* [4] */) {
	return is_big?x_endian_to_uint(b[3], b[2], b[1], b[0]):
		x_endian_to_uint(b[0], b[1], b[2], b[3]);
}

static void measure_gif(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height) {

	const void *buf;
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

	width=endian_to_ushort(false, head->width);
	height=endian_to_ushort(false, head->height);
}

static Value *parse_IFD_entry_formatted_one_value(Pool& pool,
												  bool is_big,
												  ushort format, 
												  size_t component_size, 
												  const uchar *value) {
	switch(format) {
	case 1: // unsigned byte
		return new(pool) VInt(pool, (uchar)value[0]);
	case 3: // unsigned short
		return new(pool) VInt(pool, endian_to_ushort(is_big, value));
	case 4: // unsigned long
		 // 'double' because parser's Int is signed
		return new(pool) VDouble(pool, endian_to_uint(is_big, value));
	case 5: // unsigned rational
		{
			uint numerator=endian_to_uint(is_big, value); value+=component_size/2;
			uint denominator=endian_to_uint(is_big, value);
			if(!denominator)
				return 0;
			return new(pool) VDouble(pool, ((double)numerator)/denominator);
		}
	case 6: // signed byte
		return new(pool) VInt(pool, (signed char)value[0]);
	case 8: // signed short
		return new(pool) VInt(pool, (signed short)endian_to_ushort(is_big, value));
	case 9: // signed long
		return new(pool) VInt(pool, (signed int)endian_to_uint(is_big, value));
	case 10: // signed rational
		{
			signed int numerator=(signed int)endian_to_uint(is_big, value); value+=component_size/2;
			uint denominator=endian_to_uint(is_big, value);
			if(!denominator)
				return 0;
			return new(pool) VDouble(pool, numerator/denominator);
		}
		/*
	case 11: // single float
		todo
	case 12: // double float
		todo
		*/
	};	
	
	return 0;
}

// date.C
time_t cstr_to_time_t(char *cstr, const String *report_error_origin);

static Value *parse_IFD_entry_formatted_value(Pool& pool,
											  bool is_big, ushort format, 
											  size_t component_size, uint components_count, 
											  const uchar *value) {
	if(format==2) { // ascii string, exception: the only type with varying size
		const char *cstr=(const char *)value;
		size_t size=components_count;
		// Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes
		if(size==JPEG_EXIF_DATE_CHARS 
			&& isdigit(cstr[0])
			&& cstr[JPEG_EXIF_DATE_CHARS-1]==0) {
			char cstr_writable[JPEG_EXIF_DATE_CHARS]; 
			strcpy(cstr_writable, cstr);

			time_t t=cstr_to_time_t(cstr_writable, 0/* do not throw exception, just return bad result */);
			if(t>=0)
				return new(pool) VDate(pool, t);
		}

		if(const char *premature_zero_pos=(const char *)memchr(cstr, 0, size))
			size=premature_zero_pos-cstr;
		return new(pool) VString(*new(pool) String(pool, cstr, size, true/*tainted*/));
	}

	if(components_count==1)
		return parse_IFD_entry_formatted_one_value(pool, is_big, format, component_size, value);

	VHash& result=*new(pool) VHash(pool);
	Hash& hash=result.hash(0);
	for(uint i=0; i<components_count; i++, value+=component_size) {
		String& skey=*new(pool) String(pool);
		{
			char *buf=(char *)pool.malloc(MAX_NUMBER);
			snprintf(buf, MAX_NUMBER, "%d", i);
			skey << buf;
		}
		hash.put(skey, parse_IFD_entry_formatted_one_value(pool, is_big, format, component_size, value));
	}

	return &result;
}

static Value *parse_IFD_entry_value(Pool& pool,
									bool is_big, Measure_reader& reader, long tiff_base,
									JPG_Exif_IFD_entry& entry) {
	size_t format2component_size[]={
		0, // undefined
		1, // unsigned byte
		1, // ascii string
		2, // unsigned short
		4, // unsigned long
		8, // unsigned rational
		1, // signed byte
		0, // undefined
		2, // signed short
		4, // signed long
		8, // signed rational
		/*
		4, // single float
		8, // double float
		*/
	};

	ushort format=endian_to_ushort(is_big, entry.format);
	if(format>=sizeof(format2component_size)/sizeof(format2component_size[0]))
		return 0; // format out of range, ignoring

	size_t component_size=format2component_size[format];
	if(component_size==0)
		return 0; // undefined format

	// You can get the total data byte length by multiplies 
	// a 'bytes/components' value (see above chart) by number of components stored 'NNNNNNNN' area
	uint components_count=endian_to_uint(is_big, entry.components_count);
	size_t value_size=component_size*components_count;
	// If its size is over 4bytes, 'DDDDDDDD' contains the offset to data stored address
	Value *result;

	if(value_size<=4)
		result=parse_IFD_entry_formatted_value(pool,
			is_big, format, 
			component_size, components_count, 
			entry.value_or_offset_to_it);
	else {
		long remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it), SEEK_SET);
			const void *value;
			if(reader.read(value, value_size)<sizeof(value_size))
				return 0;
			result=parse_IFD_entry_formatted_value(pool,
				is_big, format, 
				component_size, components_count, 
				(const uchar*)value);
		}
		reader.seek(remembered, SEEK_SET);
	}

	return result;
}

static void parse_IFD(Pool& pool,
					  Hash& hash,
					  bool is_big, Measure_reader& reader, long tiff_base);

static void parse_IFD_entry(Pool& pool, Hash& hash,
							bool is_big, Measure_reader& reader, long tiff_base,
							JPG_Exif_IFD_entry& entry) {
	ushort tag=endian_to_ushort(is_big, entry.tag);
	if(tag==JPG_IFD_TAG_EXIF_OFFSET) {
		long remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it), SEEK_SET);
			parse_IFD(pool, hash, is_big, reader, tiff_base);
		}
		reader.seek(remembered, SEEK_SET);
		return;
	}
	
	if(Value *value=parse_IFD_entry_value(pool, is_big, reader, tiff_base, entry)) {
		String& skey=*new(pool) String(pool);
		{
			char *buf=(char *)pool.malloc(MAX_NUMBER);
			snprintf(buf, MAX_NUMBER, "%u", tag);
			skey << buf;
		}

		if(const char *name=(const char *)exif_tag_value2name->get(skey))
			hash.put(*new(pool) String(pool, name), value);
		else
			hash.put(skey, value);
	}
}

static void parse_IFD(Pool& pool,
					  Hash& hash,
					  bool is_big, Measure_reader& reader, long tiff_base) {
	const void *buf;
	if(reader.read(buf, sizeof(JPG_Exif_IFD_start))<sizeof(JPG_Exif_IFD_start))
		return;
	JPG_Exif_IFD_start *start=(JPG_Exif_IFD_start *)buf;

	ushort directory_entry_count=endian_to_ushort(is_big, start->directory_entry_count);
	for(int i=0; i<directory_entry_count; i++) {
		if(reader.read(buf, sizeof(JPG_Exif_IFD_entry))<sizeof(JPG_Exif_IFD_entry))
			return;

		parse_IFD_entry(pool, hash, is_big, reader, tiff_base, *(JPG_Exif_IFD_entry *)buf);
	}
	// then goes: LLLLLLLL Offset to next IFD [not going there]
}

static Value *parse_exif(Pool& pool,
					   Measure_reader& reader,
					   const String *origin_string) {
	const void *buf;
	if(reader.read(buf, sizeof(JPG_Exif_segment_start))<sizeof(JPG_Exif_segment_start))
		throw Exception("image.format", 
			origin_string, 
			"not JPEG file - can not fully read Exif segment start");

	JPG_Exif_segment_start *start=(JPG_Exif_segment_start *)buf;
	if(memcmp(start->signature, "Exif\0\0", 4+2)!=0) //signature invalid?
		return 0; // ignore invalid block

	uint tiff_base=reader.tell();
	if(reader.read(buf, sizeof(JPG_Exif_TIFF_header))<sizeof(JPG_Exif_TIFF_header))
		return 0;

	JPG_Exif_TIFF_header *head=(JPG_Exif_TIFF_header *)buf;
	bool is_big=head->byte_align_identifier[0]=='M'; // [M]otorola vs [I]ntel

	uint first_IFD_offset=endian_to_uint(is_big, head->first_IFD_offset);
	reader.seek(tiff_base+first_IFD_offset, SEEK_SET);

	VHash& vhash=*new(pool) VHash(pool);

	// IFD
	parse_IFD(pool, vhash.hash(0), is_big, reader, tiff_base);

	return &vhash;
}

static void measure_jpeg(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height, Value ** exif) {
	// JFIF format markers
	const uchar MARKER=0xFF;
	const uchar CODE_SIZE_A=0xC0;
	const uchar CODE_SIZE_B=0xC1;
	const uchar CODE_SIZE_C=0xC2;
	const uchar CODE_SIZE_D=0xC3;
	const uchar CODE_EXIF=0xE1;

	const void *buf;
	const size_t prefix_size=2;
	if(reader.read(buf, prefix_size)<prefix_size)
		throw Exception("image.format", 
			origin_string, 
			"not JPEG file - too small");
	uchar *signature=(uchar *)buf;
	
	if(!(signature[0]==0xFF && signature[1]==0xD8)) 
		throw Exception("image.format", 
			origin_string, 
			"not JPEG file - wrong signature");

	while(true) {
		uint segment_base=reader.tell()+2/*marker,code*/;
		if(reader.read(buf, sizeof(JPG_Segment_head))<sizeof(JPG_Segment_head))
			break;
		JPG_Segment_head *head=(JPG_Segment_head *)buf;

        // Verify that it's a valid segment.
		if(head->marker!=MARKER)
			throw Exception("image.format", 
				origin_string, 
				"not JPEG file - marker not found");

		switch(head->code) {
		// http://www.ba.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
		case CODE_EXIF:
			if(exif && !*exif) // seen .jpg with some xml under EXIF tag, after real exif block :)
				*exif=parse_exif(pool, reader, origin_string);
			break;

		case CODE_SIZE_A:
		case CODE_SIZE_B:
		case CODE_SIZE_C:
		case CODE_SIZE_D:
			{
				// Segments that contain size info
				if(reader.read(buf, sizeof(JPG_Size_segment_body))<sizeof(JPG_Size_segment_body))
					throw Exception("image.format", 
						origin_string, 
						"not JPEG file - can not fully read Size segment");
				JPG_Size_segment_body *body=(JPG_Size_segment_body *)buf;
				
				width=endian_to_ushort(true, body->width);
				height=endian_to_ushort(true, body->height);
			}			
			return;
		};

		reader.seek(segment_base+endian_to_ushort(true, head->length), SEEK_SET);
	}

	throw Exception("image.format", 
		origin_string, 
		"broken JPEG file - size frame not found");
}

static void measure_png(Pool& pool, const String *origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height) {

	const void *buf;
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

	width=endian_to_ushort(true, head->width);
	height=endian_to_ushort(true, head->height);
}

// measure center

static void measure(Pool& pool, const String& file_name, 
			 Measure_reader& reader, ushort& width, ushort& height, Value ** exif) {
	if(const char *cext=strrchr(file_name.cstr(String::UL_FILE_SPEC), '.')) {
		cext++;
		if(strcasecmp(cext, "GIF")==0)
			measure_gif(pool, &file_name, reader, width, height);
		else if(strcasecmp(cext, "JPG")==0 || strcasecmp(cext, "JPEG")==0) 
			measure_jpeg(pool, &file_name, reader, width, height, exif);
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

// methods

#ifndef DOXYGEN
struct File_measure_action_info {
	ushort *width;
	ushort *height;
	Value ** exif;
	const String *file_name;
};
#endif
static void file_measure_action(Pool& pool,
								struct stat& finfo, int f, 
								const String& file_spec, const char *fname, bool as_text,
								void *context) {
	File_measure_action_info& info=*static_cast<File_measure_action_info *>(context);

	Measure_file_reader reader(pool, f, *info.file_name, fname);
	measure(pool, *info.file_name, reader, *info.width, *info.height, info.exif);
}

static void _measure(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& data=params->as_no_junction(0, "data must not be code");

	ushort width=0;
	ushort height=0;
	Value *exif=0;
	const String *file_name;
	if(file_name=data.get_string()) {
		File_measure_action_info info={&width, &height, &exif, file_name};
		file_read_action_under_lock(pool, r.absolute(*file_name), 
			"measure", file_measure_action, &info);
	} else {
		const VFile& vfile=*data.as_vfile();
		file_name=&static_cast<Value *>(vfile.fields().get(*name_name))->as_string();
		Measure_buf_reader reader(
			vfile.value_ptr(),
			vfile.value_size(),
			*file_name
		);
		measure(pool, *file_name, reader, width, height, &exif);
	}

	VImage &vimage=*static_cast<VImage *>(r.get_self());
	vimage.set(file_name, width, height, 0, exif);
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

	const Hash& fields=static_cast<VImage *>(r.get_self())->fields();
	Hash *attribs=0;

	if(params->size()) {
		// for backward compatibility: someday was ^html{}
		Value &vattribs=r.process_to_value(params->get(0),
			/*0/*no name* /,*/
			false/*don't intercept string*/);
		if(!vattribs.is_string()) // allow empty
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
	static_cast<VImage *>(r.get_self())->set(&file_name, width, height, &image);
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
	static_cast<VImage *>(r.get_self())->set(0, width, height, &image);
}

static void _gif(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

#define Y(y)(y+index*height)

/// simple gdImage-based font storage & text output 
class Font: public Pooled {
public:
	
	const static int letter_spacing;
	int height;	    ///< Font heigth
	int monospace;	    ///< Default char width
	int spacebarspace; ///< spacebar width
	gdImage& ifont;
	const String& alphabet;
	
	Font(Pool& pool, 
		const String& aalphabet, 
		gdImage& aifont, int aheight, int amonospace, int aspacebarspace): Pooled(pool), 
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
			for(int y=0; y<height; y++)
				if(ifont.GetPixel(x, Y(y))!=tr) 
					return x+1;
		}
		return 0;
	}
	
	void index_display(gdImage& image, int x, int y, int index){
		if(index>=0) 
			ifont.Copy(image, x, y, 0, Y(0), index_width(index), height);
	}
	
	/* ******************************** string ********************************** */
	
	int step_width(int index) {
		return letter_spacing + (monospace ? monospace : index_width(index));
	}

	// counts trailing letter_spacing, consider this OK. useful for contiuations
	int string_width(const String& s){
		const char *cstr=s.cstr();
		int result=0;
		for(; *cstr; cstr++)
			result+=step_width(index_of(*cstr));
		return result;
	}
	
	void string_display(gdImage& image, int x, int y, const String& s){
		const char *cstr=s.cstr();
		if(cstr) for(; *cstr; cstr++) {
			int index=index_of(*cstr);
			index_display(image, x, y, index);
			x+=step_width(index);
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

	if(int remainder=image.SY() % alphabet.size())
		throw Exception("parser.runtime",
			&method_name,
			"font-file height(%d) not divisable by alphabet size(%d), remainder=%d",
				image.SY(), alphabet.size(), remainder);
	
	static_cast<VImage *>(r.get_self())->font=new(pool) Font(pool, 
		alphabet, 
		image, 
		image.SY() / alphabet.size(), monospace_width, spacebar_width);
}

static void _text(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	int x=params->as_int(0, "x must be int", r);
	int y=params->as_int(1, "y must be int", r);
	const String& s=params->as_string(2, "text must not be code");

	VImage& vimage=*static_cast<VImage *>(r.get_self());
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

	VImage& vimage=*static_cast<VImage *>(r.get_self());
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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

	gdImage *image=static_cast<VImage *>(r.get_self())->image;
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
	gdImage *src=0;

	Value& value=params->as_no_junction(index, msg);

	if(Value *vimage=value.as(VIMAGE_TYPE, false)) {
		src=static_cast<VImage *>(vimage)->image;
		if(!src)
			throw Exception("parser.runtime", 
				&method_name, 
				msg);
	} else
		throw Exception("parser.runtime", 
			&method_name, 
			msg);

	return *src;
}

static void _copy(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	gdImage *dest=static_cast<VImage *>(r.get_self())->image;
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
