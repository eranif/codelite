#ifndef named_pipe_client_h__
#define named_pipe_client_h__

#include "named_pipe.h"

class clNamedPipeClient : public clNamedPipe
{
public:


	clNamedPipeClient(const char* path);
	virtual ~clNamedPipeClient(void);
	virtual bool connect(long timeToConnect = -1);
	virtual void disconnect();
	virtual bool isConnected();

};

#endif // named_pipe_client_h__
