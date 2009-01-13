#ifndef named_pipe_server_h__
#define named_pipe_server_h__

#include <stdlib.h>
#ifdef __WXMSW__
#include <WinSock2.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "named_pipe.h"

class clNamedPipeServer : public clNamedPipe
{
public:
	clNamedPipeServer(const char* pipePath);
	virtual ~clNamedPipeServer(void);
	void setHandle(PIPE_HANDLE handle) { this->setPipeHandle(handle) ; }
	virtual void disconnect();
};

#endif // named_pipe_server_h__
