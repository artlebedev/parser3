/** @file
	Parser: globals.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_GLOBALS_C="$Date: 2002/11/26 15:10:46 $";

#include "pa_config_includes.h"

#ifdef XML
#include "libxslt/extensions.h"
#include "libxslt/xsltutils.h"
extern "C" {
#include "libexslt/exslt.h"
};
#endif

#include "pa_globals.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_sql_driver_manager.h"
#include "pa_dictionary.h"
#include "pa_stylesheet_manager.h"
#include "pa_sapi.h"
#include "pa_cache_managers.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_threads.h"

String *content_type_name;
String *charset_name;
String *body_name;
String *value_name;
String *expires_name;
String *path_name;
String *name_name;
String *size_name;
String *text_name;

String *content_disposition_name;
String *content_disposition_filename_name;

String *conf_method_name;
String *auto_method_name;

String *main_class_name;

String *result_var_name;
String *match_var_name;

String *exception_var_name;
String *exception_type_part_name;
String *exception_source_part_name;
String *exception_comment_part_name;
String *exception_handled_part_name;

String *charsets_name;
String *mime_types_name;
String *vfile_mime_type_name;
String *origins_mode_name;

String *class_path_name;

String *switch_data_name;

String *cache_data_name;

String *sql_limit_name;
String *sql_offset_name;
String *sql_default_name;
String *sql_distinct_name;

String *charset_UTF8_name;

String *hash_default_element_name;

String *http_method_name;
String *http_timeout_name;
String *http_headers_name;
String *file_status_name;


Table *string_match_table_template;

Table *date_calendar_table_template;

Hash *untaint_lang_name2enum;
Hash *exif_tag_value2name;

Charset *utf8_charset;

short hex_value[0x100];

#ifdef XML
GdomeDOMImplementation *domimpl;
#endif

static void setup_hex_value() {
	memset(hex_value, 0, sizeof(hex_value));
	hex_value['0'] = 0;	
	hex_value['1'] = 1;	
	hex_value['2'] = 2;	
	hex_value['3'] = 3;	
	hex_value['4'] = 4;	
	hex_value['5'] = 5;	
	hex_value['6'] = 6;	
	hex_value['7'] = 7;	
	hex_value['8'] = 8;	
	hex_value['9'] = 9;
	hex_value['A'] = 10;
	hex_value['B'] = 11;
	hex_value['C'] = 12;
	hex_value['D'] = 13;
	hex_value['E'] = 14;
	hex_value['F'] = 15;
	hex_value['a'] = 10;
	hex_value['b'] = 11;
	hex_value['c'] = 12;
	hex_value['d'] = 13;
	hex_value['e'] = 14;
	hex_value['f'] = 15;
}

#ifdef XML

const int MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS=10;

struct XML_Generic_error_info {
	pa_thread_t thread_id;
	char *message;
} xml_generic_error_infos[MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS];

XML_Generic_error_info *xml_generic_error_info(pa_thread_t thread_id) {
	for(int i=0; i<MAX_CONCURRENT_XML_GENERIC_ERROR_THREADS; i++) {
		XML_Generic_error_info *p=xml_generic_error_infos+i;
		if(p->thread_id==thread_id)
			return p;
	}
	return 0;
}

static void
xmlParserGenericErrorFunc(void *ctx, const char *msg, ...) { 
    pa_thread_t thread_id=pa_get_thread_id();

	// infinitely looking for free slot to fill it
	while(true) {
		SYNCHRONIZED;  // find+fill blocked

		// first try to get existing for this thread_id
		XML_Generic_error_info *p=xml_generic_error_info(thread_id);
		if(!p) { // occupy empty one
			p=xml_generic_error_info(0);
			if(!p) // wait for empty for it to appear
				continue;
		}

		p->thread_id=thread_id;
		size_t offset=p->message?strlen(p->message):0;
		p->message=(char *)realloc(p->message, offset+MAX_STRING);
		if(!p->message)
			SAPI::die(
				"out of memory in 'xmlParserGenericErrorFunc', failed to reallocate to %u bytes", 
				offset+MAX_STRING);
		
		va_list args;
		va_start(args, msg);
		vsnprintf(p->message+offset, MAX_STRING, msg, args);
		va_end(args);

		break;
	}
}

bool xmlHaveGenericErrors() {
    pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find blocked

	return xml_generic_error_info(thread_id)!=0;
}

const char *xmlGenericErrors() {
    pa_thread_t thread_id=pa_get_thread_id();

	SYNCHRONIZED;  // find+free blocked

	XML_Generic_error_info *p=xml_generic_error_info(thread_id);
	if(!p) // no errors for our thread_id registered
		return 0;

	const char *result=p->message;

	// free slot up 
	memset(p, 0, sizeof(*p));

	// it is up to caller to free it
	return result;
}

/**
 * xmlFileMatchWithLocalhostEqDocumentRoot:
 * filename:  the URI for matching
 *
 * check if the URI matches an HTTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
static int
xmlFileMatchLocalhost(const char *filename) {
    if (!strncmp(filename, "http://localhost", 16))
	return(1);
    return(0);
}


/**
 * xmlFileOpenHttpLocalhost :
 * filename:  the URI for matching
 *
 * http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
 *
 * input from FILE *, supports compressed input
 * if filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void *
xmlFileOpenLocalhost (const char *filename) {
    FILE *fd;
    const char* documentRoot;
    char path[1000];

	path[0]=0;
	strcat(path, (documentRoot=getenv("DOCUMENT_ROOT"))?documentRoot:".");
	strcat(path, &filename[16]);

#ifdef WIN32
    fd = fopen(path, "rb");
#else
    fd = fopen(path, "r");
#endif /* WIN32 */
    return((void *) fd);
}

