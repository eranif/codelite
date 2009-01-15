#include "workerthread.h"
#include "network/named_pipe.h"
#include "network/named_pipe_client.h"
#include "network/cl_indexer_reply.h"
#include "network/cl_indexer_request.h"
#include "network/np_connections_server.h"
#include "network/clindexerprotocol.h"
#include "libctags/libctags.h"
#include "utils.h"

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
			// get request from the client
			clIndexerRequest req;
			if ( !clIndexerProtocol::ReadRequest(conn, req) ) {
				continue;
			}

			// create fies for the requested files
			for (size_t i=0; i<req.getFiles().size(); i++) {

#ifdef __DEBUG
				printf("------------------------------------------------------------------\n");
				printf("INFO: Source        : %s\n", req.getFiles().at(i).c_str());
				printf("INFO: Command       : %d\n", req.getCmd());
				printf("INFO: CTAGS options : %s\n", req.getCtagOptions().c_str());
				printf("INFO: Database      : %s\n", req.getDatabaseFileName().c_str());
#endif
				char *tags = ctags_make_tags(req.getCtagOptions().c_str(), req.getFiles().at(i).c_str());

#ifdef __DEBUG
				std::vector<std::string> lines = string_tokenize(tags, "\n");
#endif
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
		}
	}
	printf("INFO: WorkerThread: Going down\n");
}
