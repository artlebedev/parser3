/** @file
	Parser: apache 1.3 module.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: mod_parser3.C,v 1.7 2001/03/21 17:16:34 paf Exp $
*/

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"

#include <stdio.h>


#include "pa_common.h"
#include "pa_globals.h"
#include "pa_request.h"

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Data declarations.                                                       */
/*                                                                          */
/* Here are the static cells and structure declarations private to our      */
/* module.                                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*
 * Sample configuration record.  Used for both per-directory and per-server
 * configuration data.
 *
 * It's perfectly reasonable to have two different structures for the two
 * different environments.  The same command handlers will be called for
 * both, though, so the handlers need to be able to tell them apart.  One
 * possibility is for both structures to start with an int which is zero for
 * one and 1 for the other.
 *
 * Note that while the per-directory and per-server configuration records are
 * available to most of the module handlers, they should be treated as
 * READ-ONLY by all except the command and merge handlers.  Sometimes handlers
 * are handed a record that applies to the current location by implication or
 * inheritance, and modifying it will change the rules for other locations.
 */
typedef struct excfg {
    int cmode;                  /* Environment to which record applies (directory,
                                 * server, or combination).
                                 */
#define CONFIG_MODE_SERVER 1
#define CONFIG_MODE_DIRECTORY 2
#define CONFIG_MODE_COMBO 3     /* Shouldn't ever happen. */
    int local;                  /* Boolean: "Example" directive declared here? */
    int congenital;             /* Boolean: did we inherit an "Example"? */
    char *trace;                /* Pointer to trace string. */
    char *loc;                  /* Location to which this record applies. */
} excfg;

/*
 * Let's set up a module-local static cell to point to the accreting callback
 * trace.  As each API callback is made to us, we'll tack on the particulars
 * to whatever we've already recorded.  To avoid massive memory bloat as
 * directories are walked again and again, we record the routine/environment
 * the first time (non-request context only), and ignore subsequent calls for
 * the same routine/environment.
 */
static const char *trace = NULL;
static table *static_calls_made = NULL;

/*
 * To avoid leaking memory from pools other than the per-request one, we
 * allocate a module-private pool, and then use a sub-pool of that which gets
 * freed each time we modify the trace.  That way previous layers of trace
 * data don't get lost.
 */
static pool *parser3_pool = NULL;
static pool *parser3_subpool = NULL;

/*
 * Declare ourselves so the configuration routines can find and know us.
 * We'll fill it in at the end of the module.
 */
extern "C" module MODULE_VAR_EXPORT parser3_module;

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* The following pseudo-prototype declarations illustrate the parameters    */
/* passed to command handlers for the different types of directive          */
/* syntax.  If an argument was specified in the directive definition        */
/* (look for "command_rec" below), it's available to the command handler    */
/* via the (void *) info field in the cmd_parms argument passed to the      */
/* handler (cmd->info for the examples below).                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*
 * Command handler for a NO_ARGS directive.
 *
 * static const char *handle_NO_ARGS(cmd_parms *cmd, void *mconfig);
 */

/*
 * Command handler for a RAW_ARGS directive.  The "args" argument is the text
 * of the commandline following the directive itself.
 *
 * static const char *handle_RAW_ARGS(cmd_parms *cmd, void *mconfig,
 *                                    const char *args);
 */

/*
 * Command handler for a FLAG directive.  The single parameter is passed in
 * "bool", which is either zero or not for Off or On respectively.
 *
 * static const char *handle_FLAG(cmd_parms *cmd, void *mconfig, int bool);
 */

/*
 * Command handler for a TAKE1 directive.  The single parameter is passed in
 * "word1".
 *
 * static const char *handle_TAKE1(cmd_parms *cmd, void *mconfig,
 *                                 char *word1);
 */

/*
 * Command handler for a TAKE2 directive.  TAKE2 commands must always have
 * exactly two arguments.
 *
 * static const char *handle_TAKE2(cmd_parms *cmd, void *mconfig,
 *                                 char *word1, char *word2);
 */

/*
 * Command handler for a TAKE3 directive.  Like TAKE2, these must have exactly
 * three arguments, or the parser complains and doesn't bother calling us.
 *
 * static const char *handle_TAKE3(cmd_parms *cmd, void *mconfig,
 *                                 char *word1, char *word2, char *word3);
 */

/*
 * Command handler for a TAKE12 directive.  These can take either one or two
 * arguments.
 * - word2 is a NULL pointer if no second argument was specified.
 *
 * static const char *handle_TAKE12(cmd_parms *cmd, void *mconfig,
 *                                  char *word1, char *word2);
 */

