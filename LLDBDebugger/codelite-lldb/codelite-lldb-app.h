#ifndef CODELITE_LLDB_APP_H
#define CODELITE_LLDB_APP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include "LLDBProtocol/LLDBCommand.h"
#include "LLDBProtocol/clSocketClient.h"
#include "codelite-lldb/LLDBNetworkServerThread.h"
#include <wx/socket.h>

#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBTarget.h>

class CodeLiteLLDBApp : public wxAppConsole
{
    LLDBNetworkServerThread *m_networkThread;
    
    lldb::SBDebugger m_debugger;
    lldb::SBTarget   m_target;
    int m_debuggeePid;
    clSocketBase::Ptr_t m_replySocket;
    
public:
    CodeLiteLLDBApp();
    virtual ~CodeLiteLLDBApp();
    
    /**
     * @brief intialize the application
     */
    virtual bool OnInit();
    /**
     * @brief perform cleanup before exiting
     */
    virtual int OnExit();
    
    // callback from the network thread
    void StartDebugger(const LLDBCommand& command);
};

DECLARE_APP(CodeLiteLLDBApp)

#endif // CODELITE_LLDB_APP_H
