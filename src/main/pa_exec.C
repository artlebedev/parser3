/** @file
	Parser: program executing for different OS-es.

	Copyright(c) 2000,2001-2004 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	@todo setrlimit
*/

static const char * const IDENT_EXEC_C="$Date: 2004/12/23 16:34:50 $";

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
static DWORD CreateHiddenConsoleProcess(LPCTSTR szCmdLine,
										LPCTSTR szScriptFileSpec,
										char *szEnv,
                                        PROCESS_INFORMATION* ppi, 
                                        LPHANDLE phInWrite,
                                        LPHANDLE phOutRead,
                                        LPHANDLE phErrRead)
{
	DWORD result=0;
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
	strncpy(dir, szScriptFileSpec, MAX_STRING-1); dir[MAX_STRING-1]=0;
	lsplit(dir,' '); // trim arguments
	rsplit(dir,'/'); rsplit(dir,'\\'); // trim filename
	
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
	if(!fCreated)
		result=GetLastError();
	
	CloseHandle(hInRead);
	CloseHandle(hOutWrite);
	CloseHandle(hErrWrite);
	
	if(!fCreated)
		goto error;
	
	return result;
	
error:
	if(!result/*yet*/)
		result=GetLastError(); // get it
	
	CloseHandle(*phInWrite);
	CloseHandle(*phOutRead);
	CloseHandle(*phErrRead);
	
	return result;
}

static void read_pipe(String& result, HANDLE hOutRead, String::Language lang){
	while(true) {
		char *buf=new(PointerFreeGC) char[MAX_STRING];
		unsigned long size;
		if(!ReadFile(hOutRead, buf, MAX_STRING-1, &size, NULL) || !size) 
			break;
		buf[size]=0;
		result.append_know_length(buf, size, lang);
	}
}

static const char* buildCommand(const char* file_spec_cstr, const ArrayString& argv) {
	const char* result=file_spec_cstr;
	if(FILE *f=fopen(file_spec_cstr, "r")) {
		try {
		char buf[MAX_STRING];
		size_t size=fread(buf, 1, MAX_STRING-1, f);
		if(size>2) {
			buf[size]=0;
			if(strncmp(buf, "#!", 2)==0) {
				const char* begin=buf+2;
				while(*begin==' ') // alx: were an old magic for some linux-es
					begin++;
				if(char *end=strchr(begin, '\n')) {
					String string(pa_strdup(begin, end-begin));
					string << " " << file_spec_cstr;
					result=string.cstr();
				}
			}
		}
		} catch(...) {
			fclose(f);
			rethrow;
		}
		fclose(f);
	}
	{ // appending argv
		String string(result);
		for(size_t i=0; i<argv.count(); i++) {
			string << " ";
			string << *argv[i];
		}

		result=string.cstr();
	}

	return result;
}

#else

static pid_t execve_piped(const char* file_spec_cstr, 
			char * const argv[], char * const env[],
			int *pipe_in, int *pipe_out, int *pipe_err) {
	pid_t pid;
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
		return -1;
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
		
		/* grabbed this from Apache source: */
		/*  HP-UX SIGCHLD fix goes here, if someone will remind me what it is... */
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
	pid_t cid;
	while ((cid=waitpid(pid, &status, WUNTRACED)) == -1 && errno == EINTR);	
	if(!cid)
		return -1;
	return WIFEXITED(status) ? 
		WEXITSTATUS(status) : -2;
}

static void read_pipe(String& result, int file, String::Language lang){
	while(true) {
		char *buf=new(PointerFreeGC) char[MAX_STRING];
		ssize_t length=read(file, buf, MAX_STRING-1);
		if(length<=0)
			break;
		buf[length]=0;
		result.append_know_length(buf, length, lang);
	}
}

#endif

#ifndef DOXYGEN
struct Append_env_pair_info {
#ifdef WIN32
	String::Body& body;
	Append_env_pair_info(String::Body& abody): body(abody) {}
#else
	char **env_ref;
#endif
};
#endif
///@test maybe here and at argv construction --- cstr(String::L_UNSPECIFIED
static void append_env_pair(HashStringString::key_type key, HashStringString::value_type value,
		Append_env_pair_info *info) {
#ifdef WIN32
	info->body << key << "=" << value;
	info->body.append_know_length("\1", 1); // placeholder for of zero byte
#else
	String::Body body;
	body << key << "=" << value.cstr();

	*(info->env_ref++)=body.cstrm();
#endif
}

