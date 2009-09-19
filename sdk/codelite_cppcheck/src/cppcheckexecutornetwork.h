#ifndef __cppcheckexecutornetwork__
#define __cppcheckexecutornetwork__

#include "cppcheckexecutor.h"
#include "network/named_pipe.h"

class CppCheckExecutorNetwork : public CppCheckExecutor {

	clNamedPipe * _connection;

public:
	char        **_argv;
	int           _argc;

public:
	CppCheckExecutorNetwork(clNamedPipe *connection, int argc, char **argv);
	virtual ~CppCheckExecutorNetwork();

public:
	virtual void reportErr(const ErrorLogger::ErrorMessage &msg);
	virtual void reportStatus(unsigned int index, unsigned int max);
	virtual void reportErr(const std::string &errmsg);
};
#endif // __cppcheckexecutornetwork__
