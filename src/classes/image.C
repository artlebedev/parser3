/** @file
	Parser: @b image parser class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_IMAGE_C="$Date: 2007/04/23 10:30:09 $";

/*
	jpegsize: gets the width and height (in pixels) of a jpeg file
	Andrew Tong, werdna@ugcs.caltech.edu           February 14, 1995
	modified slightly by alex@ed.ac.uk
	and further still by rjray@uswest.com
	optimization and general re-write from tmetro@vl.com
	from perl by paf@design.ru
*/

#include "pa_config_includes.h"

#include "pa_vmethod_frame.h"

#include "gif.h"

#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_vimage.h"
#include "pa_vdate.h"
#include "pa_table.h"

// class

class MImage: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&, HashStringValue&) { return new VImage(); }

public:
	MImage();

public: // Methoded
	bool used_directly() { return true; }

};

// globals

DECLARE_CLASS_VAR(image, new MImage, 0);

// helpers

/// value of exif tag -> it's value
class EXIF_tag_value2name: public Hash<int, const char*> {
public:
	EXIF_tag_value2name() {
	// image JPEG Exif
		#define EXIF_TAG(tag, name) \
			put(tag, #name);
		// Tags used by IFD0 (main image)
		EXIF_TAG(0x010e,	ImageDescription);
		EXIF_TAG(0x010f,	Make);
		EXIF_TAG(0x0110,	Model);
		EXIF_TAG(0x0112,	Orientation);
		EXIF_TAG(0x011a,	XResolution);
		EXIF_TAG(0x011b,	YResolution);
		EXIF_TAG(0x0128,	ResolutionUnit);
		EXIF_TAG(0x0131,	Software);
		EXIF_TAG(0x0132,	DateTime);
		EXIF_TAG(0x013e,	WhitePoint);
		EXIF_TAG(0x013f,	PrimaryChromaticities);
		EXIF_TAG(0x0211,	YCbCrCoefficients);
		EXIF_TAG(0x0213,	YCbCrPositioning);
		EXIF_TAG(0x0214,	ReferenceBlackWhite);
		EXIF_TAG(0x8298,	Copyright);
		EXIF_TAG(0x8769,	ExifOffset);
		// Tags used by Exif SubIFD
		EXIF_TAG(0x829a,	ExposureTime);
		EXIF_TAG(0x829d,	FNumber);
		EXIF_TAG(0x8822,	ExposureProgram);
		EXIF_TAG(0x8827,	ISOSpeedRatings);
		EXIF_TAG(0x9000,	ExifVersion);
		EXIF_TAG(0x9003,	DateTimeOriginal);
		EXIF_TAG(0x9004,	DateTimeDigitized);
		EXIF_TAG(0x9101,	ComponentsConfiguration);
		EXIF_TAG(0x9102,	CompressedBitsPerPixel);
		EXIF_TAG(0x9201,	ShutterSpeedValue);
		EXIF_TAG(0x9202,	ApertureValue);
		EXIF_TAG(0x9203,	BrightnessValue);
		EXIF_TAG(0x9204,	ExposureBiasValue);
		EXIF_TAG(0x9205,	MaxApertureValue);
		EXIF_TAG(0x9206,	SubjectDistance);
		EXIF_TAG(0x9207,	MeteringMode);
		EXIF_TAG(0x9208,	LightSource);
		EXIF_TAG(0x9209,	Flash);
		EXIF_TAG(0x920a,	FocalLength);
		EXIF_TAG(0x927c,	MakerNote);
		EXIF_TAG(0x9286,	UserComment);
		EXIF_TAG(0x9290,	SubsecTime);
		EXIF_TAG(0x9291,	SubsecTimeOriginal);
		EXIF_TAG(0x9292,	SubsecTimeDigitized);
		EXIF_TAG(0xa000,	FlashPixVersion);
		EXIF_TAG(0xa001,	ColorSpace);
		EXIF_TAG(0xa002,	ExifImageWidth);
		EXIF_TAG(0xa003,	ExifImageHeight);
		EXIF_TAG(0xa004,	RelatedSoundFile);
		EXIF_TAG(0xa005,	ExifInteroperabilityOffset);
		EXIF_TAG(0xa20e,	FocalPlaneXResolution);
		EXIF_TAG(0xa20f,	FocalPlaneYResolution);
		EXIF_TAG(0xa210,	FocalPlaneResolutionUnit);
		EXIF_TAG(0xa215,	ExposureIndex);
		EXIF_TAG(0xa217,	SensingMethod);
		EXIF_TAG(0xa300,	FileSource);
		EXIF_TAG(0xa301,	SceneType);
		EXIF_TAG(0xa302,	CFAPattern);
		// Misc Tags
		EXIF_TAG(0x00fe,	NewSubfileType);
		EXIF_TAG(0x00ff,	SubfileType);
		EXIF_TAG(0x012d,	TransferFunction);
		EXIF_TAG(0x013b,	Artist);
		EXIF_TAG(0x013d,	Predictor);
		EXIF_TAG(0x0142,	TileWidth);
		EXIF_TAG(0x0143,	TileLength);
		EXIF_TAG(0x0144,	TileOffsets);
		EXIF_TAG(0x0145,	TileByteCounts);
		EXIF_TAG(0x014a,	SubIFDs);
		EXIF_TAG(0x015b,	JPEGTables);
		EXIF_TAG(0x828d,	CFARepeatPatternDim);
		EXIF_TAG(0x828e,	CFAPattern);
		EXIF_TAG(0x828f,	BatteryLevel);
		EXIF_TAG(0x83bb,	IPTC/NAA);
		EXIF_TAG(0x8773,	InterColorProfile);
		EXIF_TAG(0x8824,	SpectralSensitivity);
		EXIF_TAG(0x8825,	GPSInfo);
		EXIF_TAG(0x8828,	OECF);
		EXIF_TAG(0x8829,	Interlace);
		EXIF_TAG(0x882a,	TimeZoneOffset);
		EXIF_TAG(0x882b,	SelfTimerMode);
		EXIF_TAG(0x920b,	FlashEnergy);
		EXIF_TAG(0x920c,	SpatialFrequencyResponse);
		EXIF_TAG(0x920d,	Noise);
		EXIF_TAG(0x9211,	ImageNumber);
		EXIF_TAG(0x9212,	SecurityClassification);
		EXIF_TAG(0x9213,	ImageHistory);
		EXIF_TAG(0x9214,	SubjectLocation);
		EXIF_TAG(0x9215,	ExposureIndex);
		EXIF_TAG(0x9216,	TIFF/EPStandardID);
		EXIF_TAG(0xa20b,	FlashEnergy);
		EXIF_TAG(0xa20c,	SpatialFrequencyResponse);
		EXIF_TAG(0xa214,	SubjectLocation);
		#undef EXIF_TAG
	}
} exif_tag_value2name;


#ifndef DOXYGEN
class Measure_reader {
public:
	virtual size_t read(const char* &buf, size_t limit)=0;
	virtual void seek(long value, int whence)=0;
	virtual long tell()=0;
};

class Measure_file_reader: public Measure_reader {
	const String& file_name; const char* fname;
	int f;

public:
	Measure_file_reader(int af, const String& afile_name, const char* afname): 
		file_name(afile_name), fname(afname), f(af) {
	}

