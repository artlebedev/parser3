/* Copyright Abandoned 1999 TCX DataKonsult AB & Monty Program KB & Detron HB
   This file is public domain and comes with NO WARRANTY of any kind */

/* Defines to make different thread packages compatible */

#ifndef _my_pthread_h
#define _my_pthread_h

#include <errno.h>
#ifndef ETIME
#define ETIME ETIMEDOUT				// For FreeBSD
#endif

#if defined(__WIN32__)

typedef CRITICAL_SECTION pthread_mutex_t;
typedef HANDLE		 pthread_t;
typedef struct thread_attr {
    DWORD dwStackSize ;
    DWORD dwCreatingFlag ;
    int priority ;
} pthread_attr_t ;

typedef struct { int dummy; } pthread_condattr_t;

/* Implementation of posix conditions */

typedef struct st_pthread_link {
  DWORD thread_id;
  struct st_pthread_link *next;
} pthread_link;

typedef struct {
  uint32 waiting;
  HANDLE semaphore;
} pthread_cond_t;


struct timespec {		/* For pthread_cond_timedwait() */
    time_t tv_sec;
    long tv_nsec;
};

#define win_pthread_self my_thread_var->pthread_self
#define pthread_handler_decl(A,B) unsigned __cdecl A(void *B)
typedef unsigned (__cdecl *pthread_handler)(void *);

int win_pthread_setspecific(void *A,void *B,uint length);
int pthread_create(pthread_t *,pthread_attr_t *,pthread_handler,void *);
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
			   struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_attr_init(pthread_attr_t *connect_att);
int pthread_attr_setstacksize(pthread_attr_t *connect_att,DWORD stack);
int pthread_attr_setprio(pthread_attr_t *connect_att,int priority);
int pthread_attr_destroy(pthread_attr_t *connect_att);
struct tm *localtime_r(const time_t *timep,struct tm *tmp);

void pthread_exit(unsigned A);	 /* was #define pthread_exit(A) ExitThread(A)*/

#define ETIMEDOUT 145		    /* Win32 doesn't have this */
#define getpid() GetCurrentThreadId()
#define pthread_self() win_pthread_self
#define HAVE_LOCALTIME_R
#define _REENTRANT
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE

#ifdef USE_TLS					/* For LIBMYSQL.DLL */
#define pthread_key(T,V)  DWORD V
#define pthread_key_create(A,B) ((*A=TlsAlloc())==0xFFFFFFFF)
#define pthread_getspecific(A) (TlsGetValue(A))
#define my_pthread_getspecific(T,A) ((T) TlsGetValue(A))
#define my_pthread_getspecific_ptr(T,V) ((T) TlsGetValue(A))
#define my_pthread_setspecific_ptr(T,V) TlsSetValue(T,V)
#define pthread_setspecific(A,B) TlsSetValue(A,B)
#else
#define pthread_key(T,V) __declspec(thread) T V
#define pthread_key_create(A,B) pthread_dummy(0)
#define pthread_getspecific(A) (&(A))
#define my_pthread_getspecific(T,A) (&(A))
#define my_pthread_getspecific_ptr(T,V) (V)
#define my_pthread_setspecific_ptr(T,V) ((T)=(V),0)
#define pthread_setspecific(A,B) win_pthread_setspecific(&(A),(B),sizeof(A))
#endif /* USE_TLS */

#define pthread_equal(A,B) ((A) == (B))
#define pthread_mutex_init(A,B)  InitializeCriticalSection(A)
#define pthread_mutex_lock(A)	 (EnterCriticalSection(A),0)
#define pthread_mutex_unlock(A)  LeaveCriticalSection(A)
#define pthread_mutex_destroy(A) DeleteCriticalSection(A)
#define my_pthread_setprio(A,B)  SetThreadPriority(GetCurrentThread(), (B))
/* Dummy defines for easier code */
#define pthread_kill(A,B) pthread_dummy(0)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define my_pthread_attr_setprio(A,B) pthread_attr_setprio(A,B)
#define pthread_attr_setscope(A,B)
#define pthread_detach_this_thread()
#define pthread_condattr_init(A)
#define pthread_condattr_destroy(A)

//Irena: compiler does not like this:
//#define my_pthread_getprio(pthread_t thread_id) pthread_dummy(0)
#define my_pthread_getprio(thread_id) pthread_dummy(0)

#elif defined(HAVE_UNIXWARE7_THREADS)

#include <thread.h>
#include <synch.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

#define HAVE_NONPOSIX_SIGWAIT
#define pthread_t thread_t
#define pthread_cond_t cond_t
#define pthread_mutex_t mutex_t
#define pthread_key_t thread_key_t
typedef int pthread_attr_t;			/* Needed by Unixware 7.0.0 */

