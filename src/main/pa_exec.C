/** @file
	Parser: program executing for different OS-es.

	Copyright(c) 2000,2001, 2002 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exec.C,v 1.31 2002/03/25 11:36:24 paf Exp $


	@todo setrlimit
*/

#include "pa_config_includes.h"

#include "pa_exec.h"
#include "pa_exception.h"
#include "pa_common.h"

#ifdef WIN32
#	include <windows.h>
#else
#	include <signal.h>
#	include <sys/types.h>
#	include <sys/wait.h>
#endif

#ifdef WIN32

/// this func from http://www.ccas.ru/~posp/popov/spawn.htm
static BOOL WINAPI CreateHiddenConsoleProcess(LPCTSTR szCmdLine,
										char *szEnv,
                                        PROCESS_INFORMATION* ppi, 
                                        LPHANDLE phInWrite,
                                        LPHANDLE phOutRead,
                                        LPHANDLE phErrRead)
{
    BOOL fCreated;
    STARTUPINFO si;
    SECURITY_ATTRIBUTES sa={0};
    HANDLE hInRead;
    HANDLE hOutWrite;
    HANDLE hErrWrite;

    // Create pipes
    // initialize security attributes for handle inheritance (for WinNT)
    sa.nLength=sizeof(sa);
    sa.bInheritHandle=TRUE;
    sa.lpSecurityDescriptor=NULL;

    // create STDIN pipe
    if(!CreatePipe(&hInRead, phInWrite, &sa, 0))
        goto error;

    // create STDOUT pipe
    if(!CreatePipe(phOutRead, &hOutWrite, &sa, 0))
        goto error;

    // create STDERR pipe
    if(!CreatePipe(phErrRead, &hErrWrite, &sa, 0))
        goto error;

    // process startup information
    memset(&si, 0, sizeof(si));
    si.cb=sizeof(si); 
    si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    // child process' console must be hidden for Win95 compatibility
    si.wShowWindow=SW_HIDE;
    // assign "other" sides of pipes
    si.hStdInput=hInRead;
    si.hStdOutput=hOutWrite;
    si.hStdError=hErrWrite;

	// calculating script's directory
	char dir[MAX_STRING];
	strncpy(dir, szCmdLine, MAX_STRING-1); dir[MAX_STRING-1]=0;
	rsplit(dir,'/'); rsplit(dir,'\\'); // trim filename
	chdir(dir);

    // Create a child process (suspended)
    fCreated=CreateProcess(NULL,
                              (LPTSTR)szCmdLine,
                              NULL,
                              NULL,
                              TRUE,
                              CREATE_NO_WINDOW,
                              szEnv,
                              dir,
                              &si,
                              ppi);

    CloseHandle(hInRead);
    CloseHandle(hOutWrite);
    CloseHandle(hErrWrite);

    if(!fCreated)
        goto error;

    return TRUE;

error:
    CloseHandle(*phInWrite);
    CloseHandle(*phOutRead);
    CloseHandle(*phErrRead);

    return FALSE;
}

static void read_pipe(String& result, HANDLE hOutRead, const char *file_spec){
	while(true) {
		char *buf=(char *)result.pool().malloc(MAX_STRING);
		unsigned long size;
		if(!ReadFile(hOutRead, buf, MAX_STRING, &size, NULL) || !size) 
			break;
		result.APPEND_AS_IS(buf, size, file_spec, 0);
    }
}


static const char *buildCommand(Pool& pool, 
								const String& origin_string,
								const char *file_spec_cstr, const Array *argv) {
	const char *result=file_spec_cstr;
	if(FILE *f=fopen(file_spec_cstr, "r")) {
		char buf[MAX_STRING];
		size_t size=fread(buf, 1, MAX_STRING-1, f);
		if(size>2) {
			buf[size]=0;
			if(strncmp(buf, "#!", 2)==0) {
				const char *begin=buf+2;
				if(*begin==' ') // alx: were an old magic for some linux-es
					begin++;
				if(char *end=strchr(begin, '\n')) {
					String string(pool);
					string.APPEND_AS_IS(begin, end-begin, 
						origin_string.origin().file, 0);
					string << " " << file_spec_cstr;
					if(argv)
						for(int i=0; i<argv->size(); i++)
							string << argv->get_string(i)->cstr();
					result=string.cstr();
				}
			}
		}
		fclose(f);
	}
	if(argv) {
		String buf(pool);
		buf << result;
		for(int i=0; i<argv->size(); i++) {
			buf << " ";
			buf << *argv->get_string(i);
		}

		result=buf.cstr();
	}

	return result;
}

#else

