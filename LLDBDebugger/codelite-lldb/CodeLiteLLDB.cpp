#ifndef __WXMSW__

#include <wx/init.h>
#include "CodeLiteLLDBApp.h"
#include <unistd.h>
#include <sys/wait.h>

static wxString s_localSocket;
static pid_t childProcess = 0;
void OnTerminate(int signo)
{
    // Remove the socket path
    if ( !s_localSocket.IsEmpty()) {
        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        exit(0);
        
    } else if ( childProcess ) {
        // Kill our child process and exit
        ::kill(childProcess, 9);
        exit(0);
    }
}

void PrintUsage()
{
    printf("Usage:\n");
    printf("codelite-lldb -s </path/to/socket>\n");
    printf("codelite-lldb -t <ip:port>\n");
    printf("codelite-lldb -h\n");
}

int main(int argc, char** argv)
{
    wxInitializer intializer(argc, argv);

    // Register some signal handlers
    signal(SIGTERM, OnTerminate);
    signal(SIGABRT, OnTerminate);
    signal(SIGINT,  OnTerminate);
    signal(SIGPIPE, SIG_IGN);

    // Parse command line
    int c;
    wxString tcpConnectString;
    while ((c = getopt (argc, argv, "hs:t:")) != -1) {
        switch(c) {
        case 's':
            s_localSocket = optarg;
            break;

        case 't':
            tcpConnectString = optarg;
            break;

        case 'h':
            PrintUsage();
            exit(EXIT_SUCCESS);

        default:
            break;
        }
    }

    if (s_localSocket.IsEmpty() && tcpConnectString.IsEmpty() ) {
        PrintUsage();
        exit(EXIT_FAILURE);
    }

    if ( !s_localSocket.IsEmpty() ) {
        // Start on local socket
        CodeLiteLLDBApp app(s_localSocket);
        app.MainLoop();

        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        
    } else if ( !tcpConnectString.IsEmpty() ) {
        // parse the connect string
        long portNum;
        wxString ip_addr    = tcpConnectString.BeforeFirst(':');
        wxString port_str   = tcpConnectString.AfterFirst(':');
        port_str.ToCLong( &portNum );
        
        // Connect using tcp-ip
        // When using tcp-ip we fork the child process and wait until it terminates
        while ( true ) {
            childProcess = fork();
            if ( childProcess == 0 ) {
                
                // Child code
                CodeLiteLLDBApp app(ip_addr, portNum);
                app.MainLoop();
                exit(0);
                
            } else if ( childProcess ) {
                int status (0);
                ::wait( &status );
                
            } else {
                // fork error:
                perror("fork error");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}
#endif