#define pthread_key_create(A,B) thr_keycreate((A),(B))

#define pthread_handler_decl(A,B) void *A(void *B)
#define pthread_key(T,V) pthread_key_t V

void *	my_pthread_getspecific_imp(pthread_key_t key);
#define my_pthread_getspecific(A,B) ((A) my_pthread_getspecific_imp(B))
#define my_pthread_getspecific_ptr(T,V) my_pthread_getspecific(T,V)

#define pthread_setspecific(A,B) thr_setspecific(A,B)
#define my_pthread_setspecific_ptr(T,V) pthread_setspecific(T,V)

#define pthread_create(A,B,C,D) thr_create(NULL,65536L,(C),(D),THR_DETACHED,(A))
#define pthread_cond_init(a,b) cond_init((a),USYNC_THREAD,NULL)
#define pthread_cond_destroy(a) cond_destroy(a)
#define pthread_cond_signal(a) cond_signal(a)
#define pthread_cond_wait(a,b) cond_wait((a),(b))
#define pthread_cond_timedwait(a,b,c) cond_timedwait((a),(b),(c))
#define pthread_cond_broadcast(a) cond_broadcast(a)

#define pthread_mutex_init(a,b) mutex_init((a),USYNC_THREAD,NULL)
#define pthread_mutex_lock(a) mutex_lock(a)
#define pthread_mutex_unlock(a) mutex_unlock(a)
#define pthread_mutex_destroy(a) mutex_destroy(a)

#define pthread_self() thr_self()
#define pthread_exit(A) thr_exit(A)
#define pthread_equal(A,B) (((A) == (B)) ? 1 : 0)
#define pthread_kill(A,B) thr_kill((A),(B))
#define HAVE_PTHREAD_KILL

#define pthread_sigmask(A,B,C) thr_sigsetmask((A),(B),(C))

extern int my_sigwait(sigset_t *set,int *sig);

#define pthread_detach_this_thread() pthread_dummy(0)

#define pthread_attr_init(A) pthread_dummy(0)
#define pthread_attr_destroy(A) pthread_dummy(0)
#define pthread_attr_setscope(A,B) pthread_dummy(0)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define my_pthread_setprio(A,B) pthread_dummy (0)
#define my_pthread_getprio(A) pthread_dummy (0)
#define my_pthread_attr_setprio(A,B) pthread_dummy(0)

#else /* Normal threads */

#ifdef HAVE_rts_threads
#define sigwait org_sigwait
#include <signal.h>
#undef sigwait
#endif
#undef _REENTRANT			/* Fix if _REENTRANT is in pthread.h */
#include <pthread.h>
#ifndef _REENTRANT
#define _REENTRANT
#endif
#ifdef HAVE_THR_SETCONCURRENCY
#include <thread.h>			/* Probably solaris */
#endif
#ifdef HAVE_SCHED_H
#include <sched.h>
#endif
#ifdef HAVE_SYNCH_H
#include <synch.h>
#endif
#if defined(__EMX__) && (!defined(EMX_PTHREAD_REV) || (EMX_PTHREAD_REV < 2))
#error Requires at least rev 2 of EMX pthreads library.
#endif

extern int my_pthread_getprio(pthread_t thread_id);

#define pthread_key(T,V) pthread_key_t V
#define my_pthread_getspecific_ptr(T,V) my_pthread_getspecific(T,(V))
#define my_pthread_setspecific_ptr(T,V) pthread_setspecific(T,(void*) (V))
#define pthread_detach_this_thread()
#define pthread_handler_decl(A,B) void *A(void *B)
typedef void *(* pthread_handler)(void *);

/* safe mutex for debugging */

typedef struct st_safe_mutex_t
{
  pthread_mutex_t global,mutex;
  char *file;
  uint line,count;
  pthread_t thread;
} safe_mutex_t;

int safe_mutex_init(safe_mutex_t *mp, const pthread_mutexattr_t *attr);
int safe_mutex_lock(safe_mutex_t *mp,const char *file, uint line);
int safe_mutex_unlock(safe_mutex_t *mp,const char *file, uint line);
int safe_mutex_destroy(safe_mutex_t *mp);
int safe_cond_wait(pthread_cond_t *cond, safe_mutex_t *mp,const char *file, uint line);
int safe_cond_timedwait(pthread_cond_t *cond, safe_mutex_t *mp,
			struct timespec *abstime, const char *file, uint line);