static int execve_piped(const char *file_spec_cstr, 
			char * const argv[], char * const env[],
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
	
		// chdir to script's directory
		char dir[MAX_STRING];
		strncpy(dir, file_spec_cstr, MAX_STRING-1); dir[MAX_STRING-1]=0;
		rsplit(dir,'/'); // trim filename
		chdir(dir);

		//  execute
		execve(file_spec_cstr, argv, env);
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

///@test maybe here and at argv construction --- cstr(String::UL_UNSPECIFIED
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

#ifdef NO_PA_EXECS

	throw Exception(0, 0,
		&file_spec,
		"parser execs are disabled [recompile parser without --disable-execs configure option]");

#else

#ifdef WIN32

	PROCESS_INFORMATION pi;	
	HANDLE hInWrite, hOutRead, hErrRead;
	char *file_spec_cstr=file_spec.cstr(String::UL_FILE_SPEC); 
	const char *cmd=buildCommand(file_spec.pool(), file_spec, file_spec_cstr, argv);
	char *env_cstr=0;
	if(env) {
		String string(env->pool());
		env->for_each(append_env_pair, &string);
		env_cstr=string.cstr();
	}
	if(CreateHiddenConsoleProcess(cmd, env_cstr, &pi, &hInWrite, &hOutRead, &hErrRead)) {
		const char *in_cstr=in.cstr();
		DWORD written_size;
		WriteFile(hInWrite, in_cstr, in.size(), &written_size, NULL);
		// EOF for stupid text reads
		// normally they should read CONTENT_LENGTH bytes,
		// without this char
		WriteFile(hInWrite, "\x1A", 1, &written_size, NULL);
		CloseHandle(hInWrite);
		read_pipe(out, hOutRead, file_spec_cstr);
		CloseHandle(hOutRead);
		read_pipe(err, hErrRead, file_spec_cstr);		
		CloseHandle(hErrRead);
/*	
from http://www.apache.org/websrc/cvsweb.cgi/apache-1.3/src/main/util_script.c?rev=1.151&content-type=text/vnd.viewcvs-markup

	* We must close the handles to the new process and its main thread
    * to prevent handle and memory leaks.
*/	
		CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
	} else {
		DWORD error=GetLastError();
		char szErrorDesc[MAX_STRING];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                szErrorDesc, sizeof(szErrorDesc), NULL);
		size_t error_size=strlen(szErrorDesc);
		if(error_size>3) // ".\r\n"
			szErrorDesc[error_size-3]=0;
            
		throw Exception(0, 0,
			&file_spec,
			"(real command line=\"%s\") exec failed - %s (%ld)",
				cmd,
				szErrorDesc, (long)error);
	}

#else

	int pipe_write, pipe_read, pipe_err;
	char *file_spec_cstr=file_spec.cstr(String::UL_FILE_SPEC);

#ifdef NO_FOREIGN_GROUP_FILES
    struct stat finfo;
	if(stat(file_spec_cstr, &finfo)!=0)
		throw Exception(0, 0, 
				&file_spec, 
				"stat failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, file_spec_cstr);

	if(finfo.st_gid/*foreign?*/!=getegid())
		throw Exception(0, 0,
			&file_spec,
			"parser executing files of foreign group is	disabled [recompile parser without --disable-foreign-group-files configure option], actual filename '%s'", 
				file_spec_cstr);

#endif

	char *argv_cstrs[1+10+1]={file_spec_cstr, 0};
	if(argv) {
		const int argv_size=argv->size();
		const int argv_max=sizeof(argv_cstrs)/sizeof(argv_cstrs[0])-1-1;
		if(argv_size>argv_max)
			throw Exception(0, 0,
				&file_spec,
				"too many arguments (%d > max %d)", argv_size, argv_max);
		for(int i=0; i<argv_size; i++)
			argv_cstrs[1+i]=argv->get_string(i)->cstr();
		argv_cstrs[1+argv_size]=0;
	}
	char **env_cstrs=0;
	if(env) {
		env_cstrs=
			(char **)env->pool().malloc(sizeof(char *)*(env->size()+1/*0*/));
		char **env_ref=env_cstrs;
		env->for_each(append_env_pair, &env_ref);
		*env_ref=0;
	}

	int pid=execve_piped(
		file_spec_cstr,
		argv_cstrs, env_cstrs,
		&pipe_write, &pipe_read, &pipe_err);
	if(pid) {
		// in child
		const char *in_cstr=in.cstr();
		write(pipe_write, in_cstr, in.size());
		close(pipe_write);
		read_pipe(out, pipe_read, file_spec_cstr);
		close(pipe_read);
		read_pipe(err, pipe_err, file_spec_cstr);
		close(pipe_err);

		return get_exit_status(pid); // negative may mean "-errno[execl()]"
	} else 
		throw Exception(0, 0,
			&file_spec,
			"pipe error");
#endif
#endif

	return 0;
}
