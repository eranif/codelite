#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifdef __WXMSW__
#include <io.h>
#endif

#include "network/clindexerprotocol.h"
#include "network/cl_indexer_reply.h"
#include "network/cl_indexer_request.h"
#include "network/named_pipe_client.h"
#include "network/np_connections_server.h"

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
#else
#define PIPE_NAME "/tmp/codelite_indexer.%s.sock"
#endif

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("Usage: %s <unique string>\n", argv[0]);
        printf("   <unique string> - a unique string that identifies the indexer which this client should connect\n");
        printf("                     this string may contain only [a-zA-Z]\n");
        return 1;
    }

    char channel_name[1024];
    sprintf(channel_name, PIPE_NAME, argv[1]);

    clIndexerRequest req;
    clNamedPipeClient client(channel_name);

    // build the request
    req.setCmd(clIndexerRequest::CLI_PARSE_AND_SAVE);
    std::vector<std::string> files;

    char* testFile = getenv("TEST_FILE");
    if(testFile == NULL) {
        printf("ERROR: Please set env variable TEST_FILE\n");
        exit(-1);
    }

    std::string file_name = testFile;
    files.push_back(file_name);

    req.setFiles(files);
    req.setCtagOptions("--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p  -IwxT,_T");
    for(size_t i = 0; i < 1; i++) {
        // connect to server
        if(!client.connect()) {
            printf("ERROR: failed to connect to server\n");
            break;
        }

        clIndexerProtocol::SendRequest(&client, req);

        clIndexerReply reply;
        std::string errmsg;
        if(!clIndexerProtocol::ReadReply(&client, reply, errmsg)) {
            printf("ERROR: failed to read reply\n");
        }
        std::cout << reply.getTags() << std::endl;
        client.disconnect();
    }
    return 0;
}
