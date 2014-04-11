#ifndef CODELITE_LLDB_APP_H
#define CODELITE_LLDB_APP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include "LLDBProtocol/LLDBCommand.h"
#include "LLDBProtocol/clSocketClient.h"
#include "codelite-lldb/LLDBNetworkServerThread.h"
#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBTarget.h>
#include "LLDBProtocol/LLDBEnums.h"
#include "LLDBProcessEventHandlerThread.h"
#include "LLDBProtocol/LLDBReply.h"
#include "LLDBProtocol/cl_socket_server.h"

class CodeLiteLLDBApp : public wxAppConsole
{
    clSocketServer                 m_acceptSocket;
    LLDBNetworkServerThread *      m_networkThread;
    LLDBProcessEventHandlerThread* m_lldbProcessEventThread;
    lldb::SBDebugger               m_debugger;
    lldb::SBTarget                 m_target;
    int                            m_debuggeePid;
    clSocketBase::Ptr_t            m_replySocket;
    eInterruptReason               m_interruptReason;

private:
    void Cleanup();
    void AcceptNewConnection();
    
public:
    void NotifyStoppedOnFirstEntry();
    void NotifyStopped();
    void NotifyExited();
    void NotifyStarted();
    void NotifyRunning();
    void NotifyBreakpointsUpdated();
    void NotifyAllBreakpointsDeleted();

    void SendReply(const LLDBReply& reply);
    bool CanInteract();
    bool IsDebugSessionInProgress();
    
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
    void RunDebugger(const LLDBCommand& command);
    void Continue(const LLDBCommand& command);
    void ApplyBreakpoints(const LLDBCommand& command);
    void StopDebugger(const LLDBCommand& command);
    void DeleteBreakpoints(const LLDBCommand& command);
    void DeleteAllBreakpoints(const LLDBCommand& command);
    void Next(const LLDBCommand& command);
    void StepIn(const LLDBCommand& command);
    void StepOut(const LLDBCommand& command);
    void Interrupt(const LLDBCommand& command);
};

DECLARE_APP(CodeLiteLLDBApp)

#endif // CODELITE_LLDB_APP_H
