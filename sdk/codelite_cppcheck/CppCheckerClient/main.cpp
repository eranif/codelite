#include "../network/cppchecker_request.h"
#include "../network/cppchecker_protocol.h"
#include "../network/named_pipe_client.h"
#include <stdio.h>

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\cppchecker_%s"
#else
#define PIPE_NAME "/tmp/cppchecker.%s.sock"
#endif



int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Usage: %s <unique string>\n", argv[0]);
		printf("   <unique string> - a unique string that identifies the daemon\n");
		return 1;
	}

	char channel_name[1024];
	sprintf(channel_name, PIPE_NAME, argv[1]);

	CPPCheckerRequest req;
	clNamedPipeClient client(channel_name);

	// build the request
	req.setFile("C:\\Development\\C++\\codelite\\trunk\\Subversion\\");
	client.connect();

	CPPCheckerProtocol::SendRequest(&client, req);
	bool cont( true );
	while ( cont ) {
		CPPCheckerReply reply;
		if ( CPPCheckerProtocol::ReadReply(&client, reply) == false ) {
			// error
			break;
		}
		switch ( reply.getCompletionCode() ) {
		case CPPCheckerReply::StatusMessage:
			printf("STATUS: %s\n", reply.getReport().c_str() );
			break;
		case CPPCheckerReply::CheckingCompleted:
			printf("STATUS: Completed\n");
			cont = false;
			break;
		case CPPCheckerReply::CheckingInProgress:
			printf("REPORT: %s\n", reply.getReport().c_str() );
			break;
		}
	}

	return 0;
}
