#ifndef np_connections_server_h__
#define np_connections_server_h__


#include "named_pipe.h"

class clNamedPipeConnectionsServer
{
public:
	enum NP_SERVER_ERRORS {
		NP_SERVER_TIMEOUT = 1,
		NP_SERVER_UNKNOWN_ERROR
	};

	clNamedPipeConnectionsServer(const char* pipeName);
	virtual ~clNamedPipeConnectionsServer();
	bool shutdown();
	clNamedPipe *waitForNewConnection(int timeout);
	NP_SERVER_ERRORS getLastError() { return this->_lastError ; }

protected:
	void setLastError(NP_SERVER_ERRORS error) { this->_lastError = error; }

private:
	PIPE_HANDLE initNewInstance();
	NP_SERVER_ERRORS _lastError;
	char* _pipePath;
	PIPE_HANDLE _listenHandle;
};

#endif // np_connections_server_h__


