/** @file
Parser: apache 1.3 module, part, compiled by Apache.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_MOD_PARSER3_C="$Date: 2003/11/20 16:34:27 $";

#ifdef WIN32
#include <winsock2.h>
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"
#include "ap_md5.h"
#include "ap_alloc.h"

#include "pa_httpd.h"

/*
* Declare ourselves so the configuration routines can find and know us.
* We'll fill it in at the end of the module.
*/
extern module MODULE_VAR_EXPORT parser3_module;

/*
* Locate our directory configuration record for the current request.
*/
static Parser_module_config *our_dconfig(request_rec *r) {
	return (Parser_module_config *) 
		ap_get_module_config(r->per_dir_config, &parser3_module);
}

static const char* cmd_parser_config(cmd_parms *cmd, void *mconfig, char *file_spec) {
	Parser_module_config *cfg = (Parser_module_config *) mconfig;
	
	// remember assigned filespec into cfg
	cfg->parser_config_filespec=file_spec;
	
	return NULL;
}
static const char* cmd_parser_status_allowed(cmd_parms *cmd, void *mconfig, char *file_spec) {
	//_asm int 3;
	Parser_module_config *cfg = (Parser_module_config *) mconfig;
	
	cfg->parser_status_allowed=1;
	
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

static int parser_handler(request_rec *ar) {
	// record clone
	pa_request_rec lr={
		ar,
		ar->pool,
		ar->header_only,
		&ar->status,
		ar->method,
		ar->headers_out,
		ar->subprocess_env,
		&ar->content_type,
		ar->uri,
		ar->filename,
		ar->path_info,
		ar->args,
		&ar->finfo
	};

	// config
	Parser_module_config *dcfg=our_dconfig(ar);
	
	return pa_parser_handler(&lr, dcfg);
}

/* 
* This function is called during server initialisation.  Any information
* that needs to be recorded must be in static cells, since there's no
* configuration record.
*
* There is no return value.
*/

static void parser_server_init(server_rec *s, pool *p) {
#if MODULE_MAGIC_NUMBER >= 19980527
	ap_add_version_component(pa_version());
#endif	
	
	/*
	* Set up any module cells that ought to be initialised.
	*/
	pa_setup_module_cells();
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
	
	merged_config->parser_config_filespec = ap_pstrdup(p, nconf->parser_config_filespec?
		nconf->parser_config_filespec:pconf->parser_config_filespec);
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
	merged_config->parser_config_filespec = ap_pstrdup(p, s2conf->parser_config_filespec?
		s2conf->parser_config_filespec:s1conf->parser_config_filespec);
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
		"ParserConfig",              /* directive name */
			(const char* (*)(void))((void *)cmd_parser_config), // config action routine
			(void*)0,                   /* argument to include in call */
			(int)(OR_OPTIONS),             /* where available */
			TAKE1,                /* arguments */
			"Parser config filespec" // directive description
	},
	{
		"ParserStatusAllowed",              /* directive name */
			(const char* (*)(void))((void *)cmd_parser_status_allowed), // config action routine
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
#	define APACHE_WIN32_SRC "../../../../win32/apache13/src"
#	ifdef _DEBUG
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreD/ApacheCore.lib")
#	else
#		pragma comment(lib, APACHE_WIN32_SRC "/CoreR/ApacheCore.lib")
#	endif
#endif


// interface to C++

#define	PA_APLOG_EMERG	0	/* system is unusable */
#define	PA_APLOG_ALERT	1	/* action must be taken immediately */
#define	PA_APLOG_CRIT	2	/* critical conditions */
#define	PA_APLOG_ERR	3	/* error conditions */
#define	PA_APLOG_WARNING	4	/* warning conditions */
#define	PA_APLOG_NOTICE	5	/* normal but significant condition */
#define	PA_APLOG_INFO	6	/* informational */
#define	PA_APLOG_DEBUG	7	/* debug-level messages */

#define	PA_APLOG_LEVELMASK	7	/* mask off the level value */

#define PA_APLOG_NOERRNO		(PA_APLOG_LEVELMASK + 1)

#define PA_APLOG_MARK	__FILE__,__LINE__

void pa_ap_log_rerror(const char *file, int line, int level,
		      const pa_request_rec *s, const char *fmt, ...) {
	const char* str;
	va_list l;
	va_start(l, fmt); 
	str=va_arg(l, const char*);
	va_end(l); 

	ap_log_rerror(file, line, level,
		      (request_rec*)s->real_request_rec, "%s", str);
}


void pa_ap_log_error(const char *file, int line, int level,
		     const pa_server_rec *s, const char *fmt, ...) {
	const char* str;
	va_list l;
	va_start(l, fmt); 
	str=va_arg(l, const char*);
	va_end(l); 

	ap_log_error(file, line, level,
		      (server_rec*)s, "%s", str);
}

// ap_alloc.h

const char* pa_ap_table_get(const pa_table *t, const char *name) {
	return ap_table_get((const table*)t, name);
}
void pa_ap_table_addn(pa_table *t, const char *name, const char *val) {
	ap_table_addn((table*)t, name, val);
}

int pa_ap_table_size(const pa_table *t) {
	return ap_table_elts((const table*)t)->nelts;
}

void pa_ap_table_do(int (*comp) (void *, const char *, const char *), 
		    void *rec, const pa_table *t, ...) {
	ap_table_do(comp, rec, (table*)t, 0);
}

char * pa_ap_pstrdup(pa_pool *p, const char *s) {
	return ap_pstrdup((pool*)p, s);
}

// http_protocol.h

int pa_ap_setup_client_block(pa_request_rec *r, int read_policy) {
	return ap_setup_client_block((request_rec*)r->real_request_rec,
		read_policy);
}
int pa_ap_should_client_block(pa_request_rec *r) {
	return ap_should_client_block((request_rec*)r->real_request_rec);
}
long pa_ap_get_client_block(pa_request_rec *r, char *buffer, int bufsiz) {
	return ap_get_client_block((request_rec*)r->real_request_rec,
		buffer, bufsiz);
}
void pa_ap_send_http_header(pa_request_rec *r) {
	ap_send_http_header((request_rec*)r->real_request_rec);
}
int pa_ap_rwrite(const void *buf, int nbyte, pa_request_rec *r) {
	return ap_rwrite(buf, nbyte, (request_rec*)r->real_request_rec);
}


// http_main.h

void pa_ap_hard_timeout(char *s, pa_request_rec *r) {
	ap_hard_timeout(s, (request_rec*)r->real_request_rec);
}
void pa_ap_reset_timeout(pa_request_rec *r) {
	ap_reset_timeout((request_rec*)r->real_request_rec);
}
void pa_ap_kill_timeout(pa_request_rec *r) {
	ap_kill_timeout((request_rec*)r->real_request_rec);
}


// util_script.h

void pa_ap_add_cgi_vars(pa_request_rec *r) {
	ap_add_cgi_vars((request_rec*)r->real_request_rec);
}
void pa_ap_add_common_vars(pa_request_rec *r) {
	ap_add_common_vars((request_rec*)r->real_request_rec);
}


// ap_md5.h

void pa_MD5Init(PA_MD5_CTX *context) { ap_MD5Init((AP_MD5_CTX*)context); }
void pa_MD5Update(PA_MD5_CTX *context, const unsigned char *input,
			      unsigned int inputLen) { ap_MD5Update((AP_MD5_CTX *)context, input, inputLen); }
void pa_MD5Final(unsigned char digest[MD5_DIGESTSIZE],
			     PA_MD5_CTX *context) { ap_MD5Final(digest, (AP_MD5_CTX *)context); }
void pa_MD5Encode(const unsigned char *password,
			      const unsigned char *salt,
			      char *result, size_t nbytes) { ap_MD5Encode(password, salt, result, nbytes); }
void pa_to64(char *s, unsigned long v, int n) { ap_to64(s, v, n); }


// signal.h

void (*pa_signal (int sig, void (*disp)(int)))(int) {
	if(sig==PA_SIGPIPE && disp==PA_SIG_IGN)
		return signal(SIGPIPE, SIG_IGN);

	return 0;
}
