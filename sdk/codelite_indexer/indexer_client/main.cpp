#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "network/clindexerprotocol.h"
#include "network/cl_indexer_reply.h"
#include "network/cl_indexer_request.h"
#include "network/named_pipe_client.h"
#include "network/np_connections_server.h"

#ifdef __WXMSW__
#ifdef __DEBUG
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s_dbg"
#else
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
#endif
#else
#define PIPE_NAME "/tmp/codelite_indexer.%s.sock"
#endif

int main(int argc, char **argv)
{
	if(argc < 2){
		printf("Usage: %s <unique string>\n", argv[0]);
		printf("   <unique string> - a unique string that identifies the indexer which this client should connect\n");
		printf("                     this number can contains only [a-zA-Z]\n");
		return 1;
	}

	char channel_name[1024];
	sprintf(channel_name, PIPE_NAME, argv[1]);

	clIndexerRequest req;
	clNamedPipeClient client(channel_name);

	// build the request
	req.setCmd(clIndexerRequest::CLI_PARSE);
	std::vector<std::string> files;

#ifdef __WXMSW__
	files.push_back("C:\\Development\\C++\\codelite\\trunk\\sdk\\codelite_indexer\\workerthread.h");
#else
	char *home = getenv("HOME");
	std::string file_name;
	file_name += home;
	file_name += "/devl/codelite/sqlite3/sqlite3.h";
	files.push_back(file_name);
#endif

	req.setFiles(files);
	req.setCtagOptions("--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p  -IwxT,_T");
	req.setDatabaseFileName("codelite.db");
	for (size_t i=0; i<100; i++) {
		// connect to server
		if(!client.connect()){
			printf("ERROR: failed to connect to server\n");
			break;
		}

		clIndexerProtocol::SendRequest(&client, req);

		clIndexerReply reply;
		if(!clIndexerProtocol::ReadReply(&client, reply)){
			printf("ERROR: failed to read reply\n");
		}

		printf("%s\n", reply.getTags().c_str());
		// close the connection
		client.disconnect();
	}
	return 0;
}
