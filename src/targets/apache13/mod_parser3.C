/** @file
	Parser: apache 1.3 module.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: mod_parser3.C,v 1.19.2.1 2002/06/04 09:21:26 paf Exp $
*/

#include "pa_config_includes.h"

#if _MSC_VER
#	include <new.h>
#endif

#include "pa_globals.h"

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"

#include "pa_common.h"
#include "pa_sapi.h"
#include "classes.h"
#include "pa_request.h"
#include "pa_version.h"
#include "pa_socks.h"


// consts

extern const char *main_RCSIds[];
#ifdef USE_SMTP
extern const char *smtp_RCSIds[];
#endif
extern const char *gd_RCSIds[];
extern const char *classes_RCSIds[];
extern const char *types_RCSIds[];
extern const char *ApacheModuleParser3_RCSIds[];
const char **RCSIds[]={
	main_RCSIds,
#ifdef USE_SMTP
	smtp_RCSIds,
#endif
	gd_RCSIds,
	classes_RCSIds,
	types_RCSIds,
	ApacheModuleParser3_RCSIds,
	0
};


/// apache parser module configuration [httpd.conf + .htaccess-es]
struct Parser_module_config {
    const char* parser_root_config_filespec; ///< filespec of admin's config file
    const char* parser_site_config_filespec; ///< filespec of site's config file
	bool parser_status_allowed;
};

/*
 * Declare ourselves so the configuration routines can find and know us.
 * We'll fill it in at the end of the module.
 */
extern "C" module MODULE_VAR_EXPORT parser3_module;

/*
 * Locate our directory configuration record for the current request.
 */
static Parser_module_config *our_dconfig(request_rec *r) {
    return (Parser_module_config *) 
		ap_get_module_config(r->per_dir_config, &parser3_module);
}

static const char *cmd_parser_config(cmd_parms *cmd, void *mconfig, char *file_spec) {
    Parser_module_config *cfg = (Parser_module_config *) mconfig;

	// remember assigned filespec into cfg
	(cmd->info?cfg->parser_root_config_filespec:cfg->parser_site_config_filespec)=file_spec;

    return NULL;
}
static const char *cmd_parser_status_allowed(cmd_parms *cmd, void *mconfig, char *file_spec) {
	//_asm int 3;
    Parser_module_config *cfg = (Parser_module_config *) mconfig;

	cfg->parser_status_allowed=true;

    return NULL;
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Now we declare our content handlers, which are invoked when the server   */
/* encounters a document which our module is supposed to have a chance to   */
/* see.  (See mod_mime's SetHandler and AddHandler directives, and the      */
/* mod_info and mod_status examples, for more details.)                     */
/*                                                                          */
/* Since content handlers are dumping data directly into the connexion      */
/* (using the r*() routines, such as rputs() and rprintf()) without         */
/* intervention by other parts of the server, they need to make             */
/* sure any accumulated HTTP headers are sent first.  This is done by       */
/* calling send_http_header().  Otherwise, no header will be sent at all,   */
/* and the output sent to the client will actually be HTTP-uncompliant.     */
/*--------------------------------------------------------------------------*/
/* 
 * Sample content handler.  All this does is display the call list that has
 * been built up so far.
 *
 * The return value instructs the caller concerning what happened and what to
 * do next:
 *  OK ("we did our thing")
 *  DECLINED ("this isn't something with which we want to get involved")
 *  HTTP_mumble ("an error status should be reported")
 */


//@{
/// SAPI func decl
void SAPI::log(Pool& pool, const char *fmt, ...) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());

    va_list args;
    va_start(args,fmt);
	char buf[MAX_STRING];
	size_t size=vsnprintf(buf, MAX_STRING, fmt, args);
	remove_crlf(buf, buf+size);
	ap_log_rerror(0, 0, APLOG_ERR | APLOG_NOERRNO, r, "%s", buf);
    va_end(args);
}

void SAPI::die(const char *fmt, ...) {
    va_list args;
    va_start(args,fmt);
	char buf[MAX_STRING];
	size_t size=vsnprintf(buf, MAX_STRING, fmt, args);
	remove_crlf(buf, buf+size);
	ap_log_error(APLOG_MARK, APLOG_EMERG, 0, "%s", buf);
    va_end(args);

	exit(1);
}

const char *SAPI::get_env(Pool& pool, const char *name) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());
 	return (const char *)ap_table_get(r->subprocess_env, name);
}

size_t SAPI::read_post(Pool& pool, char *buf, size_t max_bytes) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());