	override size_t read(const char* &abuf, size_t limit) {
		if(limit==0)
			return 0;

		char* lbuf=new(PointerFreeGC) char[limit];
		size_t read_size=(size_t)::read(f, lbuf, limit);  abuf=lbuf;
		if(ssize_t(read_size)<0 || read_size>limit)
			throw Exception(0,
				&file_name, 
				"measure failed: actually read %u bytes count not in [0..%u] valid range", 
			read_size, limit);

		return read_size;
	}

	override void seek(long value, int whence) {
		if(lseek(f, value, whence)<0)
			throw Exception("image.format",
				&file_name, 
				"seek(value=%ld, whence=%d) failed: %s (%d), actual filename '%s'", 
					value, whence, strerror(errno), errno, fname);
	}

	override long tell() { return lseek(f, 0, SEEK_CUR); }

};

class Measure_buf_reader: public Measure_reader {

	const char* buf; size_t size;
	const String& file_name; 

	size_t offset;

public:
	Measure_buf_reader(const char* abuf, size_t asize, const String& afile_name): 
		buf(abuf), size(asize), file_name(afile_name), offset(0) {
	}
	
	override size_t read(const char* &abuf, size_t limit) {
		size_t to_read=min(limit, size-offset);
		abuf=buf+offset;
		offset+=to_read;
		return to_read;
	}

