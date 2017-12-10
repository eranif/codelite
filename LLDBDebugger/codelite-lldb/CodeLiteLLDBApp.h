//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CodeLiteLLDBApp.h
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

#ifndef CODELITE_LLDB_APP_H
#define CODELITE_LLDB_APP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include "LLDBProtocol/LLDBCommand.h"
#include "SocketAPI/clSocketClient.h"
#include "codelite-lldb/LLDBNetworkServerThread.h"
#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBTarget.h>
#include "LLDBProtocol/LLDBEnums.h"
#include "LLDBProcessEventHandlerThread.h"
#include "LLDBProtocol/LLDBReply.h"
#include "SocketAPI/clSocketServer.h"
#include "LLDBProtocol/LLDBVariable.h"
#include <lldb/API/SBValue.h>
#include <wx/msgqueue.h>
#include "LLDBProtocol/LLDBSettings.h"

struct VariableWrapper
{
    lldb::SBValue value;
    bool isWatch;
    wxString expression;
    
    VariableWrapper() : isWatch(false) {}
};

class CodeLiteLLDBApp
{
public:
    typedef void (CodeLiteLLDBApp::*CommandFunc_t)(const LLDBCommand& command);
    typedef std::pair<CodeLiteLLDBApp::CommandFunc_t, LLDBCommand> QueueItem_t;

    typedef void (CodeLiteLLDBApp::*NotifyFunc_t)();

protected:
    clSocketServer m_acceptSocket;
    LLDBNetworkServerThread* m_networkThread;
    LLDBProcessEventHandlerThread* m_lldbProcessEventThread;
    lldb::SBDebugger m_debugger;
    lldb::SBTarget m_target;
    int m_debuggeePid;
    clSocketBase::Ptr_t m_replySocket;
    eInterruptReason m_interruptReason;
    std::map<int, VariableWrapper> m_variables;
    wxArrayString m_watches;
    wxMessageQueue<CodeLiteLLDBApp::QueueItem_t> m_commands_queue;
    wxMessageQueue<CodeLiteLLDBApp::NotifyFunc_t> m_notify_queue;
    wxString m_debuggerSocketPath;
    bool m_exitMainLoop;
    LLDBSettings m_settings;
    eLLDBDebugSessionType m_sessionType;
    wxString m_ip;
    int m_port;

private:
    void Cleanup();
    void AcceptNewConnection() ;
    bool InitializeLLDB(const LLDBCommand& command);
    void DoInitializeApp();
    void DoExecutueShellCommand(const wxString& command, bool printOutput = true);

public:
    void NotifyStoppedOnFirstEntry();
    void NotifyStopped();
    void NotifyExited();
    void NotifyAborted();
    void NotifyStarted(eLLDBDebugSessionType sessionType);
    void NotifyRunning();
    void NotifyLaunchSuccess();
    void NotifyBreakpointsUpdated();
    void NotifyAllBreakpointsDeleted();

    void NotifyLocals(LLDBVariable::Vect_t locals);

    void SendReply(const LLDBReply& reply);
    bool CanInteract();
    bool IsDebugSessionInProgress();

    /**
     * @brief push new handler to the queue
     */
    void CallAfter(CodeLiteLLDBApp::CommandFunc_t func, const LLDBCommand& command);

    void CallAfter(CodeLiteLLDBApp::NotifyFunc_t func) { m_notify_queue.Post(func); }

    /**
     * @brief start codelite-lldb
     */
    void MainLoop();

public:
    CodeLiteLLDBApp(const wxString& socketPath);
    CodeLiteLLDBApp(const wxString& ip, int port);
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
    void DetachDebugger(const LLDBCommand& command);
    void DeleteBreakpoints(const LLDBCommand& command);
    void DeleteAllBreakpoints(const LLDBCommand& command);
    void Next(const LLDBCommand& command);
    void NextInstruction(const LLDBCommand& command);
    void AddWatch(const LLDBCommand& command);
    void ExecuteInterperterCommand(const LLDBCommand& command);
    void DeleteWatch(const LLDBCommand& command);
    void StepIn(const LLDBCommand& command);
    void StepOut(const LLDBCommand& command);
    void Interrupt(const LLDBCommand& command);
    void LocalVariables(const LLDBCommand& command);
    void ExpandVariable(const LLDBCommand& command);
    void SelectFrame(const LLDBCommand& command);
    void SelectThread(const LLDBCommand& command);
    void EvalExpression(const LLDBCommand& command);
    void OpenCoreFile(const LLDBCommand& command);
    void AttachProcess(const LLDBCommand& command);
    void ShowCurrentFileLine(const LLDBCommand& command);
};

DECLARE_APP(CodeLiteLLDBApp)
#endif // CODELITE_LLDB_APP_H