#ifdef SAFE_MUTEX
#undef pthread_mutex_init
#undef pthread_mutex_lock
#undef pthread_mutex_unlock
#undef pthread_mutex_destroy
#undef pthread_mutex_wait
#undef pthread_mutex_timedwait
#undef pthread_mutex_t
#define pthread_mutex_init(A,B) safe_mutex_init((A),(B))
#define pthread_mutex_lock(A) safe_mutex_lock((A),__FILE__,__LINE__)
#define pthread_mutex_unlock(A) safe_mutex_unlock((A),__FILE__,__LINE__)
#define pthread_mutex_destroy(A) safe_mutex_destroy((A))
#define pthread_cond_wait(A,B) safe_cond_wait((A),(B),__FILE__,__LINE__)
#define pthread_cond_timedwait(A,B,C) safe_cond_timedwait((A),(B),(C),__FILE__,__LINE__)
#define pthread_mutex_t safe_mutex_t
#endif

/* Test first for RTS or FSU threads */

#if defined(PTHREAD_SCOPE_GLOBAL) && !defined(PTHREAD_SCOPE_SYSTEM)
#define HAVE_rts_threads
extern int my_pthread_create_detached;
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#define PTHREAD_CREATE_DETACHED &my_pthread_create_detached
#define PTHREAD_SCOPE_SYSTEM  PTHREAD_SCOPE_GLOBAL
#define PTHREAD_SCOPE_PROCESS PTHREAD_SCOPE_LOCAL
#define USE_ALARM_THREAD
#elif defined(HAVE_mit_thread)
#define USE_ALARM_THREAD
#undef	HAVE_LOCALTIME_R
#define HAVE_LOCALTIME_R
#undef	HAVE_PTHREAD_ATTR_SETSCOPE
#define HAVE_PTHREAD_ATTR_SETSCOPE
#undef HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R	/* If we are running linux */
#undef HAVE_RWLOCK_T
#undef HAVE_RWLOCK_INIT
#undef HAVE_SNPRINTF

#define sigset(A,B) pthread_signal((A),(void (*)(int)) (B))
#define signal(A,B) pthread_signal((A),(void (*)(int)) (B))
#define my_pthread_attr_setprio(A,B)
#endif /* defined(PTHREAD_SCOPE_GLOBAL) && !defined(PTHREAD_SCOPE_SYSTEM) */

#if defined(_BSDI_VERSION) && _BSDI_VERSION < 199910
int sigwait(const sigset_t *set, int *sig);
#endif

#if defined(HAVE_UNIXWARE7_POSIX)
#undef HAVE_NONPOSIX_SIGWAIT
#define HAVE_NONPOSIX_SIGWAIT	/* sigwait takes only 1 argument */
#endif

#ifndef HAVE_NONPOSIX_SIGWAIT
#define my_sigwait(A,B) sigwait((A),(B))
#else
int my_sigwait(sigset_t *set,int *sig);
#endif

#ifdef HAVE_NONPOSIX_PTHREAD_MUTEX_INIT
#ifndef SAFE_MUTEX
#define pthread_mutex_init(a,b) my_pthread_mutex_init((a),(b))
extern int my_pthread_mutex_init(pthread_mutex_t *mp,
				 const pthread_mutexattr_t *attr);
#endif /* SAFE_MUTEX */
#define pthread_cond_init(a,b) my_pthread_cond_init((a),(b))
extern int my_pthread_cond_init(pthread_cond_t *mp,
				const pthread_condattr_t *attr);
#endif /* HAVE_NONPOSIX_PTHREAD_MUTEX_INIT */

#if defined(HAVE_SIGTHREADMASK) && !defined(HAVE_PTHREAD_SIGMASK)
#define pthread_sigmask(A,B,C) sigthreadmask((A),(B),(C))
#endif

#if !defined(HAVE_SIGWAIT) && !defined(HAVE_mit_thread) && !defined(HAVE_rts_threads) && !defined(sigwait) && !defined(alpha_linux_port) && !defined(HAVE_NONPOSIX_SIGWAIT) && !defined(HAVE_DEC_3_2_THREADS) && !defined(_AIX)
int sigwait(sigset_t *setp, int *sigp);		/* Use our implemention */
#endif
#if !defined(HAVE_SIGSET) && !defined(HAVE_mit_thread) && !defined(sigset)
#define sigset(A,B) signal((A),(B))
#endif

#ifndef my_pthread_setprio
#if defined(HAVE_PTHREAD_SETPRIO_NP)		/* FSU threads */
#define my_pthread_setprio(A,B) pthread_setprio_np((A),(B))
#elif defined(HAVE_PTHREAD_SETPRIO)
#define my_pthread_setprio(A,B) pthread_setprio((A),(B))
#else
extern void my_pthread_setprio(pthread_t thread_id,int prior);
#endif
#endif