/*    ap_log_error(APLOG_MARK, APLOG_DEBUG, r->server, 
		"mod_parser3: SAPI::read_post(max=%u)", max_bytes);
*/
	int retval;
    if((retval = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR)))
		return 0;
	if(!ap_should_client_block(r))
		return 0;

	uint total_read_bytes=0;
	void (*handler)(int)=signal(SIGPIPE, SIG_IGN);
	while (total_read_bytes<max_bytes) {
		ap_hard_timeout("Read POST information", r); /* start timeout timer */
		uint read_bytes=
			ap_get_client_block(r, buf+total_read_bytes, max_bytes-total_read_bytes);
		ap_reset_timeout(r);
		if (read_bytes<=0)
			break;
		total_read_bytes+=read_bytes;
	}
	signal(SIGPIPE, handler);	
	return total_read_bytes;
}

/// @test location provide with protocol. think about internal redirects
void SAPI::add_header_attribute(Pool& pool, const char *key, const char *value) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());

	if(strcasecmp(key, "location")==0) 
		r->status=302;

	if(strcasecmp(key, "content-type")==0) {
		/* r->content_type, *not* r->headers_out("Content-type").  If you don't
		 * set it, it will be filled in with the server's default type (typically
		 * "text/plain").  You *must* also ensure that r->content_type is lower
		 * case.
		 */
		r->content_type = value;
	} else if(strcasecmp(key, "status")==0) 
		r->status=atoi(value);
	else
		ap_table_addn(r->headers_out, key, value);
}

void SAPI::send_header(Pool& pool) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());

    ap_hard_timeout("Send header", r);
    ap_send_http_header(r);
	ap_kill_timeout(r);
}

void SAPI::send_body(Pool& pool, const void *buf, size_t size) {
	request_rec *r=static_cast<request_rec *>(pool.get_context());

    ap_hard_timeout("Send body", r);
	ap_rwrite(buf, size, r);
	ap_kill_timeout(r);
}

//@}

/**
	main workhorse
	
	@todo intelligent cache-control
*/
static void real_parser_handler(Pool& pool, request_rec *r) {
	ap_add_common_vars(r);
	ap_add_cgi_vars(r);

	// Request info
	Request::Info request_info;
	request_info.document_root=SAPI::get_env(pool, "DOCUMENT_ROOT");
	request_info.path_translated=r->filename;
	request_info.method=r->method;
	request_info.query_string=r->args;
	request_info.uri=SAPI::get_env(pool, "REQUEST_URI");
	request_info.content_type=SAPI::get_env(pool, "CONTENT_TYPE");
	const char *content_length=SAPI::get_env(pool, "CONTENT_LENGTH");
	request_info.content_length=content_length?atoi(content_length):0;
	request_info.cookie=SAPI::get_env(pool, "HTTP_COOKIE");
	request_info.user_agent=SAPI::get_env(pool, "HTTP_USER_AGENT");

    // config
	Parser_module_config *dcfg=our_dconfig(r);

	//_asm int 3;
	// prepare to process request
	Request request(pool,
		request_info,
		String::UL_HTML|String::UL_OPTIMIZE_BIT,
		dcfg->parser_status_allowed
		);
	
	// process the request
	request.core(
		dcfg->parser_root_config_filespec, true, // /path/to/admin/config
		dcfg->parser_site_config_filespec, true, // /path/to/site/config
		r->header_only!=0);
}

void call_real_parser_handler__do_SEH(Pool& pool, request_rec *r) {
#if _MSC_VER & !defined(_DEBUG)
	LPEXCEPTION_POINTERS system_exception=0;
	__try {
#endif
		real_parser_handler(pool, r);
		
#if _MSC_VER & !defined(_DEBUG)
	} __except (
		(system_exception=GetExceptionInformation()), 
		EXCEPTION_EXECUTE_HANDLER) {
		
		if(system_exception)
			if(_EXCEPTION_RECORD *er=system_exception->ExceptionRecord)
				throw Exception(0,
					0,
					"Exception 0x%08X at 0x%08X", er->ExceptionCode,  er->ExceptionAddress);
			else
				throw Exception(0, 0, "Exception <no exception record>");
			else
				throw Exception(0, 0, "Exception <no exception information>");
	}
#endif
}