/**
 * xmlFileRead:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Read @len bytes to @buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFileRead (void * context, char * buffer, int len) {
    return(fread(&buffer[0], 1,  len, (FILE *) context));
}

/**
 * xmlFileWrite:
 * @context:  the I/O context
 * @buffer:  where to drop data
 * @len:  number of bytes to write
 *
 * Write @len bytes from @buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFileWrite (void * context, const char * buffer, int len) {
    return(fwrite(&buffer[0], 1,  len, (FILE *) context));
}

/**
 * xmlFileClose:
 * @context:  the I/O context
 *
 * Close an I/O channel
 */
static int
xmlFileClose (void * context) {
    return ( ( fclose((FILE *) context) == EOF ) ? -1 : 0 );
}

#endif

void pa_globals_destroy(void *) {
	try {
#ifdef XML
		GdomeException exc;
		gdome_di_unref (domimpl, &exc);
#endif
		if(cache_managers)
			cache_managers->~Cache_managers();

		charsets->~Charsets();
		
	} catch(const Exception& e) {
		SAPI::die("pa_globals_destroy failed: %s", e.comment());
	}
}

/// @test hint on one should call this for each thread xmlSubstituteEntitiesDefault(1);
void pa_globals_init(Pool& pool) {
	pool.register_cleanup(pa_globals_destroy, 0);

	#undef NEW
	#define NEW new(pool)

	// hex value
	setup_hex_value();

	// names
	content_type_name=NEW String(pool, CONTENT_TYPE_NAME);
	charset_name=NEW String(pool, CHARSET_NAME);
	body_name=NEW String(pool, BODY_NAME);
	value_name=NEW String(pool, VALUE_NAME);
	expires_name=NEW String(pool, EXPIRES_NAME);
	path_name=NEW String(pool, PATH_NAME);
	name_name=NEW String(pool, NAME_NAME);
	size_name=NEW String(pool, SIZE_NAME);
	text_name=NEW String(pool, TEXT_NAME);

	content_disposition_name=NEW String(pool, CONTENT_DISPOSITION_NAME);
	content_disposition_filename_name=NEW String(pool, CONTENT_DISPOSITION_FILENAME_NAME);

	conf_method_name=NEW String(pool, CONF_METHOD_NAME);
	auto_method_name=NEW String(pool, AUTO_METHOD_NAME);

	main_class_name=NEW String(pool, MAIN_CLASS_NAME);

	result_var_name=NEW String(pool, RESULT_VAR_NAME);
	match_var_name=NEW String(pool, MATCH_VAR_NAME);

	exception_var_name=NEW String(pool, EXCEPTION_VAR_NAME);
	exception_type_part_name=NEW String(pool, EXCEPTION_TYPE_PART_NAME);
	exception_source_part_name=NEW String(pool, EXCEPTION_SOURCE_PART_NAME);
	exception_comment_part_name=NEW String(pool, EXCEPTION_COMMENT_PART_NAME);
	exception_handled_part_name=NEW String(pool, EXCEPTION_HANDLED_PART_NAME);

	charsets_name=NEW String(pool, CHARSETS_NAME);
	mime_types_name=NEW String(pool, MIME_TYPES_NAME);
	origins_mode_name=NEW String(pool, ORIGINS_MODE_NAME);

	class_path_name=NEW String(pool, CLASS_PATH_NAME);

	//^switch ^case
	switch_data_name=NEW String(pool, SWITCH_DATA_NAME);

	//^cache
	cache_data_name=NEW String(pool, CACHE_DATA_NAME);

	// sql
	sql_limit_name=NEW String(pool, SQL_LIMIT_NAME);
	sql_offset_name=NEW String(pool, SQL_OFFSET_NAME);
	sql_default_name=NEW String(pool, SQL_DEFAULT_NAME);
	sql_distinct_name=NEW String(pool, SQL_DISTINCT_NAME);

	// charsets
	charset_UTF8_name=NEW String(pool, CHARSET_UTF8_NAME);

	// hash
	hash_default_element_name=NEW String(pool, HASH_DEFAULT_ELEMENT_NAME);

	// http
	http_method_name=NEW String(pool, HTTP_METHOD_NAME);
	http_timeout_name=NEW String(pool, HTTP_TIMEOUT_NAME);
	http_headers_name=NEW String(pool, HTTP_HEADERS_NAME);
	file_status_name=NEW String(pool, FILE_STATUS_NAME);

	
	// hashes
	untaint_lang_name2enum=NEW Hash(pool);
	#define ULN(cstr, LANG) \
		untaint_lang_name2enum->put(*NEW String(pool, cstr), (int)String::UL_##LANG);
	ULN("as-is", AS_IS);
	ULN("file-spec", FILE_SPEC);
	ULN("http-header", HTTP_HEADER);
	ULN("mail-header", MAIL_HEADER);
	ULN("uri", URI);
	ULN("table", TABLE);
	ULN("sql", SQL);
	ULN("js", JS);
	ULN("xml", XML);
	ULN("html", HTML);
	ULN("optimized-html", HTML|String::UL_OPTIMIZE_BIT);

	// image JPEG Exif
	exif_tag_value2name=NEW Hash(pool);
	#define EXIF_TAG(tag, name) \
		{ \
			char *buf=(char *)malloc(MAX_NUMBER); \
			snprintf(buf, MAX_NUMBER, "%u", tag); \
			exif_tag_value2name->put(*NEW String(pool, buf), (void *)#name); \
		}
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

	// string_match_table_template
	{
		Array *columns=NEW Array(pool);
		*columns+=NEW String(pool, STRING_PRE_MATCH_NAME);
		*columns+=NEW String(pool, STRING_MATCH_NAME);
		*columns+=NEW String(pool, STRING_POST_MATCH_NAME);
		for(int i=1; i<=MAX_STRING_MATCH_TABLE_COLUMNS; i++) {
			char *column=(char *)pool.malloc(MAX_NUMBER);
			snprintf(column, MAX_NUMBER, "%d", i);
			*columns+=NEW String(pool, column); // .i column name
		}
		string_match_table_template=NEW Table(pool, 0, columns);
	}

	// date_calendar_table_template
	{
		Array *columns=NEW Array(pool);
		for(int i=0; i<=6; i++) {
			char *column=(char *)pool.malloc(MAX_NUMBER);
			snprintf(column, MAX_NUMBER, "%d", i);
			*columns+=NEW String(pool, column); // .i column name
		}
		*columns+=NEW String(pool, DATE_CALENDAR_WEEKNO_NAME);
		*columns+=NEW String(pool, DATE_CALENDAR_WEEKYEAR_NAME);
		date_calendar_table_template=NEW Table(pool, 0, columns);
	}

	// charsets
	charsets=NEW Charsets(pool);
	charsets->put(*charset_UTF8_name, 
		utf8_charset=NEW Charset(pool, *charset_UTF8_name, 0/*no file=system*/));


	// Status registration, must be initialized before all registrants
	cache_managers=NEW Cache_managers(pool);

	// SQL driver manager
	cache_managers->put(*NEW String(pool, "sql"), 
		SQL_driver_manager=NEW SQL_Driver_manager(pool));

#ifdef XML
	// initializing xml libs

	/* First I get a DOMImplementation reference */
	domimpl = gdome_di_mkref ();
    /*
     * Register the EXSLT extensions and the test module
     */
    exsltRegisterAll();
    xsltRegisterTestModule();
    xmlDefaultSAXHandlerInit();
    /*
     * disable CDATA from being built in the document tree
     */
    // never added yet  xmlDefaultSAXHandler.cdataBlock = NULL;

	/*
	 * Initialization function for the XML parser.
	 * This is not reentrant. Call once before processing in case of
	 * use in multithreaded programs.
	*/
	xmlInitParser();

	// 1. this is needed for proper parsing of stylesheets
	// there were a situation where honest entity ruined innocent xpath compilation
	// doc says "you sould turn it on on stylesheet load" without deepening into details
	// 2. when dom tree with entites goes under transform text nodes 
	// got [erroreosly] cut on first entity occurance
	// --
	// that is why this is:
	xmlSubstituteEntitiesDefault(1);
	
	// Bit in the loadsubset context field to tell to do ID/REFs lookups 
	xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
	// Bit in the loadsubset context field to tell to do complete the elements attributes lists 
	// with the ones defaulted from the DTDs 
    xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;

	// validate each document after load/create (?)
	//xmlDoValidityCheckingDefaultValue = 1;

//regretfully this not only replaces entities on parse, but also on generate	xmlSubstituteEntitiesDefault(1);
	// never switched this on xmlIndentTreeOutput=1;

	memset(xml_generic_error_infos, 0, sizeof(xml_generic_error_infos));
	xmlSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
	xsltSetGenericErrorFunc(0, xmlParserGenericErrorFunc);
//	FILE *f=fopen("y:\\xslt.log", "wt");
//	xsltSetGenericDebugFunc(f/*stderr*/, 0);

	// http://localhost/abc -> $ENV{DOCUMENT_ROOT}/abc | ./abc
	xmlRegisterInputCallbacks(
		xmlFileMatchLocalhost, xmlFileOpenLocalhost,
		xmlFileRead, xmlFileClose);

	// XSLT stylesheet manager
	cache_managers->put(*NEW String(pool, "stylesheet"), 
		stylesheet_manager=NEW Stylesheet_manager(pool));
#endif
}

#if defined(XML) && defined(_MSC_VER)
#	define GNOME_LIBS "/parser3project/win32xml/win32/gnome"
#	pragma comment(lib, GNOME_LIBS "/glib/lib/libglib-1.3-11.lib")
#	ifdef _DEBUG
#		pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/dsp/libxml2_so_debug/libxml2.lib")
#		pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_so_debug/libexslt.lib")
#		pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_so_debug/libxslt.lib")
#		pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/Debug/libgdome.lib")
#	else
#		pragma comment(lib, GNOME_LIBS "/libxml2-x.x.x/win32/dsp/libxml2_so_release/libxml2.lib")
#		pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libexslt_so_release/libexslt.lib")
#		pragma comment(lib, GNOME_LIBS "/libxslt-x.x.x/win32/dsp/libxslt_so_release/libxslt.lib")
#		pragma comment(lib, GNOME_LIBS "/gdome2-x.x.x/win32/dsp/Release/libgdome.lib")
#	endif
#endif
