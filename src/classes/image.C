/** @file
	Parser: @b image parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

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

#include "pa_vmethod_frame.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_vfile.h"
#include "pa_vimage.h"
#include "pa_vdate.h"
#include "pa_table.h"
#include "pa_charsets.h"

volatile const char * IDENT_IMAGE_C="$Id: image.C,v 1.181 2020/12/23 15:01:13 moko Exp $";

// defines

static const String spacebar_width_name("space");
static const String monospace_width_name("width");
static const String letter_spacing_name("spacing");

// class

class MImage: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VImage(); }

public:
	MImage();
};

// globals

DECLARE_CLASS_VAR(image, new MImage);

// helpers

#define EXIF_TAG(tag, name) put(tag, #name);

/// value of exif tag -> it's value
class EXIF_tag_value2name: public Hash<int, const char*> {
public:
	EXIF_tag_value2name() {
	// image JPEG Exif
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
		//EXIF_TAG(0x8825,	GPSInfo);
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

		// additional things added by misha@
		EXIF_TAG(0x0100,	ImageWidth);
		EXIF_TAG(0x0101,	ImageLength);
		EXIF_TAG(0x0102,	BitsPerSample);
		EXIF_TAG(0x0103,	Compression);
		EXIF_TAG(0x0106,	PhotometricInterpretation);
		EXIF_TAG(0x010a,	FillOrder);
		EXIF_TAG(0x010d,	DocumentName);
		EXIF_TAG(0x0111,	StripOffsets);
		EXIF_TAG(0x0115,	SamplesPerPixel);
		EXIF_TAG(0x0116,	RowsPerStrip);
		EXIF_TAG(0x0117,	StripByteCounts);
		EXIF_TAG(0x011c,	PlanarConfiguration);
		EXIF_TAG(0x0156,	TransferRange);
		EXIF_TAG(0x0200,	JPEGProc);
		EXIF_TAG(0x0201,	JPEGInterchangeFormat);
		EXIF_TAG(0x0202,	JPEGInterchangeFormatLength);
		EXIF_TAG(0x0212,	YCbCrSubSampling);
		EXIF_TAG(0xa401,	CustomRendered);
		EXIF_TAG(0xa402,	ExposureMode);
		EXIF_TAG(0xa403,	WhiteBalance);
		EXIF_TAG(0xa404,	DigitalZoomRatio);
		EXIF_TAG(0xa405,	FocalLengthIn35mmFilm);
		EXIF_TAG(0xa406,	SceneCaptureType);
		EXIF_TAG(0xa407,	GainControl);
		EXIF_TAG(0xa408,	Contrast);
		EXIF_TAG(0xa409,	Saturation);
		EXIF_TAG(0xa40a,	Sharpness);
		EXIF_TAG(0xa40b,	DeviceSettingDescription);
		EXIF_TAG(0xa40c,	SubjectDistanceRange);
		EXIF_TAG(0xa420,	ImageUniqueID);

		// other tags
		EXIF_TAG(0xa430,	CameraOwnerName);
		EXIF_TAG(0xa431,	BodySerialNumber);
		EXIF_TAG(0xa432,	LensSpecification);
		EXIF_TAG(0xa433,	LensManufactor);
		EXIF_TAG(0xa434,	LensModel);
		EXIF_TAG(0xa435,	LensSerialNumber);
	}
} exif_tag_value2name;

class EXIF_gps_tag_value2name: public Hash<int, const char*> {
public:
		EXIF_gps_tag_value2name() {
		EXIF_TAG(0x0,	GPSVersionID);
		EXIF_TAG(0x1,	GPSLatitudeRef);
		EXIF_TAG(0x2,	GPSLatitude);
		EXIF_TAG(0x3,	GPSLongitudeRef);
		EXIF_TAG(0x4,	GPSLongitude);
		EXIF_TAG(0x5,	GPSAltitudeRef);
		EXIF_TAG(0x6,	GPSAltitude);
		EXIF_TAG(0x7,	GPSTimeStamp);
		EXIF_TAG(0x8,	GPSSatellites);
		EXIF_TAG(0x9,	GPSStatus);
		EXIF_TAG(0xA,	GPSMeasureMode);
		EXIF_TAG(0xB,	GPSDOP);
		EXIF_TAG(0xC,	GPSSpeedRef);
		EXIF_TAG(0xD,	GPSSpeed);
		EXIF_TAG(0xE,	GPSTrackRef);
		EXIF_TAG(0xF,	GPSTrack);
		EXIF_TAG(0x10,	GPSImgDirectionRef);
		EXIF_TAG(0x11,	GPSImgDirection);
		EXIF_TAG(0x12,	GPSMapDatum);
		EXIF_TAG(0x13,	GPSDestLatitudeRef);
		EXIF_TAG(0x14,	GPSDestLatitude);
		EXIF_TAG(0x15,	GPSDestLongitudeRef);
		EXIF_TAG(0x16,	GPSDestLongitude);
		EXIF_TAG(0x17,	GPSDestBearingRef);
		EXIF_TAG(0x18,	GPSDestBearing);
		EXIF_TAG(0x19,	GPSDestDistanceRef);
		EXIF_TAG(0x1A,	GPSDestDistance);
		EXIF_TAG(0x1B,	GPSProcessingMethod);
		EXIF_TAG(0x1C,	GPSAreaInformation);
		EXIF_TAG(0x1D,	GPSDateStamp);
		EXIF_TAG(0x1E,	GPSDifferential);
	}
} exif_gps_tag_value2name;


///*********************************************** support functions

class Measure_reader {
public:
	virtual size_t read(const char* &buf, size_t limit)=0;
	virtual void seek(uint64_t value)=0;
	virtual uint64_t tell()=0;
	virtual uint64_t length()=0;
};

class Measure_file_reader: public Measure_reader {
	const String& file_name;
	int f;

public:
	Measure_file_reader(int af, const String& afile_name):
		file_name(afile_name), f(af) {
	}

	override size_t read(const char* &abuf, size_t limit) {
		if(limit==0)
			return 0;

		char* lbuf=new(PointerFreeGC) char[limit];
		ssize_t read_size=::read(f, lbuf, limit);  abuf=lbuf;
		if(read_size<0)
			throw Exception(0, &file_name, "measure read failed: %s (%d)", strerror(errno), errno);
		return read_size;
	}

	override void seek(uint64_t value) {
		if(pa_lseek(f, value, SEEK_SET)<0)
			throw Exception(IMAGE_FORMAT, &file_name, "seek to %.15g failed: %s (%d)", (double)value, strerror(errno), errno);
	}

	override uint64_t tell() { return pa_lseek(f, 0, SEEK_CUR); }

	override uint64_t length() { return pa_lseek(f, 0, SEEK_END); }

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

	override void seek(uint64_t value) {
		if(value>(uint64_t)size)
			throw Exception(IMAGE_FORMAT, &file_name, "seek to %.15g failed: out of buffer (%.15g)", value, size);
		offset=(size_t)value;
	}

	override uint64_t tell() { return offset; }

	override uint64_t length() { return size; }

};


struct Measure_info {
	ushort width;
	ushort height;
	Value** exif;
	Value** xmp;
	Charset* xmp_charset;
	bool	video;
};


inline ushort x_endian_to_ushort(uchar b0, uchar b1) {
	return (ushort)((b1<<8) + b0);
}

inline uint x_endian_to_uint(uchar b0, uchar b1, uchar b2, uchar b3) {
	return (uint)(((((b3<<8) + b2)<<8)+b1)<<8)+b0;
}

inline ushort endian_to_ushort(bool is_big, const uchar *b/* [2] */) {
	return is_big ? x_endian_to_ushort(b[1], b[0]) : x_endian_to_ushort(b[0], b[1]);
}