/// @test r->finfo.st_mode check seems to work only on win32
static int parser_handler(request_rec *r) {
	//_asm int 3;
    if(r->finfo.st_mode == 0) 
		return NOT_FOUND;
	
	Pool pool(r->pool);
	pool.set_context(r);

	/* A flag which modules can set, to indicate that the data being
	 * returned is volatile, and clients should be told not to cache it.
	 */
//	r->no_cache=1;

	try { // global try
		call_real_parser_handler__do_SEH(pool, r);
		// successful finish
	} catch(const Exception& e) { // global problem 
		// don't allocate anything on pool here:
		//   possible pool' exception not catch-ed now
		//   and there could be out-of-memory exception
		const char *body=e.comment();
		// log it
		SAPI::log(pool, "exception in request exception handler: %s", body);

		//
		int content_length=strlen(body);

		// prepare header
		SAPI::add_header_attribute(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%u", content_length);
		SAPI::add_header_attribute(pool, "content-length", content_length_cstr);

		// send header
		SAPI::send_header(pool);

		// send body
		if(!r->header_only)
			SAPI::send_body(pool, body, content_length);

		// unsuccessful finish
	}

    /*
     * We did what we wanted to do, so tell the rest of the server we
     * succeeded.
     */
    return OK;
}

#if _MSC_VER
int failed_new(size_t size) {
	SAPI::die("out of memory in 'new', failed to allocated %u bytes", size);
	return 0; // not reached
}
#endif

#ifdef HAVE_SET_NEW_HANDLER
void failed_new() {
    SAPI::die("out of memory in 'new'");
}
#endif

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Now let's declare routines for each of the callback phase in order.      */
/* (That's the order in which they're listed in the callback list, *not     */
/* the order in which the server calls them!  See the command_rec           */
/* declaration near the bottom of this file.)  Note that these may be       */
/* called for situations that don't relate primarily to our function - in   */
/* other words, the fixup handler shouldn't assume that the request has     */
/* to do with "example" stuff.                                              */
/*                                                                          */
/* With the exception of the content handler, all of our routines will be   */
/* called for each request, unless an earlier handler from another module   */
/* aborted the sequence.                                                    */
/*                                                                          */
/* Handlers that are declared as "int" can return the following:            */
/*                                                                          */
/*  OK          Handler accepted the request and did its thing with it.     */
/*  DECLINED    Handler took no action.                                     */
/*  HTTP_mumble Handler looked at request and found it wanting.             */
/*                                                                          */
/* What the server does after calling a module handler depends upon the     */
/* handler's return value.  In all cases, if the handler returns            */
/* DECLINED, the server will continue to the next module with an handler    */
/* for the current phase.  However, if the handler return a non-OK,         */
/* non-DECLINED status, the server aborts the request right there.  If      */
/* the handler returns OK, the server's next action is phase-specific;      */
/* see the individual handler comments below for details.                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* 
 * This function is called during server initialisation.  Any information
 * that needs to be recorded must be in static cells, since there's no
 * configuration record.
 *
 * There is no return value.
 */

static void setup_module_cells() {
	static bool  globals_inited=false;
	if(globals_inited)
		return;
	globals_inited=true;

#ifdef WIN32
	_set_new_handler(failed_new);
#endif

#ifdef HAVE_SET_NEW_HANDLER
	std::set_new_handler(failed_new);
#endif

	/*
     * allocate our module-private pool.
     */
	static Pool pool(ap_make_sub_pool(NULL)); // global pool
	/// no trying to __try here [yet]
	try {
		// init socks
		init_socks(pool);

		// init global classes
		init_methoded_array(pool);
		// init global variables
		pa_globals_init(pool);
	} catch(const Exception& e) { // global problem 
		SAPI::die("setup_module_cells failed: %s", e.comment());
	}
}

static void parser_server_init(server_rec *s, pool *p) {
#if MODULE_MAGIC_NUMBER >= 19980527
	ap_add_version_component("Parser/"PARSER_VERSION);
#endif	

    /*
     * Set up any module cells that ought to be initialised.
     */
    setup_module_cells();
}

/*
 * This function gets called to create a per-directory configuration
 * record.  This will be called for the "default" server environment, and for
 * each directory for which the parser finds any of our directives applicable.
 * If a directory doesn't have any of our directives involved (i.e., they
 * aren't in the .htaccess file, or a <Location>, <Directory>, or related
 * block), this routine will *not* be called - the configuration for the
 * closest ancestor is used.
 *
 * The return value is a pointer to the created module-specific
 * structure.
 */
static void *parser_create_dir_config(pool *p, char *dirspec) {
	//_asm int 3;
	/*
     * Allocate the space for our record from the pool supplied.
     */
    Parser_module_config *cfg=
		(Parser_module_config *) ap_pcalloc(p, sizeof(Parser_module_config));
    /*
     * Now fill in the defaults.  If there are any `parent' configuration
     * records, they'll get merged as part of a separate callback.
     */

    return (void *) cfg;
}

/*
 * This function gets called to merge two per-directory configuration
 * records.  This is typically done to cope with things like .htaccess files
 * or <Location> directives for directories that are beneath one for which a
 * configuration record was already created.  The routine has the
 * responsibility of creating a new record and merging the contents of the
 * other two into it appropriately.  If the module doesn't declare a merge
 * routine, the record for the closest ancestor location (that has one) is
 * used exclusively.
 *
 * The routine MUST NOT modify any of its arguments!
 *
 * The return value is a pointer to the created module-specific structure
 * containing the merged values.

   20011126 paf: noticed, that this is called even on virtual root merge with something "parent",
   while thought that that is part of merge_server...

 */
static void *parser_merge_dir_config(pool *p, void *parent_conf,
                                      void *newloc_conf) {
	//_asm int 3;
    Parser_module_config *merged_config = 
		(Parser_module_config *) ap_pcalloc(p, sizeof(Parser_module_config));
    Parser_module_config *pconf = (Parser_module_config *) parent_conf;
    Parser_module_config *nconf = (Parser_module_config *) newloc_conf;

    merged_config->parser_root_config_filespec = ap_pstrdup(p, nconf->parser_root_config_filespec?
		nconf->parser_root_config_filespec:pconf->parser_root_config_filespec);
    merged_config->parser_site_config_filespec = ap_pstrdup(p, nconf->parser_site_config_filespec?
		nconf->parser_site_config_filespec:pconf->parser_site_config_filespec);
	merged_config->parser_status_allowed=
		pconf->parser_status_allowed ||
		nconf->parser_status_allowed;

	/*
     * Some things get copied directly from the more-specific record, rather
     * than getting merged.
     */

    return (void *) merged_config;
}

/*
 * This function gets called to create a per-server configuration
 * record.  It will always be called for the "default" server.
 *
 * The return value is a pointer to the created module-specific
 * structure.
 */
static void *parser_create_server_config(pool *p, server_rec *s) {
	//_asm int 3;
    /*
     * As with the parser_create_dir_config() routine, we allocate and fill
     * in an empty record.
     */
    Parser_module_config *cfg=
		(Parser_module_config *) ap_pcalloc(p, sizeof(Parser_module_config));

    return (void *) cfg;
}

/*
 * This function gets called to merge two per-server configuration
 * records.  This is typically done to cope with things like virtual hosts and
 * the default server configuration  The routine has the responsibility of
 * creating a new record and merging the contents of the other two into it
 * appropriately.  If the module doesn't declare a merge routine, the more
 * specific existing record is used exclusively.
 *
 * The routine MUST NOT modify any of its arguments!
 *
 * The return value is a pointer to the created module-specific structure
 * containing the merged values.
 */
static void *parser_merge_server_config(pool *p, void *server1_conf,
                                         void *server2_conf)
{
	//_asm int 3;

    Parser_module_config *merged_config = 
		(Parser_module_config *) ap_pcalloc(p, sizeof(Parser_module_config));
    Parser_module_config *s1conf = (Parser_module_config *) server1_conf;
    Parser_module_config *s2conf = (Parser_module_config *) server2_conf;

    /*
     * Our inheritance rules are our own, and part of our module's semantics.
     * Basically, just note whence we came.
     */
    merged_config->parser_root_config_filespec = ap_pstrdup(p, s2conf->parser_root_config_filespec?
		s2conf->parser_root_config_filespec:s1conf->parser_root_config_filespec);
    merged_config->parser_site_config_filespec = ap_pstrdup(p, s2conf->parser_site_config_filespec?
		s2conf->parser_site_config_filespec:s1conf->parser_site_config_filespec);
	merged_config->parser_status_allowed=
		s1conf->parser_status_allowed || 
		s2conf->parser_status_allowed;
 
	return (void *) merged_config;
}

/*
 * This routine gives our module an opportunity to translate the URI into an
 * actual filename.  If we don't do anything special, the server's default
 * rules (Alias directives and the like) will continue to be followed.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, no
 * further modules are called for this phase.
 */
static int parser_translate_handler(request_rec *r) {
    Parser_module_config *cfg=our_dconfig(r);
    return DECLINED;
}

/*
 * This routine is called to check the authentication information sent with
 * the request (such as looking up the user in a database and verifying that
 * the [encrypted] password sent matches the one in the database).
 *
 * The return value is OK, DECLINED, or some HTTP_mumble error (typically
 * HTTP_UNAUTHORIZED).  If we return OK, no other modules are given a chance
 * at the request during this phase.
 */
static int parser_check_user_id(request_rec *r) {
    Parser_module_config *cfg=our_dconfig(r);
    return DECLINED;
}

/*
 * This routine is called to check to see if the resource being requested
 * requires authorisation.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, no
 * other modules are called during this phase.
 *
 * If *all* modules return DECLINED, the request is aborted with a server
 * error.
 */
static int parser_auth_checker(request_rec *r) {
    Parser_module_config *cfg=our_dconfig(r);
    return DECLINED;
}

/*
 * This routine is called to check for any module-specific restrictions placed
 * upon the requested resource.  (See the mod_access module for an example.)
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  All modules with an
 * handler for this phase are called regardless of whether their predecessors
 * return OK or DECLINED.  The first one to return any other status, however,
 * will abort the sequence (and the request) as usual.
 */
static int parser_access_checker(request_rec *r) {

    Parser_module_config *cfg=our_dconfig(r);
    return DECLINED;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* All of the routines have been declared now.  Here's the list of          */
/* directives specific to our module, and information about where they      */
/* may appear and how the command parser should pass them to us for         */
/* processing.  Note that care must be taken to ensure that there are NO    */
/* collisions of directive names between modules.                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* 
 * List of directives specific to our module.
 */
static const command_rec parser_cmds[] =
{
    {
        "ParserRootConfig",              /* directive name */
        (const char *(*)(void))((void *)cmd_parser_config), // config action routine
        (void*)true,                   /* argument to include in call */
        (int)(ACCESS_CONF|RSRC_CONF),             /* where available */
        TAKE1,                /* arguments */
        "Parser root config filespec (Admin)" // directive description
    },
    {
        "ParserSiteConfig",              /* directive name */
        (const char *(*)(void))((void *)cmd_parser_config), // config action routine
        (void*)false,                   /* argument to include in call */
        (int)(OR_OPTIONS),             /* where available */
        TAKE1,                /* arguments */
        "Parser site config filespec" // directive description
    },
	{
		"ParserStatusAllowed",              /* directive name */
        (const char *(*)(void))((void *)cmd_parser_status_allowed), // config action routine
        (void*)0,                   /* argument to include in call */
        (int)(ACCESS_CONF),             /* where available */
        NO_ARGS,                /* arguments */
        "Parser status class can be used" // directive description
	},
    {NULL}
};

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Now the list of content handlers available from this module.             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* 
 * List of content handlers our module supplies.  Each handler is defined by
 * two parts: a name by which it can be referenced (such as by
 * {Add,Set}Handler), and the actual routine name.  The list is terminated by
 * a NULL block, since it can be of variable length.
 *
 * Note that content-handlers are invoked on a most-specific to least-specific
 * basis; that is, a handler that is declared for "text/plain" will be
 * invoked before one that was declared for "text / *".  Note also that
 * if a content-handler returns anything except DECLINED, no other
 * content-handlers will be called.
 */
static const handler_rec parser_handlers[] =
{
    {"parser3-handler", parser_handler},
    {NULL}
};

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Finally, the list of callback routines and data structures that          */
/* provide the hooks into our module from the other parts of the server.    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* 
 * Module definition for configuration.  If a particular callback is not
 * needed, replace its routine name below with the word NULL.
 *
 * The number in brackets indicates the order in which the routine is called
 * during request processing.  Note that not all routines are necessarily
 * called (such as if a resource doesn't have access restrictions).
 */
module MODULE_VAR_EXPORT parser3_module =
{
    STANDARD_MODULE_STUFF,
    parser_server_init,          /* module initializer */
    parser_create_dir_config,    /* per-directory config creator */
    parser_merge_dir_config,     /* dir config merger */
    parser_create_server_config, /* server config creator */
    parser_merge_server_config,  /* server config merger */
    parser_cmds,                 /* command table */
    parser_handlers,             /* [9] list of handlers */
    parser_translate_handler,    /* [2] filename-to-URI translation */
    parser_check_user_id,        /* [5] check/validate user_id */
    parser_auth_checker,         /* [6] check user_id is valid *here* */
    parser_access_checker,       /* [4] check access by host address */
    0,                           /* [7] MIME type checker/setter */
    0,                           /* [8] fixups */
    0                            /* [10] logger */
};

#if defined(_MSC_VER)
#	define APACHE_WIN32_SRC "/parser3project/win32/apache13/src"
#	ifdef _DEBUG
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreD/ApacheCore.lib")
#	else
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreR/ApacheCore.lib")
#	endif
#endif
