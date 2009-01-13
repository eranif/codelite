#include <stdio.h>
#include <stdlib.h>
#include "workerthread.h"

#include "equeue.h"
#include "network/named_pipe_client.h"
#include "network/np_connections_server.h"
#include "libctags/libctags.h"

#ifdef __WXMSW__
#ifdef __DEBUG
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_dbg"
#else
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer"
#endif
#else

#define PIPE_NAME "/tmp/codelite_indexer.sock"
#endif

static eQueue<clNamedPipe*> g_connectionQueue;

int main(int argc, char **argv)
{
	int max_requests(1500);
	int requests(0);

	// create the connection factory
	clNamedPipeConnectionsServer server(PIPE_NAME);

	// start the worker thread
	WorkerThread worker( &g_connectionQueue );
	worker.run();

	printf("INFO: codelite_indexer started\n");
	printf("INFO: listening on %s\n", PIPE_NAME);

	while (true) {
		clNamedPipe *conn = server.waitForNewConnection(-1);
		if (!conn) {
			fprintf(stderr, "ERROR: Failed to receive new connection: %d\n", server.getLastError());
			continue;
		}

		// add the request to the queue
		g_connectionQueue.put(conn);

		requests ++;
		if(requests == max_requests) {
			// stop the worker thread and exit
			printf("INFO: Max requests reached, going down\n");
			worker.requestStop();
			worker.wait(-1);
			break;
		}
	}

	// perform some cleanup
	ctags_shutdown();
	return 0;
}