	override void seek(long value, int whence) {
		size_t new_offset;
		switch(whence) {
		case SEEK_CUR: new_offset=offset+value; break;
		case SEEK_SET: new_offset=(size_t)value; break;
		default: 
			throw Exception(0, 
				0, 
				"whence #%d not supported", 0, whence); 
			break; // never
		}
		
		if((ssize_t)new_offset<0 || new_offset>size)
			throw Exception("image.format",
				&file_name, 
				"seek(value=%l, whence=%d) failed: out of buffer, new_offset>size (%l>%l) or new_offset<0", 
					value, whence, new_offset, size);
		offset=new_offset;
	}

	override long tell() { return offset; }

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
struct JPG_Exif_segment_begin {
	char signature[6]; // Exif\0\0
};

/// JPEG Exif TIFF Header
struct JPG_Exif_TIFF_header {
	uchar byte_align_identifier[2];
	char dummy[2]; // always 000A [or 0A00]
	uchar first_IFD_offset[4]; // Usually the first IFD starts immediately next to TIFF header, so this offset has value '0x00000008'.
};

// JPEG Exif IFD start
struct JPG_Exif_IFD_begin {
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

static void measure_gif(const String& origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height) {

	const char* buf;
	const size_t head_size=sizeof(GIF_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception("image.format", 
			&origin_string, 
			"not GIF file - too small");
	GIF_Header *head=(GIF_Header *)buf;

	if(strncmp(head->signature, "GIF", 3)!=0)
		throw Exception("image.format", 
			&origin_string, 
			"not GIF file - wrong signature");	

	width=endian_to_ushort(false, head->width);
	height=endian_to_ushort(false, head->height);
}

static Value* parse_IFD_entry_formatted_one_value(
												  bool is_big,
												  ushort format, 
												  size_t component_size, 
												  const uchar *value) {
	switch(format) {
	case 1: // unsigned byte
		return new VInt((uchar)value[0]);
	case 3: // unsigned short
		return new VInt(endian_to_ushort(is_big, value));
	case 4: // unsigned long
		 // 'double' because parser's Int is signed
		return new VDouble(endian_to_uint(is_big, value));
	case 5: // unsigned rational
		{
			uint numerator=endian_to_uint(is_big, value); value+=component_size/2;
			uint denominator=endian_to_uint(is_big, value);
			if(!denominator)
				return 0;
			return new VDouble(((double)numerator)/denominator);
		}
	case 6: // signed byte
		return new VInt((signed char)value[0]);
	case 8: // signed short
		return new VInt((signed short)endian_to_ushort(is_big, value));
	case 9: // signed long
		return new VInt((signed int)endian_to_uint(is_big, value));
	case 10: // signed rational
		{
			signed int numerator=(signed int)endian_to_uint(is_big, value); value+=component_size/2;
			uint denominator=endian_to_uint(is_big, value);
			if(!denominator)
				return 0;
			return new VDouble(numerator/denominator);
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
tm cstr_to_time_t(char *cstr);

static Value* parse_IFD_entry_formatted_value(bool is_big, ushort format, 
					      size_t component_size, uint components_count, 
					      const uchar *value) {
	if(format==2) { // ascii string, exception: the only type with varying size
		const char* cstr=(const char* )value;
		size_t length=components_count;
		// Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes
		if(length==JPEG_EXIF_DATE_CHARS 
			&& isdigit((unsigned char)cstr[0])
			&& cstr[length-1]==0) {
			char cstr_writable[JPEG_EXIF_DATE_CHARS]; 
			strcpy(cstr_writable, cstr);

			try {
				return new VDate(cstr_to_time_t(cstr_writable));
			}
			catch(...) { /*ignore bad date times*/ }
		}

		if(const char* premature_zero_pos=(const char* )memchr(cstr, 0, length))
			length=premature_zero_pos-cstr;
		return new VString(*new String(cstr, length, true/*tainted*/));
	}

	if(components_count==1)
		return parse_IFD_entry_formatted_one_value(is_big, format, component_size, value);

	VHash* result=new VHash;
	HashStringValue& hash=result->hash();
	for(uint i=0; i<components_count; i++, value+=component_size) {
		hash.put(
			String::Body::Format(i),
			parse_IFD_entry_formatted_one_value(is_big, format, component_size, value));
	}

	return result;
}

static Value* parse_IFD_entry_value(
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
	Value* result;

	if(value_size<=4)
		result=parse_IFD_entry_formatted_value(
			is_big, format, 
			component_size, components_count, 
			entry.value_or_offset_to_it);
	else {
		long remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it), SEEK_SET);
			const char* value;
			if(reader.read(value, value_size)<sizeof(value_size))
				return 0;
			result=parse_IFD_entry_formatted_value(
				is_big, format, 
				component_size, components_count, 
				(const uchar*)value);
		}
		reader.seek(remembered, SEEK_SET);
	}

	return result;
}

static void parse_IFD(HashStringValue& hash,
		      bool is_big, Measure_reader& reader, long tiff_base);

static void parse_IFD_entry(HashStringValue& hash,
			    bool is_big, Measure_reader& reader, long tiff_base,
			    JPG_Exif_IFD_entry& entry) {
	ushort tag=endian_to_ushort(is_big, entry.tag);
	if(tag==JPG_IFD_TAG_EXIF_OFFSET) {
		long remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it), SEEK_SET);
			parse_IFD(hash, is_big, reader, tiff_base);
		}
		reader.seek(remembered, SEEK_SET);
		return;
	}
	