/*
 * Command handler for a TAKE123 directive.  A TAKE123 directive can be given,
 * as might be expected, one, two, or three arguments.
 * - word2 is a NULL pointer if no second argument was specified.
 * - word3 is a NULL pointer if no third argument was specified.
 *
 * static const char *handle_TAKE123(cmd_parms *cmd, void *mconfig,
 *                                   char *word1, char *word2, char *word3);
 */

/*
 * Command handler for a TAKE13 directive.  Either one or three arguments are
 * permitted - no two-parameters-only syntax is allowed.
 * - word2 and word3 are NULL pointers if only one argument was specified.
 *
 * static const char *handle_TAKE13(cmd_parms *cmd, void *mconfig,
 *                                  char *word1, char *word2, char *word3);
 */

/*
 * Command handler for a TAKE23 directive.  At least two and as many as three
 * arguments must be specified.
 * - word3 is a NULL pointer if no third argument was specified.
 *
 * static const char *handle_TAKE23(cmd_parms *cmd, void *mconfig,
 *                                  char *word1, char *word2, char *word3);
 */

/*
 * Command handler for a ITERATE directive.
 * - Handler is called once for each of n arguments given to the directive.
 * - word1 points to each argument in turn.
 *
 * static const char *handle_ITERATE(cmd_parms *cmd, void *mconfig,
 *                                   char *word1);
 */

/*
 * Command handler for a ITERATE2 directive.
 * - Handler is called once for each of the second and subsequent arguments
 *   given to the directive.
 * - word1 is the same for each call for a particular directive instance (the
 *   first argument).
 * - word2 points to each of the second and subsequent arguments in turn.
 *
 * static const char *handle_ITERATE2(cmd_parms *cmd, void *mconfig,
 *                                    char *word1, char *word2);
 */

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* These routines are strictly internal to this module, and support its     */
/* operation.  They are not referenced by any external portion of the       */
/* server.                                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*
 * Locate our directory configuration record for the current request.
 */
static excfg *our_dconfig(request_rec *r)
{

    return (excfg *) ap_get_module_config(r->per_dir_config, &parser3_module);
}

#if 0
/*
 * Locate our server configuration record for the specified server.
 */
static excfg *our_sconfig(server_rec *s)
{

    return (excfg *) ap_get_module_config(s->module_config, &parser3_module);
}

/*
 * Likewise for our configuration record for the specified request.
 */
static excfg *our_rconfig(request_rec *r)
{

    return (excfg *) ap_get_module_config(r->request_config, &parser3_module);
}
#endif

/*
 * This routine sets up some module-wide cells if they haven't been already.
 */
static void setup_module_cells()
{
    /*
     * If we haven't already allocated our module-private pool, do so now.
     */
    if (parser3_pool == NULL) {
        parser3_pool = ap_make_sub_pool(NULL);
    };
    /*
     * Likewise for the table of routine/environment pairs we visit outside of
     * request context.
     */
    if (static_calls_made == NULL) {
        static_calls_made = ap_make_table(parser3_pool, 16);
    };
}

/*
 * This routine is used to add a trace of a callback to the list.  We're
 * passed the server record (if available), the request record (if available),
 * a pointer to our private configuration record (if available) for the
 * environment to which the callback is supposed to apply, and some text.  We
 * turn this into a textual representation and add it to the tail of the list.
 * The list can be displayed by the parser3_handler() routine.
 *
 * If the call occurs within a request context (i.e., we're passed a request
 * record), we put the trace into the request pool and attach it to the
 * request via the notes mechanism.  Otherwise, the trace gets added
 * to the static (non-request-specific) list.
 *
 * Note that the r->notes table is only for storing strings; if you need to
 * maintain per-request data of any other type, you need to use another
 * mechanism.
 */

#define TRACE_NOTE "example-trace"

