/** @file
	Parser: program executing for different OS-es.

	Copyright(c) 2000,2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_exec.C,v 1.3 2001/04/23 09:38:53 paf Exp $
*/

#include "pa_config_includes.h"

#ifdef WIN32
#	include <windows.h>
#else
#	include <signal.h>
#	include <sys/types.h>
#	include <sys/wait.h>
#endif

#include <stdio.h>
#include <errno.h>

#include "pa_exec.h"
#include "pa_exception.h"
#include "pa_common.h"


#ifdef WIN32

/// this func from http://www.ccas.ru/~posp/popov/spawn.htm
static BOOL WINAPI CreateHiddenConsoleProcess( LPCTSTR szChildName,
										char *szEnv,
                                        PROCESS_INFORMATION* ppi, 
                                        LPHANDLE phInWrite,
                                        LPHANDLE phOutRead,
                                        LPHANDLE phErrRead )
{
    BOOL fCreated;
    STARTUPINFO si;
    SECURITY_ATTRIBUTES sa={0};
    HANDLE hInRead;
    HANDLE hOutWrite;
    HANDLE hErrWrite;

    // Create pipes
    // initialize security attributes for handle inheritance (for WinNT)
    sa.nLength = sizeof( sa );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor  = NULL;

    // create STDIN pipe
    if( !CreatePipe( &hInRead, phInWrite, &sa, 0 ))
        goto error;

    // create STDOUT pipe
    if( !CreatePipe( phOutRead, &hOutWrite, &sa, 0 ))
        goto error;

    // create STDERR pipe
    if( !CreatePipe( phErrRead, &hErrWrite, &sa, 0 ))
        goto error;

    // process startup information
    memset( &si, 0, sizeof( si ));
    si.cb = sizeof( si ); 
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    // child process' console must be hidden for Win95 compatibility
    si.wShowWindow = SW_HIDE;
    // assign "other" sides of pipes
//    si.hStdInput = hInRead;
    si.hStdOutput = hOutWrite;
    si.hStdError = hErrWrite;

    // Create a child process (suspended)
    fCreated = CreateProcess( NULL,
                              (LPTSTR)szChildName,
                              NULL,
                              NULL,
                              TRUE,
                              DETACHED_PROCESS,
                              szEnv,
                              NULL,
                              &si,
                              ppi );

    CloseHandle( hInRead );
    CloseHandle( hOutWrite );
    CloseHandle( hErrWrite );

    if( !fCreated )
        goto error;

    return TRUE;

error:
    CloseHandle( *phInWrite );
    CloseHandle( *phOutRead );
    CloseHandle( *phErrRead );

    return FALSE;
}

static void read_pipe(String& result, HANDLE hOutRead, const char *file_spec){
	while(true) {
		char *buf=(char *)result.pool().malloc(MAX_STRING);
		unsigned long size;
		ReadFile( hOutRead, buf, MAX_STRING, &size, NULL );
		if(!size) 
			break;
		result.APPEND_AS_IS(buf, size, file_spec, 0);
    }
}


static const char *buildCommand(Pool& pool, 
								const String& origin_string,
								const char *file_spec_cstr, const Array *argv) {
	FILE *f=fopen(file_spec_cstr, "r");
	if(f) {
		char buf[MAX_STRING];
		size_t size=fread(buf, 1, MAX_STRING-1, f);
		if(size>2) {
			buf[size]=0;
			if(strncmp(buf, "#!", 2)==0) {
				char *atEOL=strchr(buf, '\n');
				if(atEOL) {
					String string(pool);
					string.APPEND_AS_IS(buf+2, atEOL-(buf+2), 
						origin_string.origin().file, 0);
					string << " " << file_spec_cstr;
					if(argv)
						for(int i=0; i<argv->size(); i++)
							string << argv->get_string(i)->cstr(String::UL_AS_IS);
					file_spec_cstr=string.cstr();
				}
			}
		}
		fclose(f);
	}
	return file_spec_cstr;
}

#else

static int execle_piped(const char *path, 
						const char *arg1,
						const char *arg2,
						const char *arg3,
						const char *arg4,
						const char *arg5,
						char * const env[],
						int *pipe_in, int *pipe_out, int *pipe_err) {
	int pid;
	int in_fds[2];
	int out_fds[2];
	int err_fds[2];
	int save_errno;
	
	if(pipe_in && pipe(in_fds)<0) {
		save_errno=errno;
		errno=save_errno;
		return 0;
	}
	
	if(pipe_out && pipe(out_fds)<0) {
		save_errno=errno;
		if(pipe_in) {
			close(in_fds[0]); close(in_fds[1]);
		}
		errno=save_errno;
		return 0;
	}
	
	if(pipe_err && pipe(err_fds)<0) {
		save_errno=errno;
		if(pipe_in) {
			close(in_fds[0]); close(in_fds[1]);
		}
		if(pipe_out) {
			close(out_fds[0]); close(out_fds[1]);
		}
		errno=save_errno;
		return 0;
	}
	
	if((pid=fork())<0) {
		save_errno=errno;
		if(pipe_in) {
			close(in_fds[0]); close(in_fds[1]);
		}
		if(pipe_out) {
			close(out_fds[0]); close(out_fds[1]);
		}
		if(pipe_err) {
			close(err_fds[0]); close(err_fds[1]);
		}
		errno=save_errno;
		return 0;
	}
	
	if(!pid) {
		/* Child process */
		
		if(pipe_out) {
			close(out_fds[0]);
			dup2(out_fds[1], STDOUT_FILENO);
			close(out_fds[1]);
		}
		
		if(pipe_in) {
			close(in_fds[1]);
			dup2(in_fds[0], STDIN_FILENO);
			close(in_fds[0]);
		}
		
		if(pipe_err) {
			close(err_fds[0]);
			dup2(err_fds[1], STDERR_FILENO);
			close(err_fds[1]);
		}
		
		/* HP-UX SIGCHLD fix goes here, if someone will remind me what it is... */
		signal(SIGCHLD, SIG_DFL);	/* Was that it? */
	
		execle(path, arg1, arg2, arg3, arg4, arg5, NULL, env);
		exit(-errno);
	}
	
	/* Parent process */
	
	if(pipe_out) {
		close(out_fds[1]);
		*pipe_out=out_fds[0];
	}
	
	if(pipe_in) {
		close(in_fds[0]);
		*pipe_in=in_fds[1];
	}
	
	if(pipe_err) {
		close(err_fds[1]);
		*pipe_err=err_fds[0];
	}

	return pid;
}

