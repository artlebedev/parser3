/** @file
	Parser: @b xdoc parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: xdoc.C,v 1.6 2001/09/28 15:58:26 parser Exp $
*/
#include "classes.h"
#ifdef XML

#include "pa_request.h"
#include "pa_vxdoc.h"
#include "pa_xslt_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "pa_vfile.h"
#include "xnode.h"

#include <strstream>
#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransENameMap.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterToHTML.hpp>
#include <XMLSupport/FormatterToText.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>
#include <XalanDOM/XalanElement.hpp>
#include <XalanDOM/XalanNodeList.hpp>

// defines

#define XDOC_CLASS_NAME "xdoc"

#define XDOC_OUTPUT_METHOD_OPTION_NAME "method"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"

#define XDOC_OUTPUT_ENCODING_OPTION_NAME "encoding"

#define XDOC_OUTPUT_DEFAULT_INDENT 4

// class

class MXdoc : public MXnode {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VXdoc(pool); }

public:
	MXdoc(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
	void configure_admin(Request& r);
};

// methods

class ParserStringXalanOutputStream: public XalanOutputStream {
public:
	
	explicit ParserStringXalanOutputStream(String& astring) : fstring(astring) {}

protected: // XalanOutputStream

	virtual void writeData(const char *theBuffer, unsigned long theBufferLength) {
		char *copy=(char *)fstring.malloc((size_t)theBufferLength);
		memcpy(copy, theBuffer, (size_t)theBufferLength);
		fstring.APPEND_CLEAN(copy, (size_t)theBufferLength, "xdoc", 0);
	}

	virtual void doFlush() {}

private:

	String& fstring;
	
};

static void create_optioned_listener(
									 const char *& content_type, const char *& charset, FormatterListener *& listener, 
									 Pool& pool, 
									 const String& method_name, MethodParams *params, int index, Writer& writer) {
	// default encoding from pool
	const String *scharset=&pool.get_charset();
	const String *method=0;
	XalanDOMString xalan_encoding;

	if(params->size()>index) {
		Value& voptions=params->as_no_junction(index, "options must not be code");
		if(voptions.is_defined()) {
			if(Hash *options=voptions.get_hash()) {
				// $.method[xml|html|text]
				if(Value *vmethod=static_cast<Value *>(options->get(*new(pool) 
					String(pool, XDOC_OUTPUT_METHOD_OPTION_NAME))))
					method=&vmethod->as_string();

				// $.encoding[windows-1251|...]
				if(Value *vencoding=static_cast<Value *>(options->get(*new(pool) 
					String(pool, XDOC_OUTPUT_ENCODING_OPTION_NAME)))) {
					scharset=&vencoding->as_string();
				}
			} else
				PTHROW(0, 0,
					&method_name,
					"options must be hash");
		}
	}

	xalan_encoding.append(charset=scharset->cstr());
	if(!method/*default='xml'*/ || *method == XDOC_OUTPUT_METHOD_OPTION_VALUE_XML) {
		content_type="text/xml";
		listener=new FormatterToXML(writer,
			XalanDOMString(),  // version
			true, // doIndent
			XDOC_OUTPUT_DEFAULT_INDENT, // indent 
			xalan_encoding  // encoding
		);
	} else if(*method == XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML) {
		content_type="text/html";
		listener=new FormatterToHTML(writer,
			xalan_encoding,  // encoding
			XalanDOMString(),  // mediaType 
			XalanDOMString(),  // doctypeSystem; String to be printed at the top of the document 
			XalanDOMString(),  // doctypePublic  
			true, // doIndent 
			XDOC_OUTPUT_DEFAULT_INDENT // indent 
		);
	} else if(*method == XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT) {
		content_type="text/plain";
		listener=new FormatterToText(writer,
			xalan_encoding  // encoding
		);
	} else
		PTHROW(0, 0,
			method,
			XDOC_OUTPUT_METHOD_OPTION_NAME " option is invalid; valid methods are: "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "', "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "', "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "'");			

	// never reached
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must not be code");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		XalanFileOutputStream stream(XalanDOMString(filespec, strlen(filespec)));
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 1, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...
	} catch(const XSLException& e) {
		r._throw(&method_name, e);
	}
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		String parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 0, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...

		// write out result
		r.write_no_lang(parserString);
	} catch(const XSLException& e) {
		r._throw(&method_name, e);
	}
}