#ifndef my_pthread_attr_setprio
#ifdef HAVE_PTHREAD_ATTR_SETPRIO
#define my_pthread_attr_setprio(A,B) pthread_attr_setprio((A),(B))
#else
extern void my_pthread_attr_setprio(pthread_attr_t *attr, int priority);
#endif
#endif

#if !defined(HAVE_PTHREAD_ATTR_SETSCOPE) || defined(HAVE_DEC_3_2_THREADS)
#define pthread_attr_setscope(A,B)
#undef	HAVE_GETHOSTBYADDR_R			/* No definition */
#endif

#ifndef HAVE_NONPOSIX_PTHREAD_GETSPECIFIC
#define my_pthread_getspecific(A,B) ((A) pthread_getspecific(B))
#else
#define my_pthread_getspecific(A,B) ((A) my_pthread_getspecific_imp(B))
void *my_pthread_getspecific_imp(pthread_key_t key);
#endif

#ifndef HAVE_LOCALTIME_R
struct tm *localtime_r(const time_t *clock, struct tm *res);
#endif

#ifdef HAVE_PTHREAD_CONDATTR_CREATE
/* DCE threads on HPUX 10.20 */
#define pthread_condattr_init pthread_condattr_create
#define pthread_condattr_destroy pthread_condattr_delete
#endif

#ifdef HAVE_CTHREADS_WRAPPER			/* For MacOSX */
#define pthread_cond_destroy(A) pthread_dummy(0)
#define pthread_mutex_destroy(A) pthread_dummy(0)
#define pthread_attr_delete(A) pthread_dummy(0)
#define pthread_condattr_delete(A) pthread_dummy(0)
#define pthread_attr_setstacksize(A,B) pthread_dummy(0)
#define pthread_equal(A,B) ((A) == (B))
#define pthread_cond_timedwait(a,b,c) pthread_cond_wait((a),(b))
#define pthread_attr_init(A) pthread_attr_create(A)
#define pthread_attr_destroy(A) pthread_attr_delete(A)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define pthread_create(A,B,C,D) pthread_create((A),*(B),(C),(D))
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#define pthread_kill(A,B) pthread_dummy(0)
#undef	pthread_detach_this_thread
#define pthread_detach_this_thread() { pthread_t tmp=pthread_self() ; pthread_detach(&tmp); }
#endif

#if ((defined(HAVE_PTHREAD_ATTR_CREATE) && !defined(HAVE_SIGWAIT)) || defined(HAVE_DEC_3_2_THREADS)) && !defined(HAVE_CTHREADS_WRAPPER)
/* This is set on AIX_3_2 and Siemens unix (and DEC OSF/1 3.2 too) */
#define pthread_key_create(A,B) \
		pthread_keycreate(A,(B) ?\
				  (pthread_destructor_t) (B) :\
				  (pthread_destructor_t) pthread_dummy)
#define pthread_attr_init(A) pthread_attr_create(A)
#define pthread_attr_destroy(A) pthread_attr_delete(A)
#define pthread_attr_setdetachstate(A,B) pthread_dummy(0)
#define pthread_create(A,B,C,D) pthread_create((A),*(B),(C),(D))
#ifndef pthread_sigmask
#define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C))
#endif
#define pthread_kill(A,B) pthread_dummy(0)
#undef	pthread_detach_this_thread
#define pthread_detach_this_thread() { pthread_t tmp=pthread_self() ; pthread_detach(&tmp); }
#else /* HAVE_PTHREAD_ATTR_CREATE && !HAVE_SIGWAIT */
#define HAVE_PTHREAD_KILL
#endif

#if defined(HAVE_PTHREAD_ATTR_CREATE) || defined(_AIX) || defined(HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R)
#if !defined(HPUX)
struct hostent;
#endif /* HPUX */
struct hostent *my_gethostbyname_r(const char *name,
				   struct hostent *result, char *buffer,
				   int buflen, int *h_errnop);
#if defined(HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R)
#define GETHOSTBYNAME_BUFF_SIZE 2048
#else
#define GETHOSTBYNAME_BUFF_SIZE sizeof(struct hostent_data)
#endif /* defined(HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R) */

#else
#define my_gethostbyname_r(A,B,C,D,E) gethostbyname_r((A),(B),(C),(D),(E))
#define GETHOSTBYNAME_BUFF_SIZE 2048
#endif /* defined(HAVE_PTHREAD_ATTR_CREATE) || defined(_AIX) || defined(HAVE_GLIBC2_STYLE_GETHOSTBYNAME_R) */

