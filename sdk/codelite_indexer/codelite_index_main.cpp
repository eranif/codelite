#include "equeue.h"
#include "libctags/libctags.h"
#include "network/named_pipe_client.h"
#include "network/np_connections_server.h"
#include "utils.h"
#include "workerthread.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
HINSTANCE gHandler = NULL;
#else
#define PIPE_NAME "/tmp/codelite_indexer.%s.sock"
#endif

static eQueue<clNamedPipe*> g_connectionQueue;

int main(int argc, char** argv)
{
#ifdef __WXMSW__
    // No windows crash dialogs
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    // as described in http://jrfonseca.dyndns.org/projects/gnu-win32/software/drmingw/
    // load the exception handler dll so we will get Dr MinGW at runtime
    gHandler = LoadLibrary("exchndl.dll");
#endif

    int max_requests(5000);
    int requests(0);
    long parent_pid(0);
    if(argc < 2) {
        printf("Usage: %s <string> [--pid]\n", argv[0]);
        printf("Usage: %s --batch <file_list> <output file>\n", argv[0]);
        printf("   <string> - a unique string that identifies this indexer from other instances               \n");
        printf("   --pid    - when set, <string> is handled as process number and the indexer will            \n");
        printf("              check if this process alive. If it is down, the indexer will go down as well\n");
        printf("   --batch  - when set, batch parsing is done using list of files set in file_list argument   \n");
        return 1;
    }

    if(argc == 4 && strcmp(argv[1], "--batch") == 0) {
        // Batch mode
        ctags_batch_parse(argv[2], argv[3]);
        return 0;
    }

    if(argc == 3 && strcmp(argv[2], "--pid") == 0) {
        parent_pid = atol(argv[1]);
        printf("INFO: parent PID is set on %s\n", argv[1]);
    }

    // create the connection factory
    char channel_name[1024];
    sprintf(channel_name, PIPE_NAME, argv[1]);

    clNamedPipeConnectionsServer server(channel_name);

    // start the worker thread
    WorkerThread worker(&g_connectionQueue);

    // start the 'is alive thread'
    IsAliveThread isAliveThread(parent_pid, channel_name);
    worker.run();
    if(parent_pid) {
        isAliveThread.run();
    }

    printf("INFO: codelite_indexer started\n");
    printf("INFO: listening on %s\n", channel_name);

    while(true) {
        clNamedPipe* conn = server.waitForNewConnection(-1);
        if(!conn) {
#ifdef __DEBUG
            fprintf(stderr, "INFO: Failed to receive new connection: %d\n", server.getLastError());
#endif
            continue;
        }

        // add the request to the queue
        g_connectionQueue.put(conn);
        requests++;

        if(requests == max_requests) {
            // stop the worker thread and exit
            printf("INFO: Max requests reached, going down\n");
            worker.requestStop();
            worker.wait(-1);

            // stop the isAlive thread
            if(parent_pid) {
                isAliveThread.requestStop();
                isAliveThread.wait(-1);
            }
            break;
        }
    }

    // perform some cleanup
    ctags_shutdown();
    return 0;
}