static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		String& parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 0, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...

		// write out result
		VFile& vfile=*new(pool) VFile(pool);
		const char *cstr=parserString.cstr();
		String *scontent_type=new(pool) String(pool, content_type);
		Value *vcontent_type;
		if(charset) {
			VHash *vhcontent_type=new(pool) VHash(pool);
			vhcontent_type->hash().put(*value_name, new(pool) VString(*scontent_type));
			String *scharset=new(pool) String(pool, charset);
			vhcontent_type->hash().put(*new(pool) String(pool, "charset"), new(pool) VString(*scharset));
			vcontent_type=vhcontent_type;
		} else
			vcontent_type=new(pool) VString(*scontent_type);
		vfile.set(false/*tainted*/, cstr, strlen(cstr), 0/*file_name*/, vcontent_type);
		r.write_no_lang(vfile);
	} catch(const XSLException& e) {
		r._throw(&method_name, e);
	}
}

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	std::istrstream stream(xml.cstr());
	const XalanParsedSource* parsedSource;
	int error=vdoc.transformer().parseSource(&stream, parsedSource);

	if(error)
		PTHROW(0, 0,
			&method_name,
			vdoc.transformer().getLastError());

	// replace any previous parsed source
	vdoc.set_parsed_source(*parsedSource);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must not be code");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	const XalanParsedSource* parsedSource;
	int error=vdoc.transformer().parseSource(filespec, parsedSource);

	if(error)
		PTHROW(0, 0,
			&file_name,
			vdoc.transformer().getLastError());

	// replace any previous parsed source
	vdoc.set_parsed_source(*parsedSource);
}

static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
						   void *info) {
	XalanTransformer& transformer=*static_cast<XalanTransformer *>(info);
	const char *attribute_cstr=aattribute.cstr();
	const char *meaning_cstr=static_cast<Value *>(ameaning)->as_string().cstr();

	transformer.setStylesheetParam(
		XalanDOMString(attribute_cstr),  
		XalanDOMString(meaning_cstr));
}
static void _xslt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// params
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "transform parameters parameter must not be code");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash())
				params->for_each(add_xslt_param, &vdoc.transformer());
			else
				PTHROW(0, 0,
					&method_name,
					"transform parameters parameter must be hash");
	}

	// source
	const XalanParsedSource &parsed_source=vdoc.get_parsed_source(pool, &method_name);

	// stylesheet
	const String& stylesheet_file_name=params->as_string(0, "file name must not be code");
	const String& stylesheet_filespec=r.absolute(stylesheet_file_name);
	//_asm int 3;
	Stylesheet_connection& connection=XSLT_stylesheet_manager->get_connection(stylesheet_filespec);

	// target
	XalanDocument* target=vdoc.parser_liaison().createDocument();
	XSLTResultTarget domResultTarget(target);

	// transform
	int error=vdoc.transformer().transform(
		parsed_source, 
		&connection.stylesheet(true/*nocache*/), 
		domResultTarget);
	connection.close();
	if(error)
		PTHROW(0, 0,
			&stylesheet_file_name,
			vdoc.transformer().getLastError());

	// write out result
	VXdoc& result=*new(pool) VXdoc(pool);
	result.set_document(*target);
	r.write_no_lang(result);
}