#endif /* defined(__WIN32__) */

/* READ-WRITE thread locking */

#if defined(USE_MUTEX_INSTEAD_OF_RW_LOCKS)
/* use these defs for simple mutex locking */
#define rw_lock_t pthread_mutex_t
#define my_rwlock_init(A,B) pthread_mutex_init((A),(B))
#define rw_rdlock(A) pthread_mutex_lock((A))
#define rw_wrlock(A) pthread_mutex_lock((A))
#define rw_unlock(A) pthread_mutex_unlock((A))
#define rwlock_destroy(A) pthread_mutex_destroy((A))
#elif defined(HAVE_PTHREAD_RWLOCK_RDLOCK)
#define rw_lock_t pthread_rwlock_t
#define my_rwlock_init(A,B) pthread_rwlock_init((A),(B))
#define rw_rdlock(A) pthread_rwlock_rdlock(A)
#define rw_wrlock(A) pthread_rwlock_wrlock(A)
#define rw_unlock(A) pthread_rwlock_unlock(A)
#define rwlock_destroy(A) pthread_rwlock_destroy(A)
#elif defined(HAVE_RWLOCK_INIT)
#ifdef HAVE_RWLOCK_T				/* For example Solaris 2.6-> */
#define rw_lock_t rwlock_t
#endif
#define my_rwlock_init(A,B) rwlock_init((A),USYNC_THREAD,0)
#else
/* Use our own version of read/write locks */
typedef struct _my_rw_lock_t {
	pthread_mutex_t lock;		/* lock for structure		*/
	pthread_cond_t	readers;	/* waiting readers		*/
	pthread_cond_t	writers;	/* waiting writers		*/
	int		state;		/* -1:writer,0:free,>0:readers	*/
	int		waiters;	/* number of waiting writers	*/
} my_rw_lock_t;

#define rw_lock_t my_rw_lock_t
#define rw_rdlock(A) my_rw_rdlock((A))
#define rw_wrlock(A) my_rw_wrlock((A))
#define rw_unlock(A) my_rw_unlock((A))
#define rwlock_destroy(A) my_rwlock_destroy((A))

extern	int	my_rwlock_init( my_rw_lock_t *, void * );
extern	int	my_rwlock_destroy( my_rw_lock_t * );
extern	int	my_rw_rdlock( my_rw_lock_t * );
extern	int	my_rw_wrlock( my_rw_lock_t * );
extern	int	my_rw_unlock( my_rw_lock_t * );
#endif /* USE_MUTEX_INSTEAD_OF_RW_LOCKS */

#define GETHOSTBYADDR_BUFF_SIZE 2048

#ifndef HAVE_THR_SETCONCURRENCY
#define thr_setconcurrency(A) pthread_dummy(0)
#endif
#if !defined(HAVE_PTHREAD_ATTR_SETSTACKSIZE) && ! defined(pthread_attr_setstacksize)
#define pthread_attr_setstacksize(A,B) pthread_dummy(0)
#endif

extern my_bool my_thread_global_init(void);
extern void my_thread_global_end(void);
extern my_bool my_thread_init(void);
extern void my_thread_end(void);
extern char *my_thread_name(void);
extern long my_thread_id(void);
extern int pthread_no_free(void *);
extern int pthread_dummy(int);

/* All thread specific variables are in the following struct */

#define THREAD_NAME_SIZE 10

struct st_my_thread_var
{
  int thr_errno;
  int cmp_length;
  volatile int abort;
  long id;
  pthread_cond_t suspend, *current_cond;
  pthread_mutex_t mutex,  *current_mutex;
  pthread_t pthread_self;
#ifndef DBUG_OFF
  char name[THREAD_NAME_SIZE+1];
  gptr dbug;
#endif
};

extern struct st_my_thread_var *_my_thread_var(void) __attribute__ ((const));
#define my_thread_var (_my_thread_var())
#define my_errno my_thread_var->thr_errno

	/* This is only used for not essential statistic */

#ifndef thread_safe_increment
#ifdef SAFE_STATISTICS
#define thread_safe_increment(V,L) \
	pthread_mutex_lock((L)); (V)++; pthread_mutex_unlock((L));
#define thread_safe_add(V,C,L) \
	pthread_mutex_lock((L)); (V)+=(C); pthread_mutex_unlock((L));
#else
#define thread_safe_increment(V,L) (V)++
#define thread_safe_add(V,C,L) (V)+=(C)
#endif
#endif /* thread_safe_increment */

#endif /* _my_ptread_h */
