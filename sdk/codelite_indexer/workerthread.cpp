#include "workerthread.h"
#include "network/named_pipe.h"
#include "network/named_pipe_client.h"
#include "network/cl_indexer_reply.h"
#include "network/cl_indexer_request.h"
#include "network/np_connections_server.h"
#include "network/clindexerprotocol.h"
#include "libctags/libctags.h"
#include "utils.h"
#include <stdlib.h>
#include <cstdio>
#include <memory>

WorkerThread::WorkerThread(eQueue<clNamedPipe*> *queue)
		: m_queue(queue)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::start()
{
	printf("INFO: WorkerThread: Started\n");
	while ( !testDestroy() ) {
		clNamedPipe *conn(NULL);
		if (!m_queue->get(conn, 100)) {
			continue;
		}

		if (conn) {
			std::auto_ptr<clNamedPipe> p( conn );
			// get request from the client
			clIndexerRequest req;
			if ( !clIndexerProtocol::ReadRequest(conn, req) ) {
				continue;
			}

			char *tags(NULL);
			// create fies for the requested files
			for (size_t i=0; i<req.getFiles().size(); i++) {

#ifdef __DEBUG
				printf("------------------------------------------------------------------\n");
				printf("INFO: Source        : %s\n", req.getFiles().at(i).c_str());
				printf("INFO: Command       : %d\n", req.getCmd());
				printf("INFO: CTAGS options : %s\n", req.getCtagOptions().c_str());
				printf("INFO: Database      : %s\n", req.getDatabaseFileName().c_str());
#endif

				char *new_tags = ctags_make_tags(req.getCtagOptions().c_str(), req.getFiles().at(i).c_str());
				if (tags && new_tags) {
					// re-allocate the buffer to containt the new tags + 2 chars: 1 for terminating null and one for the '\n'
					// that will be appended
					char *ptmp = (char*)malloc(strlen(tags) + strlen(new_tags) + 2);
					memset(ptmp, 0, strlen(tags) + strlen(new_tags) + 2);
					strcat(ptmp, tags);
					strcat(ptmp, "\n");
					strcat(ptmp, new_tags);

					ctags_free(new_tags);
					ctags_free(tags);

					tags = ptmp;

				} else if(new_tags) {
					// first time
					tags = new_tags;
					new_tags = NULL;
				}
			}

			// prepare the reply
#ifdef __DEBUG
			std::vector<std::string> lines = string_tokenize(tags, "\n");
			for(size_t i=0; i<lines.size(); i++){
				printf("%s\n", lines.at(i).c_str());
			}
#endif

			clIndexerReply reply;
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
		}
	}
	printf("INFO: WorkerThread: Going down\n");
	exit(-1);
}

// ---------------------------------------------
// is alive thread
// ---------------------------------------------

void IsAliveThread::start()
{
	while ( !testDestroy() ) {
#ifdef __WXMSW__
		Sleep(1000);
#else
		sleep(1);
#endif
		if ( !is_process_alive(m_pid) ) {
			fprintf(stderr, "INFO: parent process died, going down\n");
#ifndef __WXMSW__
			// Delete the local socket
			::unlink(m_socket.c_str());
			::remove(m_socket.c_str());
#endif
			exit(0);
		}
	}
	
#ifndef __WXMSW__
	// Delete the local socket
	::unlink(m_socket.c_str());
	::remove(m_socket.c_str());
#endif
}