static void trace_add(server_rec *s, request_rec *r, excfg *mconfig,
                      const char *note)
{

    const char *sofar;
    char *addon;
    char *where;
    pool *p;
    const char *trace_copy;

    /*
     * Make sure our pools and tables are set up - we need 'em.
     */
    setup_module_cells();
    /*
     * Now, if we're in request-context, we use the request pool.
     */
    if (r != NULL) {
        p = r->pool;
        if ((trace_copy = ap_table_get(r->notes, TRACE_NOTE)) == NULL) {
            trace_copy = "";
        }
    }
    else {
        /*
         * We're not in request context, so the trace gets attached to our
         * module-wide pool.  We do the create/destroy every time we're called
         * in non-request context; this avoids leaking memory in some of
         * the subsequent calls that allocate memory only once (such as the
         * key formation below).
         *
         * Make a new sub-pool and copy any existing trace to it.  Point the
         * trace cell at the copied value.
         */
        p = ap_make_sub_pool(parser3_pool);
        if (trace != NULL) {
            trace = ap_pstrdup(p, trace);
        }
        /*
         * Now, if we have a sub-pool from before, nuke it and replace with
         * the one we just allocated.
         */
        if (parser3_subpool != NULL) {
            ap_destroy_pool(parser3_subpool);
        }
        parser3_subpool = p;
        trace_copy = trace;
    }
    /*
     * If we weren't passed a configuration record, we can't figure out to
     * what location this call applies.  This only happens for co-routines
     * that don't operate in a particular directory or server context.  If we
     * got a valid record, extract the location (directory or server) to which
     * it applies.
     */
    where = (mconfig != NULL) ? mconfig->loc : "nowhere";
    where = (where != NULL) ? where : "";
    /*
     * Now, if we're not in request context, see if we've been called with
     * this particular combination before.  The table is allocated in the
     * module's private pool, which doesn't get destroyed.
     */
    if (r == NULL) {
        char *key;

        key = ap_pstrcat(p, note, ":", where, NULL);
        if (ap_table_get(static_calls_made, key) != NULL) {
            /*
             * Been here, done this.
             */
            return;
        }
        else {
            /*
             * First time for this combination of routine and environment -
             * log it so we don't do it again.
             */
            ap_table_set(static_calls_made, key, "been here");
        }
    }
    addon = ap_pstrcat(p, "   <LI>\n", "    <DL>\n", "     <DT><SAMP>",
                    note, "</SAMP>\n", "     </DT>\n", "     <DD><SAMP>[",
                    where, "]</SAMP>\n", "     </DD>\n", "    </DL>\n",
                    "   </LI>\n", NULL);
    sofar = (trace_copy == NULL) ? "" : trace_copy;
    trace_copy = ap_pstrcat(p, sofar, addon, NULL);
    if (r != NULL) {
        ap_table_set(r->notes, TRACE_NOTE, trace_copy);
    }
    else {
        trace = trace_copy;
    }
    /*
     * You *could* change the following if you wanted to see the calling
     * sequence reported in the server's error_log, but beware - almost all of
     * these co-routines are called for every single request, and the impact
     * on the size (and readability) of the error_log is considerable.
     */
#define parser3_LOG_EACH 0
#if parser3_LOG_EACH
    if (s != NULL) {
        ap_log_error(APLOG_MARK, APLOG_DEBUG, s, "mod_example: %s", note);
    }
#endif
}

/*--------------------------------------------------------------------------*/
/* We prototyped the various syntax for command handlers (routines that     */
/* are called when the configuration parser detects a directive declared    */
/* by our module) earlier.  Now we actually declare a "real" routine that   */
/* will be invoked by the parser when our "real" directive is               */
/* encountered.                                                             */
/*                                                                          */
/* If a command handler encounters a problem processing the directive, it   */
/* signals this fact by returning a non-NULL pointer to a string            */
/* describing the problem.                                                  */
/*                                                                          */
/* The magic return value DECLINE_CMD is used to deal with directives       */
/* that might be declared by multiple modules.  If the command handler      */
/* returns NULL, the directive was processed; if it returns DECLINE_CMD,    */
/* the next module (if any) that declares the directive is given a chance   */
/* at it.  If it returns any other value, it's treated as the text of an    */
/* error message.                                                           */
/*--------------------------------------------------------------------------*/
/* 
 * Command handler for the NO_ARGS "Example" directive.  All we do is mark the
 * call in the trace log, and flag the applicability of the directive to the
 * current location in that location's configuration record.
 */