static void _getElementById(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// elementId
	const char *elementId=params->as_string(0, "elementID must not be code").cstr(String::UL_AS_IS);

	if(XalanElement *element=
		vdoc.get_document(pool, &method_name).getElementById(XalanDOMString(elementId))) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, element);
		r.write_no_lang(result);
	}
}
/*
static void _getElementsByTagName(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// tagname
	const char *tagname=params->as_string(0, "tagname must not be code").cstr(String::UL_AS_IS);

	VHash& result=*new(pool) VHash(pool);
	if(const XalanNodeList *nodes=
		vdoc.get_document(pool, &method_name).getElementsByTagName(XalanDOMString(tagname))) {
		for(int i=0; i<nodes->getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VXnode(pool, nodes->item(i)));
		}
	}

	// write out result
	r.write_no_lang(result);
}

static void _getElementsByTagNameNS(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// namespaceURI;localName
	const char *namespaceURI=params->as_string(0, "namespaceURI must not be code").cstr(String::UL_AS_IS);
	const char *localName=params->as_string(0, "localName must not be code").cstr(String::UL_AS_IS);

	VHash& result=*new(pool) VHash(pool);
	if(const XalanNodeList *nodes=
		vdoc.get_document(pool, &method_name).getElementsByTagNameNS(
			XalanDOMString(namespaceURI), XalanDOMString(localName))) {
		for(int i=0; i<nodes->getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VXnode(pool, nodes->item(i)));
		}
	}

	// write out result
	r.write_no_lang(result);
}
*/
// constructor

MXdoc::MXdoc(Pool& apool) : MXnode(apool) {
	set_name(*NEW String(pool(), XDOC_CLASS_NAME));

	// ^xdoc.save[some.xml]
	// ^xdoc.save[some.xml;options hash]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^xdoc.string[] <doc/>
	// ^xdoc.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 0, 1);

	// ^xdoc.file[] file with "<doc/>"
	// ^xdoc.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 0, 1);

	// ^xdoc::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);

	// ^xdoc::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^xdoc.xslt[stylesheet file_name]
	// ^xdoc.xslt[stylesheet file_name;params hash]
	add_native_method("xslt", Method::CT_DYNAMIC, _xslt, 1, 2);

	// ^xdoc.getElementById[elementId]
	add_native_method("getElementById", Method::CT_DYNAMIC, _getElementById, 1, 1);
/*	
	// ^xdoc.getElementsByTagName[tagname]
	add_native_method("getElementsByTagName", Method::CT_DYNAMIC, _getElementsByTagName, 1, 1);

	// ^xdoc.getElementsByTagNameNS[namespaceURI;localName] = array of nodes
	add_native_method("getElementsByTagNameNS", Method::CT_DYNAMIC, _getElementsByTagNameNS, 2, 2);
*/
}



#include <util/XercesDefs.hpp>
#include <util/XML256TableTranscoder.hpp>
//
//  This class provides an implementation of the XMLTranscoder interface
//  for the Windows variant of Latin1, called Windows-1251. Its close to
//  Latin1, but is somewhat different.
//
class XMLWin1251Transcoder : public XML256TableTranscoder
{
public :
    // -----------------------------------------------------------------------
    //  Public constructors and destructor
    // -----------------------------------------------------------------------
    XMLWin1251Transcoder
    (
        const   XMLCh* const    encodingName
        , const unsigned int    blockSize
    );

    virtual ~XMLWin1251Transcoder();


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    XMLWin1251Transcoder();
    XMLWin1251Transcoder(const XMLWin1251Transcoder&);
    void operator=(const XMLWin1251Transcoder&);
};