inline uint endian_to_uint(bool is_big, const uchar *b /* [4] */) {
	return is_big ? x_endian_to_uint(b[3], b[2], b[1], b[0]) : x_endian_to_uint(b[0], b[1], b[2], b[3]);
}


///*********************************************** JPEG

struct JPG_Segment_head {
	uchar marker;
	uchar code;
	uchar length[2];
};
/// JPEG frame header
struct JPG_Size_segment_body {
	char data;              //< data precision of bits/sample
	uchar height[2];        //< image height
	uchar width[2];         //< image width
	char numComponents;     //< number of color components
};

/// JPEG Exif TIFF Header
struct JPG_Exif_TIFF_header {
	char byte_align_identifier[2];
	uchar signature[2]; // always 000A [or 0A00]
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

#define JPG_IFD_TAG_EXIF_GPS_OFFSET 0x8825

#define JPEG_EXIF_DATE_CHARS 20

static Value* parse_IFD_entry_formatted_one_value(bool is_big, ushort format, size_t component_size, const uchar *value) {
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
		@todo
	case 12: // double float
		@todo
		*/
	};	
	
	return 0;
}

// date.C
tm cstr_to_time_t(char *, const char **);

static Value* parse_IFD_entry_formatted_value(bool is_big, ushort format, size_t component_size, uint components_count, const uchar *value) {
	if(format==2) { // ascii string, exception: the only type with varying size
		const char* cstr=(const char* )value;
		size_t length=components_count;
		// Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes
		if(length==JPEG_EXIF_DATE_CHARS && isdigit((unsigned char)cstr[0]) && cstr[length-1]==0) {
			char cstr_writable[JPEG_EXIF_DATE_CHARS];
			strcpy(cstr_writable, cstr);

			try {
				tm tmIn=cstr_to_time_t(cstr_writable, 0);
				return new VDate(tmIn);
			}
			catch(...) { /*ignore bad date times*/ }
		}

		return new VString(*new String(cstr, String::L_TAINTED));
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

static Value* parse_IFD_entry_value(bool is_big, Measure_reader& reader, uint64_t tiff_base, JPG_Exif_IFD_entry& entry) {
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
	uint value_size=component_size*components_count;
	// If its size is over 4bytes, 'DDDDDDDD' contains the offset to data stored address
	Value* result;

	if(value_size<=4)
		result=parse_IFD_entry_formatted_value(is_big, format, component_size, components_count, entry.value_or_offset_to_it);
	else {
		uint64_t remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it));
			const char* value;
			if(reader.read(value, value_size)<value_size)
				return 0;
			result=parse_IFD_entry_formatted_value(is_big, format, component_size, components_count, (const uchar*)value);
		}
		reader.seek(remembered);
	}

	return result;
}

static void parse_IFD(HashStringValue& hash, bool is_big, Measure_reader& reader, uint64_t tiff_base, bool gps=false);