static const char *cmd_example(cmd_parms *cmd, void *mconfig)
{

    excfg *cfg = (excfg *) mconfig;

    /*
     * "Example Wuz Here"
     */
    cfg->local = 1;
    trace_add(cmd->server, NULL, cfg, "cmd_example()");
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


// service funcs

const char *get_env(Pool& pool, const char *name) {
	request_rec *r=static_cast<request_rec *>(pool.info());
 	return (const char *)ap_table_get(r->subprocess_env, name);
}

uint read_post(Pool& pool, char *buf, uint max_bytes) {
	request_rec *r=static_cast<request_rec *>(pool.info());

/*    ap_log_error(APLOG_MARK, APLOG_DEBUG, r->server, 
		"mod_parser3: read_post(max=%u)", max_bytes);
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

void add_header_attribute(Pool& pool, const char *key, const char *value) {
	request_rec *r=static_cast<request_rec *>(pool.info());

	if(strcasecmp(key, "content-type")==0) {
		/* r->content_type, *not* r->headers_out("Content-type").  If you don't
		 * set it, it will be filled in with the server's default type (typically
		 * "text/plain").  You *must* also ensure that r->content_type is lower
		 * case.
		 */
		r->content_type = value;
	} else
		ap_table_merge(r->headers_out, key, value);
}

void send_header(Pool& pool) {
	request_rec *r=static_cast<request_rec *>(pool.info());

    ap_hard_timeout("send header", r);
    ap_send_http_header(r);
	ap_kill_timeout(r);
}

void send_body(Pool& pool, const char *buf, size_t size) {
	request_rec *r=static_cast<request_rec *>(pool.info());

    ap_hard_timeout("send body", r);
	ap_rwrite(buf, size, r);
	ap_kill_timeout(r);
}

///@todo initSocks();
static int parser3_handler(request_rec *r)
{
	Pool pool;
	pool.set_storage(r->pool);
	pool.set_info(r);

    excfg *dcfg;

    dcfg = our_dconfig(r);


	/* A flag which modules can set, to indicate that the data being
	 * returned is volatile, and clients should be told not to cache it.
	 */
	r->no_cache=1;

	PTRY { // global try
		const char *filespec_to_process=r->filename;

		ap_add_common_vars(r);
		ap_add_cgi_vars(r);

		// Request info
		Request::Info request_info;
		const char *document_root=
			(const char *)ap_table_get(r->subprocess_env, "DOCUMENT_ROOT");
		if(!document_root) {
			static char fake_document_root[MAX_STRING];
			strncpy(fake_document_root, filespec_to_process, MAX_STRING);
			rsplit(fake_document_root, '/');  rsplit(fake_document_root, '\\');// strip filename
			document_root=fake_document_root;
		}
		request_info.document_root=document_root;
		request_info.path_translated=filespec_to_process;
		request_info.method=r->method;
		request_info.query_string=r->args;
		request_info.uri=
			(const char *)ap_table_get(r->subprocess_env, "REQUEST_URI");
		request_info.content_type=
			(const char *)ap_table_get(r->subprocess_env, "CONTENT_TYPE");
		const char *content_length = 
			(const char *)ap_table_get(r->subprocess_env, "CONTENT_LENGTH");
		request_info.content_length=(content_length?atoi(content_length):0);
		request_info.cookie=
			(const char *)ap_table_get(r->subprocess_env, "HTTP_COOKIE");

		// prepare to process request
		Request request(pool,
			request_info,
			String::UL_HTML_TYPO
			);
		
		/* move this to httpd.conf | .htaccess

		// some root-controlled location
		char *sys_auto_path1;
#ifdef WIN32
		// c:\windows
		sys_auto_path1=(char *)pool.malloc(MAX_STRING);
		GetWindowsDirectory(sys_auto_path1, MAX_STRING);
		strcat(sys_auto_path1, PATH_DELIMITER_STRING);
#else
		// ~nobody
		sys_auto_path1=zgetenv("HOME");
#endif
		
		// beside by binary
		char *sys_auto_path2=(char *)pool.malloc(MAX_STRING);
		strncpy(sys_auto_path2, argv[0], MAX_STRING);  // filespec of my binary
		rsplit(sys_auto_path2, '/');  rsplit(sys_auto_path2, '\\');// strip filename
		strcat(sys_auto_path2, PATH_DELIMITER_STRING);
		
		*/

		// process the request
		request.core(
			0/*sys_auto_path1*/, 
			0/*sys_auto_path2*/,
			r->header_only!=0);
		// no actions with request' data past this point
		// request.exception not not handled here, but all
		// request' data are associated with it's pool=exception

		// successful finish
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		int content_length=strlen(body);

		// prepare header
		(*service_funcs.add_header_attribute)(pool, "content-type", "text/plain");
		char content_length_cstr[MAX_NUMBER];
		snprintf(content_length_cstr, MAX_NUMBER, "%d", content_length);
		(*service_funcs.add_header_attribute)(pool, "content-length", 
			content_length_cstr);

		// send header
		(*service_funcs.send_header)(pool);

		// send body
		if(!r->header_only)
			(*service_funcs.send_body)(pool, body, content_length);

		// unsuccessful finish
	}
	PEND_CATCH

    /*
     * We did what we wanted to do, so tell the rest of the server we
     * succeeded.
     */
    return OK;
}

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

