#ifndef CODELITE_LLDB_APP_H
#define CODELITE_LLDB_APP_H

#ifndef __WXMSW__

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
#include "LLDBProtocol/LLDBLocalVariable.h"
#include <lldb/API/SBValue.h>
#include <wx/msgqueue.h>
#include "LLDBProtocol/LLDBSettings.h"

class CodeLiteLLDBApp 
{
public:
    typedef void (CodeLiteLLDBApp::*CommandFunc_t)(const LLDBCommand& command);
    typedef std::pair<CodeLiteLLDBApp::CommandFunc_t, LLDBCommand> QueueItem_t;
    
    typedef void (CodeLiteLLDBApp::*NotifyFunc_t)();
    
protected:
    clSocketServer                                  m_acceptSocket;
    LLDBNetworkServerThread *                       m_networkThread;
    LLDBProcessEventHandlerThread*                  m_lldbProcessEventThread;
    lldb::SBDebugger                                m_debugger;
    lldb::SBTarget                                  m_target;
    int                                             m_debuggeePid;
    clSocketBase::Ptr_t                             m_replySocket;
    eInterruptReason                                m_interruptReason;
    std::map<int, lldb::SBValue>                    m_variables;
    wxMessageQueue<CodeLiteLLDBApp::QueueItem_t>    m_commands_queue;
    wxMessageQueue<CodeLiteLLDBApp::NotifyFunc_t>   m_notify_queue;
    wxString                                        m_debuggerSocketPath;
    bool                                            m_exitMainLoop;
    LLDBSettings                                    m_settings;
    
private:
    void Cleanup();
    void AcceptNewConnection() throw(clSocketException);
    
public:
    void NotifyStoppedOnFirstEntry();
    void NotifyStopped();
    void NotifyExited();
    void NotifyStarted();
    void NotifyRunning();
    void NotifyBreakpointsUpdated();
    void NotifyAllBreakpointsDeleted();
    
    void NotifyLocals(LLDBLocalVariable::Vect_t locals);
    
    void SendReply(const LLDBReply& reply);
    bool CanInteract();
    bool IsDebugSessionInProgress();
    
    /**
     * @brief push new handler to the queue
     */
    void CallAfter( CodeLiteLLDBApp::CommandFunc_t func, const LLDBCommand& command );

    void CallAfter( CodeLiteLLDBApp::NotifyFunc_t func ) {
        m_notify_queue.Post( func );
    }
    
    /**
     * @brief start codelite-lldb 
     */
    void MainLoop();
    
public:
    CodeLiteLLDBApp(const wxString &socketPath);
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
    void LocalVariables(const LLDBCommand& command);
    void ExpandVariable(const LLDBCommand& command);
    void SelectFrame(const LLDBCommand& command);
};

DECLARE_APP(CodeLiteLLDBApp)
#endif // !__WXMSW__
#endif // CODELITE_LLDB_APP_H
