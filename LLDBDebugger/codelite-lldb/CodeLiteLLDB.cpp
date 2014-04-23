#ifndef __WXMSW__

#include <wx/init.h>
#include "CodeLiteLLDBApp.h"
#include <unistd.h>

static wxString s_localSocket;
void OnTerminate(int signo)
{
    // Remove the socket path
    if ( !s_localSocket.IsEmpty()) {
        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        exit(0);
    }
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
    while ((c = getopt (argc, argv, "s:t:")) != -1) {
        switch(c) {
        case 's':
            s_localSocket = optarg;
            break;

        case 't':
            tcpConnectString = optarg;
            break;

        default:
            break;
        }
    }

    if (s_localSocket.IsEmpty() && tcpConnectString.IsEmpty() ) {
        wxPrintf("codelite-lldb: error: usage: codelite-lldb -s <unique-string>\n");
        exit(1);
    }

    if ( !s_localSocket.IsEmpty() ) {
        // Start on local socket
        CodeLiteLLDBApp app(s_localSocket);
        app.MainLoop();

        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        
    } else if ( !tcpConnectString.IsEmpty() ) {
        // Connect using tcp-ip
        // When using tcp-ip we fork the child process and wait until it terminates
    }
    return 0;
}
#endif