/*
 * All our module-initialiser does is add its trace to the log.
 */
static void parser3_init(server_rec *s, pool *p)
{
	static bool globals_inited=false;
	if(globals_inited)
		return;
	globals_inited=true;

	static Pool pool; // global pool
	pool.set_storage(p);
	PTRY {
		// init global variables
		globals_init(pool);

		// Service funcs 
		service_funcs.get_env=get_env;
		service_funcs.read_post=read_post;
		service_funcs.add_header_attribute=add_header_attribute;
		service_funcs.send_header=send_header;
		service_funcs.send_body=send_body;
		
		//...
	} PCATCH(e) { // global problem 
		const char *body=e.comment();
		// somehow report that error
	}
	PEND_CATCH
}

/* 
 * This function is called when an heavy-weight process (such as a child) is
 * being run down or destroyed.  As with the child-initialisation function,
 * any information that needs to be recorded must be in static cells, since
 * there's no configuration record.
 *
 * There is no return value.
 */

/*
 * All our process-death routine does is add its trace to the log.
 */
static void parser3_child_exit(server_rec *s, pool *p)
{

    char *note;
    char *sname = s->server_hostname;

    /*
     * The arbitrary text we add to our trace entry indicates for which server
     * we're being called.
     */
    sname = (sname != NULL) ? sname : "";
    note = ap_pstrcat(p, "parser3_child_exit(", sname, ")", NULL);
    trace_add(s, NULL, NULL, note);
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
static void *parser3_create_dir_config(pool *p, char *dirspec)
{

    excfg *cfg;
    char *dname = dirspec;

    /*
     * Allocate the space for our record from the pool supplied.
     */
    cfg = (excfg *) ap_pcalloc(p, sizeof(excfg));
    /*
     * Now fill in the defaults.  If there are any `parent' configuration
     * records, they'll get merged as part of a separate callback.
     */
    cfg->local = 0;
    cfg->congenital = 0;
    cfg->cmode = CONFIG_MODE_DIRECTORY;
    /*
     * Finally, add our trace to the callback list.
     */
    dname = (dname != NULL) ? dname : "";
    cfg->loc = ap_pstrcat(p, "DIR(", dname, ")", NULL);
    trace_add(NULL, NULL, cfg, "parser3_create_dir_config()");
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
 */
static void *parser3_merge_dir_config(pool *p, void *parent_conf,
                                      void *newloc_conf)
{

    excfg *merged_config = (excfg *) ap_pcalloc(p, sizeof(excfg));
    excfg *pconf = (excfg *) parent_conf;
    excfg *nconf = (excfg *) newloc_conf;
    char *note;

    /*
     * Some things get copied directly from the more-specific record, rather
     * than getting merged.
     */
    merged_config->local = nconf->local;
    merged_config->loc = ap_pstrdup(p, nconf->loc);
    /*
     * Others, like the setting of the `congenital' flag, get ORed in.  The
     * setting of that particular flag, for instance, is TRUE if it was ever
     * true anywhere in the upstream configuration.
     */
    merged_config->congenital = (pconf->congenital | pconf->local);
    /*
     * If we're merging records for two different types of environment (server
     * and directory), mark the new record appropriately.  Otherwise, inherit
     * the current value.
     */
    merged_config->cmode =
        (pconf->cmode == nconf->cmode) ? pconf->cmode : CONFIG_MODE_COMBO;
    /*
     * Now just record our being called in the trace list.  Include the
     * locations we were asked to merge.
     */
    note = ap_pstrcat(p, "parser3_merge_dir_config(\"", pconf->loc, "\",\"",
                   nconf->loc, "\")", NULL);
    trace_add(NULL, NULL, merged_config, note);
    return (void *) merged_config;
}

/*
 * This function gets called to create a per-server configuration
 * record.  It will always be called for the "default" server.
 *
 * The return value is a pointer to the created module-specific
 * structure.
 */
static void *parser3_create_server_config(pool *p, server_rec *s)
{

    excfg *cfg;
    char *sname = s->server_hostname;

    /*
     * As with the parser3_create_dir_config() reoutine, we allocate and fill
     * in an empty record.
     */
    cfg = (excfg *) ap_pcalloc(p, sizeof(excfg));
    cfg->local = 0;
    cfg->congenital = 0;
    cfg->cmode = CONFIG_MODE_SERVER;
    /*
     * Note that we were called in the trace list.
     */
    sname = (sname != NULL) ? sname : "";
    cfg->loc = ap_pstrcat(p, "SVR(", sname, ")", NULL);
    trace_add(s, NULL, cfg, "parser3_create_server_config()");
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
static void *parser3_merge_server_config(pool *p, void *server1_conf,
                                         void *server2_conf)
{

    excfg *merged_config = (excfg *) ap_pcalloc(p, sizeof(excfg));
    excfg *s1conf = (excfg *) server1_conf;
    excfg *s2conf = (excfg *) server2_conf;
    char *note;

    /*
     * Our inheritance rules are our own, and part of our module's semantics.
     * Basically, just note whence we came.
     */
    merged_config->cmode =
        (s1conf->cmode == s2conf->cmode) ? s1conf->cmode : CONFIG_MODE_COMBO;
    merged_config->local = s2conf->local;
    merged_config->congenital = (s1conf->congenital | s1conf->local);
    merged_config->loc = ap_pstrdup(p, s2conf->loc);
    /*
     * Trace our call, including what we were asked to merge.
     */
    note = ap_pstrcat(p, "parser3_merge_server_config(\"", s1conf->loc, "\",\"",
                   s2conf->loc, "\")", NULL);
    trace_add(NULL, NULL, merged_config, note);
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
static int parser3_translate_handler(request_rec *r)
{

    excfg *cfg;

    cfg = our_dconfig(r);
    /*
     * We don't actually *do* anything here, except note the fact that we were
     * called.
     */
    trace_add(r->server, r, cfg, "parser3_translate_handler()");
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
static int parser3_check_user_id(request_rec *r)
{

    excfg *cfg;

    cfg = our_dconfig(r);
    /*
     * Don't do anything except log the call.
     */
    trace_add(r->server, r, cfg, "parser3_check_user_id()");
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
static int parser3_auth_checker(request_rec *r)
{

    excfg *cfg;

    cfg = our_dconfig(r);
    /*
     * Log the call and return OK, or access will be denied (even though we
     * didn't actually do anything).
     */
    trace_add(r->server, r, cfg, "parser3_auth_checker()");
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
static int parser3_access_checker(request_rec *r)
{

    excfg *cfg;

    cfg = our_dconfig(r);
    trace_add(r->server, r, cfg, "parser3_access_checker()");
    return DECLINED;
}

/*
 * This routine is called to perform any module-specific logging activities
 * over and above the normal server things.
 *
 * The return value is OK, DECLINED, or HTTP_mumble.  If we return OK, any
 * remaining modules with an handler for this phase will still be called.
 */
static int parser3_logger(request_rec *r)
{

    excfg *cfg;

    cfg = our_dconfig(r);
    trace_add(r->server, r, cfg, "parser3_logger()");
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
static const command_rec parser3_cmds[] =
{
    {
        "Example",              /* directive name */
        (const char *(*)(void))((void *)cmd_example),            /* config action routine */
        NULL,                   /* argument to include in call */
        (int)OR_OPTIONS,             /* where available */
        NO_ARGS,                /* arguments */
        "Example directive - no arguments"
                                /* directive description */
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
static const handler_rec parser3_handlers[] =
{
    {"parser3-handler", parser3_handler},
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
    parser3_init,               /* module initializer */
    parser3_create_dir_config,  /* per-directory config creator */
    parser3_merge_dir_config,   /* dir config merger */
    parser3_create_server_config,       /* server config creator */
    parser3_merge_server_config,        /* server config merger */
    parser3_cmds,               /* command table */
    parser3_handlers,           /* [9] list of handlers */
    parser3_translate_handler,  /* [2] filename-to-URI translation */
    parser3_check_user_id,      /* [5] check/validate user_id */
    parser3_auth_checker,       /* [6] check user_id is valid *here* */
    parser3_access_checker,     /* [4] check access by host address */
    0,                          /* [7] MIME type checker/setter */
    0,                          /* [8] fixups */
    parser3_logger              /* [10] logger */
#if MODULE_MAGIC_NUMBER >= 19970103
    ,0                          /* [3] header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
    ,0                          /* process initializer */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
    ,parser3_child_exit         /* process exit/cleanup */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
    ,0   /* [1] post read_request handling */
#endif
};