PA_exec_result pa_exec(
			bool 
#if defined(NO_PA_EXEC) || defined(PA_SAFE_MODE)
			forced_allow
#endif
			, 
			const String& file_spec, 
			const HashStringString* env, 
			const ArrayString& argv, 
			String& in) {
	PA_exec_result result;

#ifdef NO_PA_EXECS
	if(!forced_allow)
		throw Exception("parser.runtime",
			&file_spec,
			"parser execs are disabled [recompile parser without --disable-execs configure option]");
#endif

#ifdef WIN32

	PROCESS_INFORMATION pi;	
	HANDLE hInWrite, hOutRead, hErrRead;
	const char* script_spec_cstr=file_spec.cstr(String::L_FILE_SPEC);
	const char* cmd=buildCommand(script_spec_cstr, argv);
	char* env_cstr=0;
	if(env) {
		String::Body body;
		Append_env_pair_info info(body);
		env->for_each(append_env_pair, &info);
		env_cstr=info.body.cstrm();
		for(char* replacer=env_cstr; *replacer; replacer++)
			if(*replacer=='\1')
				*replacer=0;
	}
	if(DWORD error=CreateHiddenConsoleProcess(cmd, script_spec_cstr, env_cstr, &pi, &hInWrite, &hOutRead, &hErrRead)) {
		char szErrorDesc[MAX_STRING];
		const char* param="the file you tried to run";
		size_t error_size=FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ARGUMENT_ARRAY , NULL, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			szErrorDesc, sizeof(szErrorDesc), (va_list *)&param);
		if(error_size>3) // ".\r\n"
			szErrorDesc[error_size-3]=0;
            
		throw Exception(0,
			&file_spec,
			"exec failed - %s (%u). Consider adding shbang line (#!x:\\interpreter\\command line)", 
				error_size?szErrorDesc:"<unknown>", error);
	} else {
		const char* in_cstr=in.cstr();
		DWORD written_size;
		WriteFile(hInWrite, in_cstr, in.length(), &written_size, NULL);
		// EOF for stupid text reads
		// normally they should read CONTENT_LENGTH bytes,
		// without this char
		WriteFile(hInWrite, "\x1A", 1, &written_size, NULL);
		CloseHandle(hInWrite);
		read_pipe(result.out, hOutRead, String::L_AS_IS);
		CloseHandle(hOutRead);
		read_pipe(result.err, hErrRead, String::L_TAINTED);		
		CloseHandle(hErrRead);
/*	
from http://www.apache.org/websrc/cvsweb.cgi/apache-1.3/src/main/util_script.c?rev=1.151&content-type=text/vnd.viewcvs-markup

	* We must close the handles to the new process and its main thread
    * to prevent handle and memory leaks.
*/	
		CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
	}

#else

	// execve needs non const
	char* file_spec_cstr=file_spec.cstrm(String::L_FILE_SPEC); 

	int pipe_write, pipe_read, pipe_err;

#ifdef PA_SAFE_MODE
	if(!forced_allow) {
		struct stat finfo;
		if(stat(file_spec_cstr, &finfo)!=0)
			throw Exception("file.missing", 
				&file_spec, 
				"stat failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, file_spec_cstr);

		check_safe_mode(finfo, file_spec, file_spec_cstr);
	}
#endif

	char* argv_cstrs[1+10+1]={file_spec_cstr, 0};
	const int argv_size=argv.count();
	const int argv_max=sizeof(argv_cstrs)/sizeof(argv_cstrs[0])-1-1;
	if(argv_size>argv_max)
		throw Exception("parser.runtime",
			&file_spec,
			"too many arguments (%d > max %d)", argv_size, argv_max);
	for(int i=0; i<argv_size; i++)
		argv_cstrs[1+i]=argv[i]->cstrm();
	argv_cstrs[1+argv_size]=0;

	char **env_cstrs;
	if(env) {
		env_cstrs=new(PointerFreeGC) char *[env->count()+1/*0*/];
		Append_env_pair_info info={env_cstrs}; 
		env->for_each(append_env_pair, &info);
		*info.env_ref=0;
	} else
		env_cstrs=0;

	pid_t pid=execve_piped(
		file_spec_cstr,
		argv_cstrs, env_cstrs,
		&pipe_write, &pipe_read, &pipe_err);
	if(pid>0) {
		// in child
		if(in.length()) {// there is some in data
			const char* in_cstr=in.cstr();
			write(pipe_write, in_cstr, in.length());
		}
		close(pipe_write);
		read_pipe(result.out, pipe_read, String::L_AS_IS);
		close(pipe_read);
		read_pipe(result.err, pipe_err, String::L_TAINTED);
		close(pipe_err);

		result.status=get_exit_status(pid); // negative may mean "-errno[execl()]"
	} else { 
		const char* str=strerror(errno);
		throw Exception(0,
			&file_spec,
			"%s error: %s (%d)", pid<0?"fork":"pipe", str?str:"<unknown>", errno); 
	}
#endif

	return result;
}