static void parse_IFD_entry(HashStringValue& hash, bool is_big, Measure_reader& reader, uint64_t tiff_base, JPG_Exif_IFD_entry& entry, bool gps=false) {
	ushort tag=endian_to_ushort(is_big, entry.tag);

	if(tag==JPG_IFD_TAG_EXIF_OFFSET || tag==JPG_IFD_TAG_EXIF_GPS_OFFSET){
		uint64_t remembered=reader.tell();
		{
			reader.seek(tiff_base+endian_to_uint(is_big, entry.value_or_offset_to_it));
			parse_IFD(hash, is_big, reader, tiff_base, (tag==JPG_IFD_TAG_EXIF_GPS_OFFSET)?true:gps);
		}
		reader.seek(remembered);
		return;
	}

	if(Value* value=parse_IFD_entry_value(is_big, reader, tiff_base, entry)) {
		if(const char* name=(gps)?exif_gps_tag_value2name.get(tag):exif_tag_value2name.get(tag))
			hash.put(name, value);
		else
			hash.put(String::Body::Format(tag), value);
	}
}

static void parse_IFD(HashStringValue& hash, bool is_big, Measure_reader& reader, uint64_t tiff_base, bool gps) {
	const char* buf;
	if(reader.read(buf, sizeof(JPG_Exif_IFD_begin))<sizeof(JPG_Exif_IFD_begin))
		return;
	JPG_Exif_IFD_begin *start=(JPG_Exif_IFD_begin *)buf;

	ushort directory_entry_count=endian_to_ushort(is_big, start->directory_entry_count);
	for(int i=0; i<directory_entry_count; i++) {
		if(reader.read(buf, sizeof(JPG_Exif_IFD_entry))<sizeof(JPG_Exif_IFD_entry))
			return;

		parse_IFD_entry(hash, is_big, reader, tiff_base, *(JPG_Exif_IFD_entry *)buf, gps);
	}
	// then goes: LLLLLLLL Offset to next IFD [not going there]
}

static Value* parse_exif(Measure_reader& reader) {
	const char* buf;

	uint64_t tiff_base=reader.tell();
	if(reader.read(buf, sizeof(JPG_Exif_TIFF_header))<sizeof(JPG_Exif_TIFF_header))
		return 0;

	JPG_Exif_TIFF_header *head=(JPG_Exif_TIFF_header *)buf;
	bool is_big=head->byte_align_identifier[0]=='M'; // [M]otorola vs [I]ntel

	uint first_IFD_offset=endian_to_uint(is_big, head->first_IFD_offset);
	reader.seek(tiff_base+first_IFD_offset);

	VHash* vhash=new VHash;

	// IFD
	parse_IFD(vhash->hash(), is_big, reader, tiff_base);

	return vhash;
}

static Value* parse_xmp(Measure_reader& reader, ushort xmp_length, Measure_info &info) {
	const char* buf;
	if(reader.read(buf, xmp_length)<xmp_length)
		return 0;

	String::C xmp = Charset::transcode(String::C(pa_strdup(buf, xmp_length), xmp_length), *info.xmp_charset, pa_thread_request().charsets.source());
	return new VString(*new String(xmp, String::L_TAINTED));
}

static void measure_jpeg(const String& origin_string, Measure_reader& reader, Measure_info &info) {
	// JFIF format markers
	const uchar MARKER=0xFF;
	const uchar CODE_SIZE_A=0xC0;
	const uchar CODE_SIZE_B=0xC1;
	const uchar CODE_SIZE_C=0xC2;
	const uchar CODE_SIZE_D=0xC3;
	const uchar CODE_APP1=0xE1;

	const char* buf;
	const size_t prefix_size=2;
	if(reader.read(buf, prefix_size)<prefix_size)
		throw Exception(IMAGE_FORMAT, &origin_string, "not JPEG file - too small");
	uchar *signature=(uchar *)buf;
	
	if(!(signature[0]==0xFF && signature[1]==0xD8)) 
		throw Exception(IMAGE_FORMAT, &origin_string, "not JPEG file - wrong signature");

	while(true) {
		uint64_t segment_base=reader.tell()+2/*marker,code*/;
		if(reader.read(buf, sizeof(JPG_Segment_head))<sizeof(JPG_Segment_head))
			break;
		JPG_Segment_head *head=(JPG_Segment_head *)buf;

		// Verify that it's a valid segment.
		if(head->marker!=MARKER)
			throw Exception(IMAGE_FORMAT, &origin_string, "not JPEG file - marker not found");

		ushort segment_length=endian_to_ushort(true, head->length);

		switch(head->code) {
		// http://dev.exiv2.org/projects/exiv2/wiki/The_Metadata_in_JPEG_files
		case CODE_APP1:
			{
				const size_t EXIF_SIG_LEN=6; // Exif\0\0
				const size_t XMP_SIG_LEN=29; // http://ns.adobe.com/xap/1.0/\0

				if(segment_length<EXIF_SIG_LEN+2 || reader.read(buf, EXIF_SIG_LEN)<EXIF_SIG_LEN)
					break;
				if(memcmp(buf, "Exif\0\0", EXIF_SIG_LEN)==0){
					if(info.exif && !*info.exif) // backward compatibility: using first segment
						*info.exif=parse_exif(reader);
					break;
				}

				if(memcmp(buf, "http:/", EXIF_SIG_LEN))
					break;
				if(segment_length<XMP_SIG_LEN+2 || reader.read(buf, XMP_SIG_LEN-EXIF_SIG_LEN)<XMP_SIG_LEN-EXIF_SIG_LEN)
					break;
				if(memcmp(buf, "/ns.adobe.com/xap/1.0/\0", XMP_SIG_LEN-EXIF_SIG_LEN)==0){
					if(info.xmp && !*info.xmp) // backward compatibility: using first segment
						*info.xmp=parse_xmp(reader, segment_length - XMP_SIG_LEN - 2 /* segment_length */, info);
				}

			}
			break;

		case CODE_SIZE_A:
		case CODE_SIZE_B:
		case CODE_SIZE_C:
		case CODE_SIZE_D:
			{
				// Segments that contain size info
				if(reader.read(buf, sizeof(JPG_Size_segment_body))<sizeof(JPG_Size_segment_body))
					throw Exception(IMAGE_FORMAT, &origin_string, "not JPEG file - can not fully read Size segment");
				JPG_Size_segment_body *body=(JPG_Size_segment_body *)buf;
				
				info.width=endian_to_ushort(true, body->width);
				info.height=endian_to_ushort(true, body->height);
			}
			return;
		};

		reader.seek(segment_base + segment_length);
	}

	throw Exception(IMAGE_FORMAT, &origin_string, "broken JPEG file - size frame not found");
}

