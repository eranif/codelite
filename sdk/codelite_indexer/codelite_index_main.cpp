#include <stdio.h>
#include <stdlib.h>

#include "network/named_pipe_client.h"
#include "network/cl_indexer_reply.h"
#include "network/cl_indexer_request.h"
#include "network/np_connections_server.h"
#include "network/clindexerprotocol.h"
#include "libctags/libctags.h"

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer"
DWORD WINAPI ClientThread(LPVOID lpvParam)
{
	Sleep(100);
	clNamedPipeClient client(PIPE_NAME);
	client.connect();
	client.disconnect();
	return 0;
}
#else

#define PIPE_NAME "/tmp/codelite_indexer.sock"
#endif

int main(int argc, char **argv)
{
	int max_requests(10000);
	int requests(0);

	// create the connection factory
	clNamedPipeConnectionsServer server(PIPE_NAME);

	// wait for new connections from the cient side
#ifdef __WXMSW__

	// On Windows, we need to always have a connection active
	// we do this by creating a thread in the server process which
	// connects to the server, once the connection established, we
	// we keep this connection open
	DWORD dwThreadId;
	CreateThread(
				NULL,              // no security attribute
				0,                 // default stack size
				ClientThread,      // thread proc
				NULL,              // thread parameter
				0,                 // not suspended
				&dwThreadId);      // returns thread ID

	clNamedPipe *dummyConnection = server.waitForNewConnection(-1);
	printf("INFO: Received dummy connection from client thread\n");
	
#endif

	printf("INFO: codelite_indexer started\n");
	printf("INFO: listening on %s\n", PIPE_NAME);
	
	while (true) {
		clNamedPipe *conn = server.waitForNewConnection(-1);
		if (!conn) {
			fprintf(stderr, "ERROR: Failed to receive new connection: %d\n", server.getLastError());
			continue;
		}

		// get request from the client
		clIndexerRequest req;
		if ( !clIndexerProtocol::ReadRequest(conn, req) ) {
			continue;
		}

		// create fies for the requested files
		for (size_t i=0; i<req.getFiles().size(); i++) {
			char *tags = ctags_make_tags(req.getCtagOptions().c_str(), req.getFiles().at(i).c_str());
			
			clIndexerReply reply;
			reply.setFileName(req.getFiles().at(i));
			if (tags) {
				// prepare reply
				reply.setCompletionCode(1);
				reply.setTags(tags);
			} else {
				reply.setCompletionCode(0);
			}

			ctags_free(tags);
			// send the reply
			if ( !clIndexerProtocol::SendReply(conn, reply) ) {
				fprintf(stderr, "ERROR: Protocol error: failed to send reply for file %s\n", reply.getFileName().c_str());
				break;
			}
			delete conn;
		}
		requests ++;
		if(requests == max_requests) {
			printf("INFO: Max requests reached, going down\n");
			break;
		}
	}

	ctags_shutdown();
	return 0;
}