	if(Value* value=parse_IFD_entry_value(is_big, reader, tiff_base, entry)) {
		if(const char* name=exif_tag_value2name.get(tag))
			hash.put(String::Body(name), value);
		else
			hash.put(String::Body::Format(tag), value);
	}
}

static void parse_IFD(
		      HashStringValue& hash,
		      bool is_big, Measure_reader& reader, long tiff_base) {
	const char* buf;
	if(reader.read(buf, sizeof(JPG_Exif_IFD_begin))<sizeof(JPG_Exif_IFD_begin))
		return;
	JPG_Exif_IFD_begin *start=(JPG_Exif_IFD_begin *)buf;

	ushort directory_entry_count=endian_to_ushort(is_big, start->directory_entry_count);
	for(int i=0; i<directory_entry_count; i++) {
		if(reader.read(buf, sizeof(JPG_Exif_IFD_entry))<sizeof(JPG_Exif_IFD_entry))
			return;

		parse_IFD_entry(hash, is_big, reader, tiff_base, *(JPG_Exif_IFD_entry *)buf);
	}
	// then goes: LLLLLLLL Offset to next IFD [not going there]
}

static Value* parse_exif(Measure_reader& reader, const String& origin_string) {
	const char* buf;
	if(reader.read(buf, sizeof(JPG_Exif_segment_begin))<sizeof(JPG_Exif_segment_begin))
		throw Exception("image.format", 
			&origin_string, 
			"not JPEG file - can not fully read Exif segment start");

	JPG_Exif_segment_begin *start=(JPG_Exif_segment_begin *)buf;
	if(memcmp(start->signature, "Exif\0\0", 4+2)!=0) //signature invalid?
		return 0; // ignore invalid block

	uint tiff_base=reader.tell();
	if(reader.read(buf, sizeof(JPG_Exif_TIFF_header))<sizeof(JPG_Exif_TIFF_header))
		return 0;

	JPG_Exif_TIFF_header *head=(JPG_Exif_TIFF_header *)buf;
	bool is_big=head->byte_align_identifier[0]=='M'; // [M]otorola vs [I]ntel

	uint first_IFD_offset=endian_to_uint(is_big, head->first_IFD_offset);
	reader.seek(tiff_base+first_IFD_offset, SEEK_SET);

	VHash* vhash=new VHash;

	// IFD
	parse_IFD(vhash->hash(), is_big, reader, tiff_base);

	return vhash;
}

static void measure_jpeg(const String& origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height, Value** exif) {
	// JFIF format markers
	const uchar MARKER=0xFF;
	const uchar CODE_SIZE_A=0xC0;
	const uchar CODE_SIZE_B=0xC1;
	const uchar CODE_SIZE_C=0xC2;
	const uchar CODE_SIZE_D=0xC3;
	const uchar CODE_EXIF=0xE1;

	const char* buf;
	const size_t prefix_size=2;
	if(reader.read(buf, prefix_size)<prefix_size)
		throw Exception("image.format", 
			&origin_string, 
			"not JPEG file - too small");
	uchar *signature=(uchar *)buf;
	
	if(!(signature[0]==0xFF && signature[1]==0xD8)) 
		throw Exception("image.format", 
			&origin_string, 
			"not JPEG file - wrong signature");

	while(true) {
		uint segment_base=reader.tell()+2/*marker,code*/;
		if(reader.read(buf, sizeof(JPG_Segment_head))<sizeof(JPG_Segment_head))
			break;
		JPG_Segment_head *head=(JPG_Segment_head *)buf;

        // Verify that it's a valid segment.
		if(head->marker!=MARKER)
			throw Exception("image.format", 
				&origin_string, 
				"not JPEG file - marker not found");

		switch(head->code) {
		// http://www.ba.wakwak.com/~tsuruzoh/Computer/Digicams/exif-e.html
		case CODE_EXIF:
			if(exif && !*exif) // seen .jpg with some xml under EXIF tag, after real exif block :)
				*exif=parse_exif(reader, origin_string);
			break;

		case CODE_SIZE_A:
		case CODE_SIZE_B:
		case CODE_SIZE_C:
		case CODE_SIZE_D:
			{
				// Segments that contain size info
				if(reader.read(buf, sizeof(JPG_Size_segment_body))<sizeof(JPG_Size_segment_body))
					throw Exception("image.format", 
						&origin_string, 
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
		&origin_string, 
		"broken JPEG file - size frame not found");
}

static void measure_png(const String& origin_string, 
			 Measure_reader& reader, ushort& width, ushort& height) {

	const char* buf;
	const size_t head_size=sizeof(PNG_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception("image.format", 
			&origin_string, 
			"not PNG file - too small");
	PNG_Header *head=(PNG_Header *)buf;

	if(strncmp(head->signature, "IHDR", 4)!=0)
		throw Exception("image.format", 
			&origin_string, 
			"not PNG file - wrong signature");	

	width=endian_to_ushort(true, head->width);
	height=endian_to_ushort(true, head->height);
}

// measure center

static void measure(const String& file_name, 
			 Measure_reader& reader, ushort& width, ushort& height, Value** exif) {
	const char* file_name_cstr=file_name.cstr(String::L_FILE_SPEC);
	if(const char* cext=strrchr(file_name_cstr, '.')) {
		cext++;
		if(strcasecmp(cext, "GIF")==0)
			measure_gif(file_name, reader, width, height);
		else if(strcasecmp(cext, "JPG")==0 || strcasecmp(cext, "JPEG")==0) 
			measure_jpeg(file_name, reader, width, height, exif);
		else if(strcasecmp(cext, "PNG")==0)
			measure_png(file_name, reader, width, height);
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
	ushort* width;
	ushort* height;
	Value** exif;
	const String* file_name;
};
#endif
static void file_measure_action(
								struct stat& /*finfo*/, int f, 
								const String& /*file_spec*/, const char* fname, bool /*as_text*/,
								void *context) {
	File_measure_action_info& info=*static_cast<File_measure_action_info *>(context);

	Measure_file_reader reader(f, *info.file_name, fname);
	measure(*info.file_name, reader, *info.width, *info.height, info.exif);
}

static void _measure(Request& r, MethodParams& params) {
	Value& data=params.as_no_junction(0, "data must not be code");

	ushort width=0;
	ushort height=0;
	Value* exif=0;
	const String* file_name;
	if((file_name=data.get_string())) {
		File_measure_action_info info={
			&width, &height,
			&exif,
			file_name
		};
		file_read_action_under_lock(r.absolute(*file_name), 
			"measure", file_measure_action, &info);
	} else {
		VFile* vfile=data.as_vfile();
		file_name=&vfile->fields().get(name_name)->as_string();
		Measure_buf_reader reader(
			vfile->value_ptr(),
			vfile->value_size(),
			*file_name
		);
		measure(*file_name, reader, width, height, &exif);
	}

	GET_SELF(r, VImage).set(file_name, width, height, 0, exif);
}

#ifndef DOXYGEN
struct Attrib_info {
	String* tag; ///< html tag being constructed
	HashStringValue* skip; ///< tag attributes not to append to tag string [to skip]
};
#endif
static void append_attrib_pair(
			       HashStringValue::key_type key, 
			       HashStringValue::value_type value, 
			       Attrib_info* info) {
	// skip user-specified and internal(starting with "line-") attributes 
	if(info->skip && info->skip->get(key) || key.pos("line-")==0)
		return;

	// src="a.gif" width=123 ismap[=-1]
	*info->tag << " " << key;
	if(value->is_string() || value->as_int()>=0)
		*info->tag << "=\"" << value->as_string() << "\"";
}
static void _html(Request& r, MethodParams& params) {

	String tag;
	tag << "<img";

	const HashStringValue& fields=GET_SELF(r, VImage).fields();
	HashStringValue* attribs=0;

	if(params.count()) {
		// for backward compatibility: someday was ^html{}
		Value& vattribs=r.process_to_value(params[0],
			false/*don't intercept string*/);
		if(!vattribs.is_string()) // allow empty
			if((attribs=vattribs.get_hash())) {
				Attrib_info info={&tag, 0};
				attribs->for_each<Attrib_info*>(append_attrib_pair, &info);
			} else
				throw Exception(PARSER_RUNTIME, 
					0, 
					"attributes must be hash");
	}

	{
		Attrib_info info={&tag, attribs};
		fields.for_each<Attrib_info*>(append_attrib_pair, &info);
	}
	tag << " />";
	r.write_pass_lang(tag);
}

/// @test wrap FILE to auto-object
static gdImage* load(Request& r, 
					 const String& file_name){
	const char* file_name_cstr=r.absolute(file_name).cstr(String::L_FILE_SPEC);
	if(FILE *f=fopen(file_name_cstr, "rb")) {
		gdImage* image=new gdImage;
		bool ok=image->CreateFromGif(f);
		fclose(f);
		if(!ok)
			throw Exception("image.format", 
				&file_name,
				"is not in GIF format");
		return image;
	} else {
		throw Exception("file.missing", 
			0, 
			"can not open '%s'", file_name_cstr);
	}
}


static void _load(Request& r, MethodParams& params) {
	const String& file_name=params.as_string(0, "file name must not be code");

	gdImage* image=load(r, file_name);
	GET_SELF(r, VImage).set(&file_name, image->SX(), image->SY(), image);
}

static void _create(Request& r, MethodParams& params) {
	int width=params.as_int(0, "width must be int", r);
	int height=params.as_int(1, "height must be int", r);
	int bgcolor_value=0xffFFff;
	if(params.count()>2)
		bgcolor_value=params.as_int(2, "color must be int", r);
	gdImage* image=new gdImage;
	image->Create(width, height);
	image->FilledRectangle(0, 0, width-1, height-1, image->Color(bgcolor_value));
	GET_SELF(r, VImage).set(0, width, height, image);
}

static void _gif(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	const String *file_name=0;
	if(params.count()>0)
		file_name=&params.as_string(0, "file name must be string");

	gdBuf buf=image.Gif();
	
	VFile& vfile=*new VFile;
	Value* content_type=new VString(*new String("image/gif"));
	vfile.set(false/*not tainted*/, 
		(const char*)buf.ptr, buf.size, 
		file_name? file_name->cstr(String::L_FILE_SPEC): 0, 
		content_type);

	r.write_no_lang(vfile);
}

static void _line(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.Line(
		params.as_int(0, "x0 must be int", r), 
		params.as_int(1, "y0 must be int", r), 
		params.as_int(2, "x1 must be int", r), 
		params.as_int(3, "y1 must be int", r), 
		image.Color(params.as_int(4, "color must be int", r)));
}

static void _fill(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.Fill(
		params.as_int(0, "x must be int", r), 
		params.as_int(1, "y must be int", r), 
		image.Color(params.as_int(2, "color must be int", r)));
}

static void _rectangle(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.Rectangle(
		params.as_int(0, "x0 must be int", r), 
		params.as_int(1, "y0 must be int", r), 
		params.as_int(2, "x1 must be int", r), 
		params.as_int(3, "y1 must be int", r), 
		image.Color(params.as_int(4, "color must be int", r)));
}

static void _bar(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.FilledRectangle(
		params.as_int(0, "x0 must be int", r), 
		params.as_int(1, "y0 must be int", r), 
		params.as_int(2, "x1 must be int", r), 
		params.as_int(3, "y1 must be int", r), 
		image.Color(params.as_int(4, "color must be int", r)));
}

#ifndef DOXYGEN
static void add_point(Table::element_type row, 
					  gdImage::Point **p) {
	if(row->count()!=2)
		throw Exception(0,
			0,
			"coordinates table must contain two columns: x and y values");
	(**p).x=row->get(0)->as_int();
	(**p).y=row->get(1)->as_int();
	(*p)++;
}
#endif
static void _replace(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	Table* table=params.as_no_junction(2, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			0,
			"coordinates must be table");

	gdImage::Point *all_p=new(PointerFreeGC) gdImage::Point[table->count()];
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image.FilledPolygonReplaceColor(all_p, table->count(), 
		image.Color(params.as_int(0, "src color must be int", r)),
		image.Color(params.as_int(1, "dest color must be int", r)));
}

static void _polyline(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	Table* table=params.as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			0,
			"coordinates must be table");

	gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image.Polygon(all_p, table->count(), 
		image.Color(params.as_int(0, "color must be int", r)),
		false/*not closed*/);
}

static void _polygon(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	Table* table=params.as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(0,
			0,
			"coordinates must be table");

	gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image.Polygon(all_p, table->count(), 
		image.Color(params.as_int(0, "color must be int", r)));
}

static void _polybar(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	Table* table=params.as_no_junction(1, "coordinates must not be code").get_table();
	if(!table) 
		throw Exception(PARSER_RUNTIME,
			0,
			"coordinates must be table");

	gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
	gdImage::Point *add_p=all_p;	
	table->for_each(add_point, &add_p);
	image.FilledPolygon(all_p, table->count(), 
		image.Color(params.as_int(0, "color must be int", r)));
}

// font

#define Y(y)(y+index*height)

// Font class

const int Font::letter_spacing=1;

Font::Font(//, 
	const String& aalphabet, 
	gdImage* aifont, int aheight, int amonospace, int aspacebarspace):
	height(aheight), monospace(amonospace),  spacebarspace(aspacebarspace),
	ifont(aifont),
	alphabet(aalphabet)	{
}

/* ******************************** char ********************************** */

size_t Font::index_of(char ch) {
	if(ch==' ') return STRING_NOT_FOUND;
	return alphabet.pos(ch);
}

int Font::index_width(size_t index) {
	if(index==STRING_NOT_FOUND)
		return spacebarspace;
	int tr=ifont->GetTransparent();
	for(int x=ifont->SX()-1; x>=0; x--) {
		for(int y=0; y<height; y++)
			if(ifont->GetPixel(x, Y(y))!=tr) 
				return x+1;
	}
	return 0;
}

void Font::index_display(gdImage& image, int x, int y, size_t index){
	if(index!=STRING_NOT_FOUND) 
		ifont->Copy(image, x, y, 0, Y(0), index_width(index), height);
}

/* ******************************** string ********************************** */

int Font::step_width(int index) {
	return letter_spacing + (monospace ? monospace : index_width(index));
}

// counts trailing letter_spacing, consider this OK. useful for contiuations
int Font::string_width(const String& s){
	const char* cstr=s.cstr();
	int result=0;
	for(const char* current=cstr; *current; current++)
		result+=step_width(index_of(*current));
	return result;
}

void Font::string_display(gdImage& image, int x, int y, const String& s){
	const char* cstr=s.cstr();
	for(const char* current=cstr; *current; current++) {
		size_t index=index_of(*current);
		index_display(image, x, y, index);
		x+=step_width(index);
	}
}

//


static void _font(Request& r, MethodParams& params) {
	const String& alphabet=params.as_string(0, "alphabet must not be code");
	gdImage* image=load(r, params.as_string(1, "file_name must not be code"));
	int spacebar_width=params.as_int(2, "spacebar_width must be int", r);
	int monospace_width;
	if(params.count()>3) {
		monospace_width=params.as_int(3, "monospace_width must be int", r);
		if(!monospace_width)
			monospace_width=image->SX();
	} else
		monospace_width=0;

	if(!alphabet.length())
		throw Exception(PARSER_RUNTIME,
			0,
			"alphabet must not be empty");

	if(int remainder=image->SY() % alphabet.length())
		throw Exception(PARSER_RUNTIME,
			0,
			"font-file height(%d) not divisable by alphabet size(%d), remainder=%d",
				image->SY(), alphabet.length(), remainder);
	
	GET_SELF(r, VImage).set_font(new Font(
		alphabet, 
		image, 
		image->SY() / alphabet.length(), monospace_width, spacebar_width));
}

static void _text(Request& r, MethodParams& params) {
	int x=params.as_int(0, "x must be int", r);
	int y=params.as_int(1, "y must be int", r);
	const String& s=params.as_string(2, "text must not be code");

	VImage& vimage=GET_SELF(r, VImage);
	vimage.font().string_display(vimage.image(), x, y, s);
}

static void _length(Request& r, MethodParams& params) {
	const String& s=params.as_string(0, "text must not be code");

	VImage& vimage=GET_SELF(r, VImage);
	r.write_no_lang(*new VInt(vimage.font().string_width(s)));
}

static void _arc(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.Arc(
		params.as_int(0, "center_x must be int", r), 
		params.as_int(1, "center_y must be int", r), 
		params.as_int(2, "width must be int", r), 
		params.as_int(3, "height must be int", r), 
		params.as_int(4, "start degrees must be int", r), 
		params.as_int(5, "end degrees must be int", r), 
		image.Color(params.as_int(6, "cx must be int", r)));
}

static void _sector(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	image.Sector(
		params.as_int(0, "center_x must be int", r), 
		params.as_int(1, "center_y must be int", r), 
		params.as_int(2, "width must be int", r), 
		params.as_int(3, "height must be int", r), 
		params.as_int(4, "start degrees must be int", r), 
		params.as_int(5, "end degrees must be int", r), 
		image.Color(params.as_int(6, "color must be int", r)));
}

static void _circle(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	int size=params.as_int(2, "radius must be int", r)*2;
	image.Arc(
		params.as_int(0, "center_x must be int", r), 
		params.as_int(1, "center_y must be int", r), 
		size, //w
		size, //h
		0, //s
		360, //e
		image.Color(params.as_int(3, "color must be int", r)));
}

gdImage& as_image(MethodParams& params, int index, const char* msg) {
	Value& value=params.as_no_junction(index, msg);

	if(Value* vimage=value.as(VIMAGE_TYPE, false)) {
		return static_cast<VImage *>(vimage)->image();
	} else
		throw Exception(PARSER_RUNTIME, 
			0, 
			msg);
}

static void _copy(Request& r, MethodParams& params) {
	gdImage& dest=GET_SELF(r, VImage).image();

	gdImage& src=as_image(params, 0, "src must be image");

	int sx=params.as_int(1, "src_x must be int", r);
	int sy=params.as_int(2, "src_y must be int", r);
	int sw=params.as_int(3, "src_w must be int", r);
	int sh=params.as_int(4, "src_h must be int", r);
	int dx=params.as_int(5, "dest_x must be int", r);
	int dy=params.as_int(6, "dest_y must be int", r);
	if(params.count()>1+2+2+2) {
		int dw=params.as_int(1+2+2+2, "dest_w must be int", r);
		int dh=(int)(params.count()>1+2+2+2+1?
			params.as_int(1+2+2+2+1, "dest_h must be int", r):sh*(((double)dw)/((double)sw)));
		int tolerance=params.count()>1+2+2+2+2?
			params.as_int(1+2+2+2+2, "tolerance must be int", r):150;

		src.CopyResampled(dest, dx, dy, sx, sy, dw, dh, sw, sh, tolerance);
	} else
		src.Copy(dest, dx, dy, sx, sy, sw, sh);
}

static void _pixel(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	int x=params.as_int(0, "x must be int", r); 
	int y=params.as_int(1, "y must be int", r);

	if(params.count()>2) {
		image.SetPixel(x, y, 
			image.Color(params.as_int(2, "color must be int", r)));
	} else 
		r.write_no_lang(*new VInt(image.DecodeColor(image.GetPixel(x, y))));
}


// constructor

MImage::MImage(): Methoded("image") {
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
	add_native_method("gif", Method::CT_DYNAMIC, _gif, 0, 1);

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

	// ^image.pixel(x;y)[(color)]
	add_native_method("pixel", Method::CT_DYNAMIC, _pixel, 2, 3);
}