///*********************************************** TIFF

static bool parse_tiff_IFD(bool is_big, Measure_reader& reader, Measure_info &info) {
	const char* buf;
	if(reader.read(buf, sizeof(JPG_Exif_IFD_begin))<sizeof(JPG_Exif_IFD_begin))
		return false;
	JPG_Exif_IFD_begin *start=(JPG_Exif_IFD_begin *)buf;

	ushort directory_entry_count=endian_to_ushort(is_big, start->directory_entry_count);
	for(int i=0; i<directory_entry_count; i++) {
		if(reader.read(buf, sizeof(JPG_Exif_IFD_entry))<sizeof(JPG_Exif_IFD_entry))
			return false;

		JPG_Exif_IFD_entry *entry=(JPG_Exif_IFD_entry *)buf;
		ushort entry_tag=endian_to_ushort(is_big, entry->tag);

		if(entry_tag == 256 || entry_tag == 257){
			ushort entry_format=endian_to_ushort(is_big, entry->format);
			if(entry_format != 3 && entry_format != 4 || endian_to_uint(is_big, entry->components_count) != 1)
				return false;
			uint value = (entry_format == 3) ? endian_to_ushort(is_big, entry->value_or_offset_to_it) : endian_to_uint(is_big, entry->value_or_offset_to_it);
			(entry_tag == 256) ? info.width=(short)value : info.height=(short)value;
			if(info.width && info.height)
				return true;
		}
	}

	return false;
	// then goes: LLLLLLLL Offset to next IFD [not going there]
}

static void measure_tiff(const String& origin_string, Measure_reader& reader, Measure_info &info) {
	const char* buf;

	if(reader.read(buf, sizeof(JPG_Exif_TIFF_header))<sizeof(JPG_Exif_TIFF_header))
		throw Exception(IMAGE_FORMAT, &origin_string, "not TIFF file - too small");

	JPG_Exif_TIFF_header *head=(JPG_Exif_TIFF_header *)buf;

	if(strncmp(head->byte_align_identifier, "II", 2)!=0 && strncmp(head->byte_align_identifier, "MM", 2)!=0)
		throw Exception(IMAGE_FORMAT, &origin_string, "not TIFF file - wrong signature");

	bool is_big=head->byte_align_identifier[0]=='M'; // [M]otorola vs [I]ntel

	if(endian_to_ushort(is_big, head->signature) != 42)
		throw Exception(IMAGE_FORMAT, &origin_string, "not TIFF file - wrong signature");

	reader.seek(endian_to_uint(is_big, head->first_IFD_offset));
	if(!parse_tiff_IFD(is_big, reader, info))
		throw Exception(IMAGE_FORMAT, &origin_string, "broken TIFF file - size field entry not found");
}


///*********************************************** GIF

struct GIF_Header {
	char	signature[3];   // 'GIF'
	char	version[3];
	uchar	width[2];
	uchar	height[2];
	char	dif;
	char	fonColor;
	char	nulls;
};