// from ICU: ibm-1251.ucm by PAF@design.ru
// ---------------------------------------------------------------------------
//  Local const data
//
//  gFromTable
//      This is the translation table for Windows 1251 to Unicode. This
//      table contains 255 entries. The entry for 1251 byte x is the
//      Unicode translation of that byte.
//
//  gToTable
//  gToTableSz
//      This is the translation table for Unicode to Windows 1251. This one
//      contains a list of records, sorted by the Unicode code point. We do
//      a binary search to find the Unicode point, and that record's other
//      field is the 1251 code point to translate to.
// ---------------------------------------------------------------------------
static const XMLCh gFromTable[256] =
{
0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 
0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 
0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 
0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F, 
0x0020, 0xFF01, 0xFF02, 0xFF03, 0xFF04, 0xFF05, 0xFF06, 0xFF07, 
0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F, 
0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17, 
0xFF18, 0xFF19, 0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E, 0xFF1F, 
0xFF20, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 
0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 
0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 
0xFF38, 0xFF39, 0xFF3A, 0xFF3B, 0xFF3C, 0xFF3D, 0xFF3E, 0xFF3F, 
0xFF40, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 
0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 
0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 
0xFF58, 0xFF59, 0xFF5A, 0xFF5B, 0xFF5C, 0xFF5D, 0xFF5E, 0x007F, 
0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 
0x0088, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F, 
0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 
0x0098, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F, 
0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 
0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407, 
0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 
0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457, 
0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 
0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 
0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 
0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F, 
0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 
0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F, 
0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 
0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

static const XMLTransService::TransRec gToTable[] =
{
{ 0x0000, 0x00 }, { 0x0001, 0x01 }, { 0x0002, 0x02 }, { 0x0003, 0x03 }, 
{ 0x0004, 0x04 }, { 0x0005, 0x05 }, { 0x0006, 0x06 }, { 0x0007, 0x07 }, 
{ 0x0008, 0x08 }, { 0x0009, 0x09 }, { 0x000A, 0x0A }, { 0x000B, 0x0B }, 
{ 0x000C, 0x0C }, { 0x000D, 0x0D }, { 0x000E, 0x0E }, { 0x000F, 0x0F }, 
{ 0x0010, 0x10 }, { 0x0011, 0x11 }, { 0x0012, 0x12 }, { 0x0013, 0x13 }, 
{ 0x0014, 0x14 }, { 0x0015, 0x15 }, { 0x0016, 0x16 }, { 0x0017, 0x17 }, 
{ 0x0018, 0x18 }, { 0x0019, 0x19 }, { 0x001A, 0x1A }, { 0x001B, 0x1B }, 
{ 0x001C, 0x1C }, { 0x001D, 0x1D }, { 0x001E, 0x1E }, { 0x001F, 0x1F }, 
{ 0x0020, 0x20 }, { 0x0021, 0x21 }, { 0x0022, 0x22 }, { 0x0023, 0x23 }, 
{ 0x0024, 0x24 }, { 0x0025, 0x25 }, { 0x0026, 0x26 }, { 0x0027, 0x27 }, 
{ 0x0028, 0x28 }, { 0x0029, 0x29 }, { 0x002A, 0x2A }, { 0x002B, 0x2B }, 
{ 0x002C, 0x2C }, { 0x002D, 0x2D }, { 0x002E, 0x2E }, { 0x002F, 0x2F }, 
{ 0x0030, 0x30 }, { 0x0031, 0x31 }, { 0x0032, 0x32 }, { 0x0033, 0x33 }, 
{ 0x0034, 0x34 }, { 0x0035, 0x35 }, { 0x0036, 0x36 }, { 0x0037, 0x37 }, 
{ 0x0038, 0x38 }, { 0x0039, 0x39 }, { 0x003A, 0x3A }, { 0x003B, 0x3B }, 
{ 0x003C, 0x3C }, { 0x003D, 0x3D }, { 0x003E, 0x3E }, { 0x003F, 0x3F }, 
{ 0x0040, 0x40 }, { 0x0041, 0x41 }, { 0x0042, 0x42 }, { 0x0043, 0x43 }, 
{ 0x0044, 0x44 }, { 0x0045, 0x45 }, { 0x0046, 0x46 }, { 0x0047, 0x47 }, 
{ 0x0048, 0x48 }, { 0x0049, 0x49 }, { 0x004A, 0x4A }, { 0x004B, 0x4B }, 
{ 0x004C, 0x4C }, { 0x004D, 0x4D }, { 0x004E, 0x4E }, { 0x004F, 0x4F }, 
{ 0x0050, 0x50 }, { 0x0051, 0x51 }, { 0x0052, 0x52 }, { 0x0053, 0x53 }, 
{ 0x0054, 0x54 }, { 0x0055, 0x55 }, { 0x0056, 0x56 }, { 0x0057, 0x57 }, 
{ 0x0058, 0x58 }, { 0x0059, 0x59 }, { 0x005A, 0x5A }, { 0x005B, 0x5B }, 
{ 0x005C, 0x5C }, { 0x005D, 0x5D }, { 0x005E, 0x5E }, { 0x005F, 0x5F }, 
{ 0x0060, 0x60 }, { 0x0061, 0x61 }, { 0x0062, 0x62 }, { 0x0063, 0x63 }, 
{ 0x0064, 0x64 }, { 0x0065, 0x65 }, { 0x0066, 0x66 }, { 0x0067, 0x67 }, 
{ 0x0068, 0x68 }, { 0x0069, 0x69 }, { 0x006A, 0x6A }, { 0x006B, 0x6B }, 
{ 0x006C, 0x6C }, { 0x006D, 0x6D }, { 0x006E, 0x6E }, { 0x006F, 0x6F }, 
{ 0x0070, 0x70 }, { 0x0071, 0x71 }, { 0x0072, 0x72 }, { 0x0073, 0x73 }, 
{ 0x0074, 0x74 }, { 0x0075, 0x75 }, { 0x0076, 0x76 }, { 0x0077, 0x77 }, 
{ 0x0078, 0x78 }, { 0x0079, 0x79 }, { 0x007A, 0x7A }, { 0x007B, 0x7B }, 
{ 0x007C, 0x7C }, { 0x007D, 0x7D }, { 0x007E, 0x7E }, { 0x007F, 0x7F }, 
{ 0x0088, 0x88 }, { 0x0098, 0x98 }, { 0x00A0, 0xA0 }, { 0x00A4, 0xA4 }, 
{ 0x00A6, 0xA6 }, { 0x00A7, 0xA7 }, { 0x00A9, 0xA9 }, { 0x00AB, 0xAB }, 
{ 0x00AC, 0xAC }, { 0x00AD, 0xAD }, { 0x00AE, 0xAE }, { 0x00B0, 0xB0 }, 
{ 0x00B1, 0xB1 }, { 0x00B5, 0xB5 }, { 0x00B6, 0xB6 }, { 0x00B7, 0xB7 }, 
{ 0x00BB, 0xBB }, { 0x0401, 0xA8 }, { 0x0402, 0x80 }, { 0x0403, 0x81 }, 
{ 0x0404, 0xAA }, { 0x0405, 0xBD }, { 0x0406, 0xB2 }, { 0x0407, 0xAF }, 
{ 0x0408, 0xA3 }, { 0x0409, 0x8A }, { 0x040A, 0x8C }, { 0x040B, 0x8E }, 
{ 0x040C, 0x8D }, { 0x040E, 0xA1 }, { 0x040F, 0x8F }, { 0x0410, 0xC0 }, 
{ 0x0411, 0xC1 }, { 0x0412, 0xC2 }, { 0x0413, 0xC3 }, { 0x0414, 0xC4 }, 
{ 0x0415, 0xC5 }, { 0x0416, 0xC6 }, { 0x0417, 0xC7 }, { 0x0418, 0xC8 }, 
{ 0x0419, 0xC9 }, { 0x041A, 0xCA }, { 0x041B, 0xCB }, { 0x041C, 0xCC }, 
{ 0x041D, 0xCD }, { 0x041E, 0xCE }, { 0x041F, 0xCF }, { 0x0420, 0xD0 }, 
{ 0x0421, 0xD1 }, { 0x0422, 0xD2 }, { 0x0423, 0xD3 }, { 0x0424, 0xD4 }, 
{ 0x0425, 0xD5 }, { 0x0426, 0xD6 }, { 0x0427, 0xD7 }, { 0x0428, 0xD8 }, 
{ 0x0429, 0xD9 }, { 0x042A, 0xDA }, { 0x042B, 0xDB }, { 0x042C, 0xDC }, 
{ 0x042D, 0xDD }, { 0x042E, 0xDE }, { 0x042F, 0xDF }, { 0x0430, 0xE0 }, 
{ 0x0431, 0xE1 }, { 0x0432, 0xE2 }, { 0x0433, 0xE3 }, { 0x0434, 0xE4 }, 
{ 0x0435, 0xE5 }, { 0x0436, 0xE6 }, { 0x0437, 0xE7 }, { 0x0438, 0xE8 }, 
{ 0x0439, 0xE9 }, { 0x043A, 0xEA }, { 0x043B, 0xEB }, { 0x043C, 0xEC }, 
{ 0x043D, 0xED }, { 0x043E, 0xEE }, { 0x043F, 0xEF }, { 0x0440, 0xF0 }, 
{ 0x0441, 0xF1 }, { 0x0442, 0xF2 }, { 0x0443, 0xF3 }, { 0x0444, 0xF4 }, 
{ 0x0445, 0xF5 }, { 0x0446, 0xF6 }, { 0x0447, 0xF7 }, { 0x0448, 0xF8 }, 
{ 0x0449, 0xF9 }, { 0x044A, 0xFA }, { 0x044B, 0xFB }, { 0x044C, 0xFC }, 
{ 0x044D, 0xFD }, { 0x044E, 0xFE }, { 0x044F, 0xFF }, { 0x0451, 0xB8 }, 
{ 0x0452, 0x90 }, { 0x0453, 0x83 }, { 0x0454, 0xBA }, { 0x0455, 0xBE }, 
{ 0x0456, 0xB3 }, { 0x0457, 0xBF }, { 0x0458, 0xBC }, { 0x0459, 0x9A }, 
{ 0x045A, 0x9C }, { 0x045B, 0x9E }, { 0x045C, 0x9D }, { 0x045E, 0xA2 }, 
{ 0x045F, 0x9F }, { 0x0490, 0xA5 }, { 0x0491, 0xB4 }, { 0x2013, 0x96 }, 
{ 0x2014, 0x97 }, { 0x2018, 0x91 }, { 0x2019, 0x92 }, { 0x201A, 0x82 }, 
{ 0x201C, 0x93 }, { 0x201D, 0x94 }, { 0x201E, 0x84 }, { 0x2020, 0x86 }, 
{ 0x2021, 0x87 }, { 0x2022, 0x95 }, { 0x2026, 0x85 }, { 0x2030, 0x89 }, 
{ 0x2039, 0x8B }, { 0x203A, 0x9B }, { 0x2116, 0xB9 }, { 0x2122, 0x99 }, 
{ 0xFF01, 0x21 }, { 0xFF02, 0x22 }, { 0xFF03, 0x23 }, { 0xFF04, 0x24 }, 
{ 0xFF05, 0x25 }, { 0xFF06, 0x26 }, { 0xFF07, 0x27 }, { 0xFF08, 0x28 }, 
{ 0xFF09, 0x29 }, { 0xFF0A, 0x2A }, { 0xFF0B, 0x2B }, { 0xFF0C, 0x2C }, 
{ 0xFF0D, 0x2D }, { 0xFF0E, 0x2E }, { 0xFF0F, 0x2F }, { 0xFF10, 0x30 }, 
{ 0xFF11, 0x31 }, { 0xFF12, 0x32 }, { 0xFF13, 0x33 }, { 0xFF14, 0x34 }, 
{ 0xFF15, 0x35 }, { 0xFF16, 0x36 }, { 0xFF17, 0x37 }, { 0xFF18, 0x38 }, 
{ 0xFF19, 0x39 }, { 0xFF1A, 0x3A }, { 0xFF1B, 0x3B }, { 0xFF1C, 0x3C }, 
{ 0xFF1D, 0x3D }, { 0xFF1E, 0x3E }, { 0xFF1F, 0x3F }, { 0xFF20, 0x40 }, 
{ 0xFF21, 0x41 }, { 0xFF22, 0x42 }, { 0xFF23, 0x43 }, { 0xFF24, 0x44 }, 
{ 0xFF25, 0x45 }, { 0xFF26, 0x46 }, { 0xFF27, 0x47 }, { 0xFF28, 0x48 }, 
{ 0xFF29, 0x49 }, { 0xFF2A, 0x4A }, { 0xFF2B, 0x4B }, { 0xFF2C, 0x4C }, 
{ 0xFF2D, 0x4D }, { 0xFF2E, 0x4E }, { 0xFF2F, 0x4F }, { 0xFF30, 0x50 }, 
{ 0xFF31, 0x51 }, { 0xFF32, 0x52 }, { 0xFF33, 0x53 }, { 0xFF34, 0x54 }, 
{ 0xFF35, 0x55 }, { 0xFF36, 0x56 }, { 0xFF37, 0x57 }, { 0xFF38, 0x58 }, 
{ 0xFF39, 0x59 }, { 0xFF3A, 0x5A }, { 0xFF3B, 0x5B }, { 0xFF3C, 0x5C }, 
{ 0xFF3D, 0x5D }, { 0xFF3E, 0x5E }, { 0xFF3F, 0x5F }, { 0xFF40, 0x60 }, 
{ 0xFF41, 0x61 }, { 0xFF42, 0x62 }, { 0xFF43, 0x63 }, { 0xFF44, 0x64 }, 
{ 0xFF45, 0x65 }, { 0xFF46, 0x66 }, { 0xFF47, 0x67 }, { 0xFF48, 0x68 }, 
{ 0xFF49, 0x69 }, { 0xFF4A, 0x6A }, { 0xFF4B, 0x6B }, { 0xFF4C, 0x6C }, 
{ 0xFF4D, 0x6D }, { 0xFF4E, 0x6E }, { 0xFF4F, 0x6F }, { 0xFF50, 0x70 }, 
{ 0xFF51, 0x71 }, { 0xFF52, 0x72 }, { 0xFF53, 0x73 }, { 0xFF54, 0x74 }, 
{ 0xFF55, 0x75 }, { 0xFF56, 0x76 }, { 0xFF57, 0x77 }, { 0xFF58, 0x78 }, 
{ 0xFF59, 0x79 }, { 0xFF5A, 0x7A }, { 0xFF5B, 0x7B }, { 0xFF5C, 0x7C }, 
{ 0xFF5D, 0x7D }, { 0xFF5E, 0x7E }
};
static const unsigned int gToTableSz = 350;



// ---------------------------------------------------------------------------
//  XML1140Transcoder: Constructors and Destructor
// ---------------------------------------------------------------------------
XMLWin1251Transcoder::XMLWin1251Transcoder( const   XMLCh* const encodingName
                                            , const unsigned int blockSize) :
    XML256TableTranscoder
    (
        encodingName
        , blockSize
        , gFromTable
        , gToTable
        , gToTableSz
    )
{
}


XMLWin1251Transcoder::~XMLWin1251Transcoder()
{
}



void MXdoc::configure_admin(Request& r) {
	XalanDOMString *sencoding=new XalanDOMString("WINDOWS-125ODIN");
	const XMLCh* const encoding_cstr=sencoding->c_str();
    XMLPlatformUtils::fgTransService->addEncoding(
		encoding_cstr, 
		new ENameMapFor<XMLWin1251Transcoder>(encoding_cstr));
	// delete sencoding; somehow
}

// global variable

Methoded *Xdoc_class;

// creator

#endif

Methoded *MXdoc_create(Pool& pool) {
	return 
#ifdef XML
		Xdoc_class=new(pool) MXdoc(pool);
#else
		0
#endif
	;
}
