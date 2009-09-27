#include "cppcheckexecutornetwork.h"
#include <sstream>
#include "network/cppchecker_protocol.h"
#include "network/cppchecker_request.h"
#include <iostream>

CppCheckExecutorNetwork::CppCheckExecutorNetwork()
		: _connection(NULL)
		, _argv      (NULL)
		, _argc      (0)
{
}

CppCheckExecutorNetwork::CppCheckExecutorNetwork(clNamedPipe *connection, int argc, char **argv)
		: _connection(connection)
		, _argv (NULL)
		, _argc (0)
{
	// get request from the client
	CPPCheckerRequest req;
	/*printf( "command: " );*/
	if ( CPPCheckerProtocol::ReadRequest(_connection, req) ) {
		_argc = argc;
		_argv = new char*[_argc];

		// copy the original array
		for (int i=0, j=0; i<argc; i++) {
			if ( strncmp ( argv[i], "--daemon", 9) == 0 ) {
				// skip this one
				continue;
			}

			_argv[j] = new char[strlen(argv[i])+1];
			strcpy(_argv[j], argv[i]);
			j++;
		}

		// add the new entry
		_argv[argc-1] = new char [ req.getFile().length() + 1 ];
		strcpy ( _argv[argc-1], req.getFile().c_str() );
	}
}

CppCheckExecutorNetwork::~CppCheckExecutorNetwork()
{
	if ( _connection ) {
		for (int i=0; i<_argc; i++) {
			delete [] _argv[i];
		}
		delete [] _argv;
	}
}

void CppCheckExecutorNetwork::reportErr(const ErrorLogger::ErrorMessage& msg)
{
	// report to the network
	std::string data;
	if ( _settings._xml ) {
		data = msg.toXML();
	} else {
		data = msg.toText();
	}

	if ( _connection ) {
		CPPCheckerReply reply;
		reply.setCompletionCode(CPPCheckerReply::CheckingInProgress);
		reply.setReport( data );
		CPPCheckerProtocol::SendReply(_connection, reply);

	} else {
		fprintf(stdout, "%s", data.c_str());
		fflush (stdout);
	}
}

void CppCheckExecutorNetwork::reportStatus(unsigned int index, unsigned int max)
{
	if (max > 1 && !_settings._errorsOnly) {

		std::ostringstream oss;
		oss << index << "/" << max
		<< " files checked " <<
		static_cast<int>(static_cast<double>(index) / max*100)
		<< "% done";

		if ( _connection ) {
			CPPCheckerReply reply;
			reply.setCompletionCode(CPPCheckerReply::StatusMessage);
			reply.setReport( oss.str() );
			CPPCheckerProtocol::SendReply(_connection, reply);

		} else {
			fprintf(stderr, "%s", oss.str().c_str());
			fflush (stderr);
		}

	}
}

void CppCheckExecutorNetwork::reportErr(const std::string& errmsg)
{
	if ( _connection ) {
		CPPCheckerReply reply;
		reply.setCompletionCode(CPPCheckerReply::CheckingInProgress);
		reply.setReport( errmsg );
		CPPCheckerProtocol::SendReply(_connection, reply);

	} else {
		fprintf(stdout, "%s", errmsg.c_str());
		fflush (stdout);
	}
}
