//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CodeLiteLLDB.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <wx/init.h>
#include "CodeLiteLLDBApp.h"
#include <unistd.h>
#ifndef __WXMSW__
#include <sys/wait.h>
#endif
#include <lldb/API/SBDebugger.h>
#include <wx/filename.h>
#include <wx/wxcrtvararg.h>

static wxString s_localSocket;
void OnTerminate(int signo)
{
    // Remove the socket path
    if ( !s_localSocket.IsEmpty()) {
        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        exit(0);
        
    } else {
        // Kill our child process and exit
        wxPrintf("codelite-lldb: bye\n");
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
#ifndef __WXMSW__
    signal(SIGPIPE, SIG_IGN);
#endif
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
    
    // Initialize SBDebugger
    lldb::SBDebugger::Initialize();
    
    if ( !s_localSocket.IsEmpty() ) {
        // Start on local socket
        CodeLiteLLDBApp app(s_localSocket);
        app.MainLoop();

        wxPrintf("codelite-lldb: removing socket file '%s'\n", s_localSocket);
        ::remove( s_localSocket.mb_str(wxConvUTF8).data() );
        wxPrintf("codelite-lldb: bye\n");
        
    } else if ( !tcpConnectString.IsEmpty() ) {
        // TCP/IP mode, redirect stdout/err
        // wxFileName stdout_err(::wxGetHomeDir(), ".codelite-lldb-stdout-stderr.log");
        // FILE* new_stdout = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "w+b", stdout);
        // FILE* new_stderr = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "w+b", stderr);
        // wxUnusedVar(new_stderr);
        // wxUnusedVar(new_stdout);

        // parse the connect string
        long portNum;
        wxString ip_addr    = tcpConnectString.BeforeFirst(':');
        wxString port_str   = tcpConnectString.AfterFirst(':');
        port_str.ToCLong( &portNum );
        
        wxString invocationLine;
        invocationLine << argv[0] << " -t " << tcpConnectString << "'";
        
        // Connect using tcp-ip
        // When using tcp-ip we fork the child process and wait until it terminates
        // Child code
        {
            // this must be in a scope so that 'app' will get destroyed before we create new instance
            CodeLiteLLDBApp app(ip_addr, portNum);
            app.MainLoop();
        }

        // Restart ourslef ( lldb does not clear nicely :( )
        ::wxExecute(invocationLine);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

