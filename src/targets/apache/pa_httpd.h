/** @file
	Parser: http wrapper.

	Copyright (c) 2003-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_HTTPD_H
#define PA_HTTPD_H

#define IDENT_PA_HTTPD_H "$Id: pa_httpd.h,v 1.8 2020/12/15 17:10:38 moko Exp $";

#ifdef __cplusplus
extern "C" {
#endif

// import from c to c++

typedef void pa_pool;
typedef void pa_server_rec;
typedef void pa_table;

typedef struct pa_request_rec_tag {
	void* real_request_rec;

	pa_pool* pool;
	int header_only;		/* HEAD request, as opposed to GET */
	int* status;
	const char *method;		/* GET, HEAD, POST, etc. */

	pa_table *headers_out;
	void* subprocess_env;
	const char ** content_type;

	char *uri;			/* the path portion of the URI */
	char *filename;			/* filename if found, otherwise NULL */
	char *path_info;
	char *args;			/* QUERY_ARGS, if any */
	int file_not_found;		/* non-zero if no such file */
} pa_request_rec;

/// apache parser module configuration [httpd.conf + .htaccess-es]
typedef struct Parser_module_config_tag {
	const char* parser_config_filespec; ///< filespec of site's config file
} Parser_module_config;

void pa_setup_module_cells();
void pa_destroy_module_cells();
int pa_parser_handler(pa_request_rec*, Parser_module_config*);

// export from c to c++


// http_log.h

#define	PA_APLOG_EMERG	0	/* system is unusable */
#define	PA_APLOG_ALERT	1	/* action must be taken immediately */
#define	PA_APLOG_CRIT	2	/* critical conditions */
#define	PA_APLOG_ERR	3	/* error conditions */
#define	PA_APLOG_WARNING 4	/* warning conditions */
#define	PA_APLOG_NOTICE	5	/* normal but significant condition */
#define	PA_APLOG_INFO	6	/* informational */
#define	PA_APLOG_DEBUG	7	/* debug-level messages */

#define	PA_APLOG_LEVELMASK 7	/* mask off the level value */

#define PA_APLOG_NOERRNO	(PA_APLOG_LEVELMASK + 1)

#define PA_APLOG_MARK	__FILE__,__LINE__

void pa_ap_log_rerror(const char *file, int line, int level,
			     const pa_request_rec *s, const char *fmt, ...);


void pa_ap_log_error(const char *file, int line, int level,
			     const pa_server_rec *s, const char *fmt, ...);

// ap_alloc.h

const char * pa_ap_table_get(const pa_table *, const char *);
void pa_ap_table_addn(pa_table *, const char *name, const char *val);

int pa_ap_table_size(const pa_table *);

void pa_ap_table_do(int (*comp) (void *, const char *, const char *), 
                                    void *rec, const pa_table *t,...);

char * pa_ap_pstrdup(pa_pool *, const char *s);

// http_protocol.h

/* Possible values for request_rec.read_body (set by handling module):
 *    REQUEST_NO_BODY          Send 413 error if message has any body
 *    REQUEST_CHUNKED_ERROR    Send 411 error if body without Content-Length
 *    REQUEST_CHUNKED_DECHUNK  If chunked, remove the chunks for me.
 *    REQUEST_CHUNKED_PASS     Pass the chunks to me without removal.
 */
#define PA_REQUEST_NO_BODY          0
#define PA_REQUEST_CHUNKED_ERROR    1
#define PA_REQUEST_CHUNKED_DECHUNK  2
#define PA_REQUEST_CHUNKED_PASS     3

int pa_ap_setup_client_block(pa_request_rec *r, int read_policy);
int pa_ap_should_client_block(pa_request_rec *r);
long pa_ap_get_client_block(pa_request_rec *r, char *buffer, int bufsiz);
void pa_ap_send_http_header(pa_request_rec *l);
int pa_ap_rwrite(const void *buf, int nbyte, pa_request_rec *r);


// http_main.h

void pa_ap_hard_timeout(const char *, pa_request_rec *);
void pa_ap_reset_timeout(pa_request_rec *);
void pa_ap_kill_timeout(pa_request_rec *);


// util_script.h

void pa_ap_add_cgi_vars(pa_request_rec *r);
void pa_ap_add_common_vars(pa_request_rec *r);

// httpd.h

#define PA_HTTP_NOT_FOUND                     404
#define PA_OK 0			/* Module has handled this stage. */

// signal.h

#define PA_SIGPIPE 1 /* must translate to real one */
#define PA_SIG_IGN (void (*)(int))1           /* must translate to real one */

void (*pa_signal (int sig, void (*disp)(int)))(int);

#ifdef __cplusplus
}
#endif

#endif
