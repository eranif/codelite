#include "unixprocess_impl.h"

#if defined(__WXMAC__)||defined(__WXGTK__)

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/wait.h>

static char *argv[128];
static int    argc = 0;

static void make_argv(const wxString &cmd)
{
	wxString      currentWord;
	bool          inString(false);
	bool          inSingleString(false);
	wxArrayString argvArray;

	// release previous allocation
	for (int i=0; i<argc; i++ ) {
		if ( argv[i] ) {
			free(argv[i]);
		}
	}
	argc = 0;
	for ( size_t i=0; i<cmd.Length(); i++ ) {
		switch ( cmd.GetChar(i) ) {
		case wxT(' '):
					case wxT('\t'):
							if ( !inSingleString && !inString && currentWord.IsEmpty() == false ) {
						// we found the end of the new token
						argvArray.Add( currentWord );
						currentWord.Clear();
					} else if ( inString || inSingleString ) {
						// we are inside a string, concatenate the char
						currentWord << cmd.GetChar(i);
					}
			break;
		case wxT('\''):
			if ( inString ) {
				currentWord << cmd.GetChar(i);
				
			} else  if ( inSingleString && currentWord.IsEmpty() == false ) {
			// this is the terminating quotation mark
				argvArray.Add( currentWord );
				currentWord.Clear();
				inSingleString = false;
			} else {
				currentWord.Clear();
				inSingleString = true;
			}
			break;
		case wxT('"'):
				if ( inSingleString ) {
					currentWord << cmd.GetChar(i);
					
				} else if ( inString && currentWord.IsEmpty() == false ) {
					// this is the terminating quotation mark
					argvArray.Add( currentWord );
					currentWord.Clear();
					inString = false;
					
				} else {
					currentWord.Clear();
					inString = true;
					
				}
			break;
		default:
			currentWord << cmd.GetChar(i);
			break;
		}
	}

	if ( inString ) {
		// we got an unterminated string quotations
		argc = 0;
		return;
	}

	if ( currentWord.IsEmpty() == false ) {
		argvArray.Add( currentWord );
	}

	argc = argvArray.GetCount();
	for (size_t i=0; i<argvArray.GetCount(); i++ ) {
		argv[i] = strdup(argvArray.Item(i).mb_str(wxConvUTF8).data());
		//printf("Argv[%d]=%s\n", i, argv[i]);
	}
	argv[argc] = 0;
}

UnixProcessImpl::UnixProcessImpl(wxEvtHandler *parent)
		: IProcess(parent)
		, m_readHandle  (-1)
		, m_writeHandle (-1)
		, m_thr         (NULL)
{
}

UnixProcessImpl::~UnixProcessImpl()
{
	Cleanup();
}

void UnixProcessImpl::Cleanup()
{
	close(GetReadHandle());
	close(GetWriteHandle());
	
	
	if ( m_thr ) {
		// Stop the reader thread
		m_thr->Stop();
		delete m_thr;
	}
	
	wxKill (GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
	// Perform process cleanup
	int status(0);
	waitpid(GetPid(), &status, 0);
	
	/*
#ifdef __WXGTK__
	// Kill the child process
	wxString cmd;
	wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
	wxFileName script(exePath.GetPath(), wxT("codelite_kill_children"));
	cmd << wxT("/bin/sh -f ") << script.GetFullPath() << wxT(" ") << GetPid();
	wxExecute(cmd, wxEXEC_ASYNC);
#else
	 wxKill(GetPid(), wxSIGKILL, NULL, wxKILL_CHILDREN);
#endif
  */
}

bool UnixProcessImpl::IsAlive()
{
	return kill(m_pid, 0) == 0;
}

bool UnixProcessImpl::Read(wxString& buff)
{
	fd_set  rs;
	timeval timeout;

	memset(&rs, 0, sizeof(rs));
	FD_SET(GetReadHandle(), &rs);
	timeout.tv_sec  = 1; // 1 second
	timeout.tv_usec = 0;

	int rc = select(GetReadHandle()+1, &rs, NULL, NULL, &timeout);
	if ( rc == 0 ) {
		// timeout
		return true;
	} else if ( rc > 0 ) {
		// there is something to read
		char buffer[2048]; // our read buffer
		memset(buffer, 0, sizeof(buffer));
		if(read(GetReadHandle(), buffer, sizeof(buffer)) > 0) {
			buff.Empty();
			buff.Append( wxString(buffer, wxConvUTF8) );
			return true;
		}
		return false;
	} else {
		if ( rc == EINTR || rc == EAGAIN ) {
			return true;
		}
		return false;
	}
}

bool UnixProcessImpl::Write(const wxString& buff)
{
	wxString tmpbuf = buff;
	tmpbuf << wxT("\n");
	int bytes = write(GetWriteHandle(), tmpbuf.mb_str(wxConvUTF8).data(), tmpbuf.Length());
	return bytes == (int)tmpbuf.length();
}

IProcess* UnixProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, const wxString& workingDirectory)
{
	make_argv(cmd);
	if ( argc == 0 ) {
		return NULL;
	}

	int filedes[2];
	int filedes2[2];

	// create a pipe
	int d;
	d = pipe(filedes);
	d = pipe(filedes2);

	wxUnusedVar (d);

	int stdin_pipe_write = filedes[1];
	int stdin_pipe_read  = filedes[0];

	int stdout_pipe_write = filedes2[1];
	int stdout_pipe_read  = filedes2[0];

	// fork the child process
	wxString curdir = wxGetCwd();

	int rc = fork();
	if ( rc == 0 ) {
		// Set process group to child process' pid.  Then killing -pid
		// of the parent will kill the process and all of its children.
		setsid();

		// Child process
		if(workingDirectory.IsEmpty() == false) {
			wxSetWorkingDirectory( workingDirectory );
		}
//		wxPrintf(wxT("My current WD is: %s\n"), wxGetCwd().c_str());
		
		int stdin_file  = fileno( stdin  );
		int stdout_file = fileno( stdout );
		int stderr_file = fileno( stderr );

		// Replace stdin/out with our pipe ends
		dup2 ( stdin_pipe_read,  stdin_file );
		close( stdin_pipe_write );

		dup2 ( stdout_pipe_write, stdout_file);
		dup2 ( stdout_pipe_write, stderr_file);
		close( stdout_pipe_read );

		// execute the process
		execvp(argv[0], argv);

		// if we got here, we failed...
		exit(0);

	} else if ( rc < 0 ) {
		// Error

		// restore the working directory
		wxSetWorkingDirectory(curdir);

		return NULL;

	} else {
		// Parent

		// restore the working directory
		wxSetWorkingDirectory(curdir);

		UnixProcessImpl *proc = new UnixProcessImpl(parent);
		proc->SetReadHandle  (stdout_pipe_read);
		proc->SetWriteHandler(stdin_pipe_write);

		close ( stdin_pipe_read   );
		close ( stdout_pipe_write );
		proc->SetPid( rc );
		proc->StartReaderThread();
		return proc;
	}
}

void UnixProcessImpl::StartReaderThread()
{
	// Launch the 'Reader' thread
	m_thr = new ProcessReaderThread();
	m_thr->SetProcess( this );
	m_thr->SetNotifyWindow( m_parent );
	m_thr->Start();
}

#endif //#if defined(__WXMAC )||defined(__WXGTK__)