static int get_exit_status(int pid) {
	int status;
	if(!waitpid(pid, &status, 0))
		return -1;
	return WIFEXITED(status) ? 
		WEXITSTATUS(status) : -2;
}

static void read_pipe(String& result, int file, const char *file_spec){
	while(true) {
		char *buf=(char *)result.pool().malloc(MAX_STRING);
		size_t size=read(file, buf, MAX_STRING);
		if(!size) 
			break;
		result.APPEND_AS_IS(buf, size, file_spec, 0);
    }
}

#endif

static void append_env_pair(const Hash::Key& key, Hash::Val *value, void *info) {
#ifdef WIN32
	String& string=*static_cast<String *>(info);
	
	string << key << "=" << *static_cast<String *>(value);
	string.APPEND_AS_IS("", 1, 0, 0); // zero byte
#else
	String string(key.pool());
	string << key << "=" << *static_cast<String *>(value);

	char ***env_ref=static_cast<char ***>(info);
	**env_ref=string.cstr();  (*env_ref)++;
#endif
}
int pa_exec(const String& file_spec, 
			const Hash *env,
			const Array *argv,
			const String& in, String& out, String& err) {
	Pool& pool=file_spec.pool();

#ifdef WIN32

	char pwd[MAX_STRING];
	GetCurrentDirectory(sizeof(pwd), pwd);
	char *dir=file_spec.cstr(String::UL_FILE_NAME);
	rsplit(dir, '/'); SetCurrentDirectory(dir);

	PROCESS_INFORMATION pi;	
	HANDLE hInWrite, hOutRead, hErrRead;
	char *file_spec_cstr=file_spec.cstr(String::UL_FILE_NAME); 
	const char *cmd=buildCommand(file_spec.pool(), file_spec, file_spec_cstr, argv);
	char *env_cstr=0;
	if(env) {
		String string(env->pool());
		env->for_each(append_env_pair, &string);
		env_cstr=string.cstr(String::UL_AS_IS);
	}
	if( CreateHiddenConsoleProcess(cmd, env_cstr, &pi, &hInWrite, &hOutRead, &hErrRead )) {
		SetCurrentDirectory(pwd);

		read_pipe(out, hOutRead, file_spec_cstr);
		read_pipe(err, hErrRead, file_spec_cstr);
		CloseHandle( hInWrite );
		CloseHandle( hOutRead );
		CloseHandle( hErrRead );
/*	
from http://www.apache.org/websrc/cvsweb.cgi/apache-1.3/src/main/util_script.c?rev=1.151&content-type=text/vnd.viewcvs-markup

	* We must close the handles to the new process and its main thread
    * to prevent handle and memory leaks.
*/	
		CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
	} else {
		SetCurrentDirectory(pwd);

		DWORD error=GetLastError();
		char szErrorDesc[MAX_STRING];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                szErrorDesc, sizeof(szErrorDesc), NULL);
		size_t error_size=strlen(szErrorDesc);
		if(error_size>3) // ".\r\n"
			szErrorDesc[error_size-3]=0;
            
		PTHROW(0, 0,
			&file_spec,
			"exec failed - %s (%d)",
				szErrorDesc,
				(long)error);
	}

#else

	int pipe_write, pipe_read, pipe_err;
	const char *argv_cstrs[5]={"", "", "", "", ""};
	if(argv) {
		int size=min(5, argv->size());
		for(int i=0; i<size; i++)
			argv_cstrs[i]=argv->get_string(i)->cstr(String::UL_AS_IS);
	}
	const char *file_spec_cstr=file_spec.cstr(String::UL_FILE_NAME);
	char **env_cstrs=0;
	if(env) {
		env_cstrs=
			(char **)env->pool().malloc(sizeof(char *)*(env->size()+1/*0*/));
		char **env_ref=env_cstrs;
		env->for_each(append_env_pair, &env_ref);
		*env_ref=0;
	}
	if(int pid=execle_piped(
		file_spec_cstr,
		argv_cstrs[0], argv_cstrs[1], argv_cstrs[2], argv_cstrs[3], argv_cstrs[4],
		env_cstrs,
		&pipe_write, &pipe_read, &pipe_err)) {

		const char *in_cstr=in.cstr(String::UL_AS_IS);
		write(pipe_write, in_cstr, in.size());
		close(pipe_write);
		read_pipe(out, pipe_read, file_spec_cstr);
		read_pipe(err, pipe_err, file_spec_cstr);
		close(pipe_read);
		close(pipe_err);

		return get_exit_status(pid); // negative may mean "-errno[execl()]"
	} else 
		PTHROW(0, 0,
			&file_spec,
			"pipe error");
#endif

	return 0;
}
