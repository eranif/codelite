#include "named_pipe_server.h"
#ifndef __WXMSW__
# include <sys/types.h>
# include <sys/unistd.h>
# include <sys/socket.h>
#endif
clNamedPipeServer::clNamedPipeServer(const char* pipePath)
		: clNamedPipe(pipePath)
{

}


clNamedPipeServer::~clNamedPipeServer(void)
{
	disconnect();
}

void clNamedPipeServer::disconnect()
{
	this->setLastError(ZNP_OK);
#ifdef __WXMSW__
	if (_pipeHandle != INVALID_PIPE_HANDLE) {
		FlushFileBuffers(_pipeHandle);
		DisconnectNamedPipe(_pipeHandle);
		CloseHandle(_pipeHandle);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#else
	if( _pipeHandle != INVALID_PIPE_HANDLE ) {
		close(_pipeHandle);
		shutdown(_pipeHandle, SHUT_RDWR);
		_pipeHandle = INVALID_PIPE_HANDLE;
	}
#endif
}
