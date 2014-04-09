#include "codelite-lldb-app.h"
#include <iostream>

IMPLEMENT_APP_CONSOLE(CodeLiteLLDBApp)

static void PrintUsage()
{
    std::cout << "Usage: codelite-lldb debug </path/to/exe>" << std::endl;
    std::cout << "       codelite-lldb attach <process-id>" << std::endl;
    std::cout << "       codelite-lldb debug_core </path/to/core>" << std::endl;
}

CodeLiteLLDBApp::CodeLiteLLDBApp()
{
}

CodeLiteLLDBApp::~CodeLiteLLDBApp()
{
}

int CodeLiteLLDBApp::OnExit()
{
    return TRUE;
}

bool CodeLiteLLDBApp::OnInit()
{
    if ( argc < 3 ) {
        PrintUsage();
        return false;
    }
    
    // Validate the command line arguments
    wxString command = argv[1];
    wxString command_value = argv[2];
    if ( command != "debug" && command != "attach" && command != "debug_core" ) {
        PrintUsage();
        return false;
    }
    
    if ( command == "debug" ) {
        DebugProcess( command_value );
    }
    // place your initialization code here
    return true;
}

void CodeLiteLLDBApp::DebugProcess(const wxString& exePath)
{
}