static void measure_gif(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height) {

	const char* buf;
	const size_t head_size=sizeof(GIF_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception(IMAGE_FORMAT, &origin_string, "not GIF file - too small");
	GIF_Header *head=(GIF_Header *)buf;

	if(strncmp(head->signature, "GIF", 3)!=0)
		throw Exception(IMAGE_FORMAT, &origin_string, "not GIF file - wrong signature");

	width=endian_to_ushort(false, head->width);
	height=endian_to_ushort(false, head->height);
}


///*********************************************** PNG

struct PNG_Header {
	char	dummy[12];
	char	signature[4];   //< must be "IHDR"
	uchar	high_width[2];  //< image width high bytes [we ignore for now]
	uchar	width[2];       //< image width low bytes
	uchar	high_height[2]; //< image height high bytes [we ignore for now]
	uchar	height[4];      //< image height
};

static void measure_png(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height) {

	const char* buf;
	const size_t head_size=sizeof(PNG_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception(IMAGE_FORMAT, &origin_string, "not PNG file - too small");
	PNG_Header *head=(PNG_Header *)buf;

	if(strncmp(head->signature, "IHDR", 4)!=0)
		throw Exception(IMAGE_FORMAT, &origin_string, "not PNG file - wrong signature");

	width=endian_to_ushort(true, head->width);
	height=endian_to_ushort(true, head->height);
}


///*********************************************** BMP

struct BMP_Header {
	char	signature[2];   // 'BM'
	uchar	file_size[4];
	uchar	reserved[4];
	uchar	bitmap_offset[4];
	uchar	header_size[4];
	uchar	width[2];
	uchar	high_width[2];  //< image width high bytes [we ignore for now]
	uchar	height[2];
	uchar	high_height[2]; //< image height high bytes [we ignore for now]
};

static void measure_bmp(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height) {

	const char* buf;
	const size_t head_size=sizeof(BMP_Header);
	if(reader.read(buf, head_size)<head_size)
		throw Exception(IMAGE_FORMAT, &origin_string, "not BMP file - too small");
	BMP_Header *head=(BMP_Header *)buf;

	if(strncmp(head->signature, "BM", 2)!=0)
		throw Exception(IMAGE_FORMAT, &origin_string, "not BMP file - wrong signature");

	if((uint)reader.length() != endian_to_uint(false, head->file_size))
		throw Exception(IMAGE_FORMAT, &origin_string, "not BMP file - length header and file size do not match");

	width=endian_to_ushort(false, head->width);
	height=endian_to_ushort(false, head->height);
}


///*********************************************** WEBP

struct WEBP_Header {
	char signature_riff[4]; // 'RIFF'
	uchar file_size[4];
	char signature[4];      // 'WEBP'
	char format[4];         // 'VP8 ' or 'VP8L' or 'VP8X'
};

struct WEBP_VP8_Chunk {
	uchar size[4];
	char tag[3];
	uchar signature[3];    // 0x9D 0x01 0x2A
	uchar width[2];        // 14 bits each
	uchar height[2];       // 14 bits each
};

struct WEBP_VP8L_Chunk {
	uchar size[4];
	char signature;        // 0x2F
	uchar width_height[4]; // 14 bits each
};

struct WEBP_X_Chunk {
	uchar size[4];
	char reserved[4];
	uchar width[3];
	uchar height[3];
};

static void measure_webp(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height) {
	const char* buf;

	if(reader.read(buf, sizeof(WEBP_Header))<sizeof(WEBP_Header))
		throw Exception(IMAGE_FORMAT, &origin_string, "not WEBP file - too small");

	WEBP_Header *head=(WEBP_Header *)buf;

	if(strncmp(head->signature_riff, "RIFF", 4)!=0 || strncmp(head->signature, "WEBP", 4)!=0)
		throw Exception(IMAGE_FORMAT, &origin_string, "not WEBP file - wrong signature");

	if(strncmp(head->format, "VP8 ", 4)==0){
		if(reader.read(buf, sizeof(WEBP_VP8_Chunk))<sizeof(WEBP_VP8_Chunk))
			throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - too small VP8 chunk");

		WEBP_VP8_Chunk *chunk=(WEBP_VP8_Chunk *)buf;
		if (chunk->signature[0] != 0x9D || chunk->signature[1] != 0x01 || chunk->signature[2] != 0x2A)
			throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - wrong VP8 chunk signature");

		width=endian_to_ushort(false, chunk->width) & 0x3FFF;
		height=endian_to_ushort(false, chunk->height) & 0x3FFF;
	} else if(strncmp(head->format, "VP8L", 4)==0){
		if(reader.read(buf, sizeof(WEBP_VP8L_Chunk))<sizeof(WEBP_VP8L_Chunk))
			throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - too small VP8L chunk");

		WEBP_VP8L_Chunk *chunk=(WEBP_VP8L_Chunk *)buf;
		if(chunk->signature != 0x2F)
			throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - wrong VP8L chunk signature");

		uint wh=endian_to_uint(false, chunk->width_height);
		width=(wh & 0x3FFF) + 1;
		height=((wh >> 14) & 0x3FFF) + 1;
	} else if (strncmp(head->format, "VP8X", 4)==0){
		if(reader.read(buf, sizeof(WEBP_X_Chunk))<sizeof(WEBP_X_Chunk))
			throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - too small VP8X chunk");

		WEBP_X_Chunk *chunk=(WEBP_X_Chunk *)buf;

		width=endian_to_ushort(false, chunk->width) + 1;   // we ignore third byte to simplify code
		height=endian_to_ushort(false, chunk->height) + 1; // we ignore third byte to simplify code
	} else throw Exception(IMAGE_FORMAT, &origin_string, "broken WEBP file - invalid chunk signature");
}


///*********************************************** MP4

struct MP4_Header {
	uchar size[4];
	char signature[4];   // 'ftyp' in first chunk
};

struct MP4_ExtSize {
	uchar high[4];
	uchar low[4];
};

struct MP4_Tkhd {
	uchar width[4];
	uchar height[4];
};

static bool measure_mp4(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height, uint64_t anext, const char* lastTkhd=NULL) {
	for(bool first=anext==0;;){
		const char* buf;
		uint64_t next=reader.tell();

		if(reader.read(buf, sizeof(MP4_Header))<sizeof(MP4_Header))
			throw Exception(IMAGE_FORMAT, &origin_string, first ? "not MP4 file - too small" : "broken MP4 file - truncated chunk header");

		MP4_Header *head=(MP4_Header *)buf;
		uint64_t size=endian_to_uint(true, head->size);

		if(size==1){
			if(reader.read(buf, sizeof(MP4_ExtSize))<sizeof(MP4_ExtSize))
				throw Exception(IMAGE_FORMAT, &origin_string, "broken MP4 file - truncated chunk extended size header");
			MP4_ExtSize *ext_size=(MP4_ExtSize *)buf;
			size=((uint64_t)endian_to_uint(true, ext_size->high) << 32) + endian_to_uint(true, ext_size->low);
		}
		next+=size;

		if(first){
			if(strncmp(head->signature, "ftyp", 4)!=0)
				throw Exception(IMAGE_FORMAT, &origin_string, "not MP4 file - wrong signature");
			first=false;
			anext=reader.length(); // to avoid reading beyond EOF
		} else if(strncmp(head->signature, "moov", 4)==0 || strncmp(head->signature, "mdia", 4)==0 || strncmp(head->signature, "trak", 4)==0) {
			if(measure_mp4(origin_string, reader, width, height, next, lastTkhd))
				return true;
		} else if(strncmp(head->signature, "tkhd", 4)==0) {
			if(size>8){
				reader.seek(next-8);
				if(reader.read(lastTkhd, sizeof(MP4_Tkhd))<sizeof(MP4_Tkhd))
					throw Exception(IMAGE_FORMAT, &origin_string, "broken MP4 file - bad tkhd chunk");
			}
		} else if (strncmp(head->signature, "hdlr", 4)==0) {
			if(size>12){
				const char* hdlr;
				if(reader.read(hdlr, 12)<12)
					throw Exception(IMAGE_FORMAT, &origin_string, "broken MP4 file - bad hdlr chunk");
				if(lastTkhd && strncmp(hdlr+8, "vide", 4)==0) {
					MP4_Tkhd *tkhd=(MP4_Tkhd *)lastTkhd;
					width=endian_to_ushort(true, tkhd->width);
					height=endian_to_ushort(true, tkhd->height);
					return true;
				}
			}
		}
		if(anext && next>=anext)
			break;
		reader.seek(next);
	}
	return false;
}

static void measure_mp4(const String& origin_string, Measure_reader& reader, ushort& width, ushort& height) {
	if(!measure_mp4(origin_string, reader, width, height, 0))
		throw Exception(IMAGE_FORMAT, &origin_string, "unsupported MP4 file - size not found");
}

///*********************************************** measure center

static void measure(const String& file_name, Measure_reader& reader, Measure_info &info) {
	const char* file_name_cstr=file_name.taint_cstr(String::L_FILE_SPEC);
	if(const char* cext=strrchr(file_name_cstr, '.')) {
		cext++;
		if(strcasecmp(cext, "GIF")==0)
			measure_gif(file_name, reader, info.width, info.height);
		else if(strcasecmp(cext, "JPG")==0 || strcasecmp(cext, "JPEG")==0)
			measure_jpeg(file_name, reader, info);
		else if(strcasecmp(cext, "PNG")==0)
			measure_png(file_name, reader, info.width, info.height);
		else if(strcasecmp(cext, "BMP")==0)
			measure_bmp(file_name, reader, info.width, info.height);
		else if(strcasecmp(cext, "WEBP")==0)
			measure_webp(file_name, reader, info.width, info.height);
		else if(strcasecmp(cext, "TIF")==0 || strcasecmp(cext, "TIFF")==0)
			measure_tiff(file_name, reader, info);
		else if(strcasecmp(cext, "MP4")==0 || strcasecmp(cext, "MOV")==0)
			if(info.video)
				measure_mp4(file_name, reader, info.width, info.height);
			else
				throw Exception(IMAGE_FORMAT, &file_name, "handling disabled for file name extension '%s'", cext);
		else
			throw Exception(IMAGE_FORMAT, &file_name, "unhandled file name extension '%s'", cext);
	} else
		throw Exception(IMAGE_FORMAT, &file_name, "can not determine file type - no file name extension");
}

// methods

static void file_measure_action(struct stat& /*finfo*/, int f, const String& file_spec, void *context) {
	Measure_file_reader reader(f, file_spec);
	measure(file_spec, reader, *static_cast<Measure_info *>(context));
}

static void _measure(Request& r, MethodParams& params) {
	Value& data=params.as_no_junction(0, "data must not be code");

	Value* exif=0;
	Value* xmp=0;
	Measure_info info={ 0, 0, 0, 0, &pa_UTF8_charset, false };

	if(params.count()>1)
		if(HashStringValue* options=params.as_hash(1, "methods options")) {
			int valid_options=0;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == "exif") {
					if(r.process(*value).as_bool())
						info.exif=&exif;
					valid_options++;
				}
				if(key == "xmp") {
					if(r.process(*value).as_bool())
						info.xmp=&xmp;
					valid_options++;
				}
				if(key == "xmp-charset") {
					info.xmp_charset=&pa_charsets.get(value->as_string());
					valid_options++;
				}
				if(key == "video") {
					info.video=r.process(*value).as_bool();
					valid_options++;
				}
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	const String* file_name;

	if(file_name=data.get_string()) {
		file_read_action_under_lock(r.full_disk_path(*file_name), "measure", file_measure_action, &info);
	} else {
		VFile* vfile=data.as_vfile(String::L_AS_IS);
		file_name=&vfile->fields().get(name_name)->as_string();
		Measure_buf_reader reader(vfile->value_ptr(), vfile->value_size(), *file_name);
		measure(*file_name, reader, info);
	}

	GET_SELF(r, VImage).set(file_name, info.width, info.height, 0, exif, xmp);
}

static void append_attrib_pair(String &tag, String::Body key, Value* value){
	// skip border attribute with empty value
	if(key=="border" && !value->is_defined())
		return;

	// src="a.gif" width="123" ismap[=-1]
	tag << " " << key;
	if(value->is_string() || value->as_int()>=0)
		tag << "=\"" << value->as_string() << "\"";
}

static void _html(Request& r, MethodParams& params) {
	String tag;
	tag << "<img";

	const HashStringValue& fields=GET_SELF(r, VImage).fields();
	HashStringValue* attribs=0;

	if(params.count()) {
		// for backward compatibility: someday was ^html{}
		Value& vattribs=r.process(params[0]);
		if(!vattribs.is_string()) { // allow empty
			if((attribs=vattribs.get_hash())) {
				for(HashStringValue::Iterator i(*attribs); i; i.next() )
					append_attrib_pair(tag, i.key(), i.value());
			} else
				throw Exception(PARSER_RUNTIME, 0, "attributes must be hash");
		}
	}

	for(HashStringValue::Iterator i(fields); i; i.next() ){
		String::Body key=i.key();
		// skip user-specified attributes
		if(attribs && attribs->get(key))
			continue;
		// allow only html attributes (to exclude exif, line-*)
		if(key=="src" || key=="width" || key=="height" || key=="border")
			append_attrib_pair(tag, key, i.value());
	}

	tag << " />";
	r.write(tag);
}

/// @test wrap FILE to auto-object
static gdImage* load(Request& r, const String& file_name){
	const char* file_name_cstr=r.full_disk_path(file_name).taint_cstr(String::L_FILE_SPEC);
	if(FILE *f=pa_fopen(file_name_cstr, "rb")) {
		gdImage* image=new gdImage;
		bool ok=image->CreateFromGif(f);
		fclose(f);
		if(!ok)
			throw Exception(IMAGE_FORMAT, &file_name, "is not in GIF format");
		return image;
	} else {
		throw Exception("file.missing", 0, "can not open '%s'", file_name_cstr);
	}
}


static void _load(Request& r, MethodParams& params) {
	const String& file_name=params.as_string(0, FILE_NAME_MUST_NOT_BE_CODE);

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

	const String *file_name=params.count()>0?&params.as_string(0, FILE_NAME_MUST_BE_STRING):0;

	gdBuf buf=image.Gif();
	
	VFile& vfile=*new VFile;

	vfile.set_binary(false/*not tainted*/, (const char *)buf.ptr, buf.size, file_name, new VString(*new String("image/gif")));

	r.write(vfile);
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
static void add_point(Table::element_type row, gdImage::Point **p) {
	if(row->count()!=2)
		throw Exception(0, 0, "coordinates table must contain two columns: x and y values");
	(**p).x=row->get(0)->as_int();
	(**p).y=row->get(1)->as_int();
	(*p)++;
}
#endif
#ifndef DOXYGEN
static void add_point(int x, int y, gdImage::Point **p) {
	(**p).x=x;
	(**p).y=y;
	(*p)++;
}
#endif
static void _replace(Request& r, MethodParams& params) {
	int src_color=params.as_int(0, "src color must be int", r);
	int dest_color=params.as_int(1, "dest color must be int", r);

	gdImage& image=GET_SELF(r, VImage).image();

	gdImage::Point* all_p=0;
	size_t count=0;
	if(params.count() == 3){
		if(Table* table=params.as_table(2, "coordinates")){
			count=table->count();
			all_p=new(PointerFreeGC) gdImage::Point[count];
			gdImage::Point* add_p=all_p;
			table->for_each(add_point, &add_p);
		}
	} else {
		int max_x=image.SX()-1;
		int max_y=image.SY()-1;
		if(max_x > 0 && max_y > 0){
			count=4;
			all_p=new(PointerFreeGC) gdImage::Point[count];
			gdImage::Point* add_p=all_p;
			add_point(0, 0, &add_p);
			add_point(max_x, 0, &add_p);
			add_point(max_x, max_y, &add_p);
			add_point(0, max_y, &add_p);
		}
	}

	if(count)
		image.FilledPolygonReplaceColor(all_p, count, image.Color(src_color), image.Color(dest_color));
}

static void _polyline(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	if(Table* table=params.as_table(1, "coordinates")){
		gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
		gdImage::Point *add_p=all_p;
		table->for_each(add_point, &add_p);
		image.Polygon(all_p, table->count(), image.Color(params.as_int(0, "color must be int", r)), false/*not closed*/);
	}
}

static void _polygon(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	if(Table* table=(Table*)params.as_table(1, "coordinates")){
		gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
		gdImage::Point *add_p=all_p;
		table->for_each(add_point, &add_p);
		image.Polygon(all_p, table->count(), image.Color(params.as_int(0, "color must be int", r)));
	}
}

static void _polybar(Request& r, MethodParams& params) {
	gdImage& image=GET_SELF(r, VImage).image();

	if(Table* table=(Table*)params.as_table(1, "coordinates")){
		gdImage::Point* all_p=new(PointerFreeGC) gdImage::Point[table->count()];
		gdImage::Point *add_p=all_p;
		table->for_each(add_point, &add_p);
		image.FilledPolygon(all_p, table->count(), image.Color(params.as_int(0, "color must be int", r)));
	}
}

// font

#define Y(y)(y+index*height)

// Font class

Font::Font(Charset& asource_charset, const String& aalphabet, gdImage* aifont, int aheight, int amonospace, int aspacebarspace, int aletterspacing):
	letterspacing(aletterspacing),
	height(aheight),
	monospace(amonospace),
	spacebarspace(aspacebarspace),
	ifont(aifont),
	alphabet(aalphabet),
	fsource_charset(asource_charset){

	if(fsource_charset.isUTF8()){
		size_t index=0;
		for(UTF8_string_iterator i(alphabet); i.has_next(); )
			fletter2index.put_dont_replace(i.next(), index++);
	}
}

/* ******************************** char ********************************** */

size_t Font::index_of(char ch) {
	if(ch==' ') return STRING_NOT_FOUND;
	return alphabet.pos(ch);
}

size_t Font::index_of(XMLCh ch) {
	if(ch==' ') return STRING_NOT_FOUND;
	return fletter2index.get(ch);
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
	return letterspacing + (monospace ? monospace : index_width(index));
}

// counts trailing letter_spacing, consider this OK. useful for contiuations
int Font::string_width(const String& s){
	const char* cstr=s.cstr();
	int result=0;

	if(fsource_charset.isUTF8()){
		for(UTF8_string_iterator i(s); i.has_next(); )
			result+=step_width(index_of(i.next()));
	} else {
		for(const char* current=cstr; *current; current++)
			result+=step_width(index_of(*current));
	}

	return result;
}

void Font::string_display(gdImage& image, int x, int y, const String& s){
	const char* cstr=s.cstr();

	if(fsource_charset.isUTF8()){
		for(UTF8_string_iterator i(s); i.has_next(); ){
			size_t index=index_of(i.next());
			index_display(image, x, y, index);
			x+=step_width(index);
		}
	} else {
		for(const char* current=cstr; *current; current++) {
			size_t index=index_of(*current);
			index_display(image, x, y, index);
			x+=step_width(index);
		}
	}
}

//


static void _font(Request& r, MethodParams& params) {
	const String& alphabet=params.as_string(0, "alphabet must not be code");
	size_t alphabet_length=alphabet.length(r.charsets.source());
	if(!alphabet_length)
		throw Exception(PARSER_RUNTIME, 0, "alphabet must not be empty");

	gdImage* image=load(r, params.as_string(1, FILE_NAME_MUST_NOT_BE_CODE));

	int spacebar_width=image->SX();
	int monospace_width=0; // proportional
	int letter_spacing=1;
	if(params.count()>2){
		if(HashStringValue* options=params[2].get_hash()){
			// third option is hash
			if(params.count()>3)
				throw Exception(PARSER_RUNTIME, 0, "too many params were specified");
			int valid_options=0;
			if(Value* vspacebar_width=options->get(spacebar_width_name)){
				valid_options++;
				spacebar_width=r.process(*vspacebar_width).as_int();
			}
			if(Value* vmonospace_width=options->get(monospace_width_name)){
				valid_options++;
				monospace_width=r.process(*vmonospace_width).as_int();
				if(!monospace_width)
					monospace_width=image->SX();
			}
			if(Value* vletter_spacing=options->get(letter_spacing_name)){
				valid_options++;
				letter_spacing=r.process(*vletter_spacing).as_int();
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		} else {
			// backward
			spacebar_width=params.as_int(2, "param must be int or hash", r);
			if(params.count()>3) {
				monospace_width=params.as_int(3, "monospace_width must be int", r);
				if(!monospace_width)
					monospace_width=image->SX();
			}
		}
	}

	if(int remainder=image->SY() % alphabet_length)
		throw Exception(PARSER_RUNTIME, 0, "font-file height(%d) not divisable by alphabet size(%d), remainder=%d", image->SY(), alphabet_length, remainder);
	
	GET_SELF(r, VImage).set_font(new Font(r.charsets.source(), alphabet, image, image->SY() / alphabet_length, monospace_width, spacebar_width, letter_spacing));
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
	r.write(*new VInt(vimage.font().string_width(s)));
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

	if(Value* vimage=value.as(VIMAGE_TYPE)) {
		return static_cast<VImage *>(vimage)->image();
	} else
		throw Exception(PARSER_RUNTIME, 0, msg);
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
		r.write(*new VInt(image.DecodeColor(image.GetPixel(x, y))));
}


// constructor

MImage::MImage(): Methoded("image") {
	// ^image:measure[DATA]
	// ^image:measure[DATA; $.exif(false) $.xmp(false) $.xmp-charset[UTF-8] ]
	add_native_method("measure", Method::CT_DYNAMIC, _measure, 1, 2);

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
	// ^image.replace(color-source;color-dest)
	add_native_method("replace", Method::CT_DYNAMIC, _replace, 2, 3);

	// ^image.polyline(color)[table x:y]
	add_native_method("polyline", Method::CT_DYNAMIC, _polyline, 2, 2);

	// ^image.polygon(color)[table x:y]
	add_native_method("polygon", Method::CT_DYNAMIC, _polygon, 2, 2);

	// ^image.polybar(color)[table x:y]
	add_native_method("polybar", Method::CT_DYNAMIC, _polybar, 2, 2);

	// ^image.font[alPHAbet;font-file-name.gif]
	// ^image.font[alPHAbet;font-file-name.gif](spacebar_width)
	// ^image.font[alPHAbet;font-file-name.gif](spacebar_width;letter_width)
	// ^image.font[alPHAbet;font-file-name.gif][$.space-width(.) $.letter-width(.) $.letter-space(.)]
	add_native_method("font", Method::CT_DYNAMIC, _font, 2, 4);

	// ^image.text(x;y)[text]
	add_native_method("text", Method::CT_DYNAMIC, _text, 3, 3);
	
	// ^image.length[text]
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
