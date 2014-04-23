#ifndef LLDBHANDLERTHREAD_H
#define LLDBHANDLERTHREAD_H

#ifndef __WXMSW__

#include <wx/thread.h>
#include <lldb/API/SBListener.h>
#include <lldb/API/SBProcess.h>
#include "LLDBProtocol/LLDBEnums.h"


/**
 * @class LLDBHandlerThread
 * @author eran
 * @brief Wait for the process events and report any changes to the application
 */
class CodeLiteLLDBApp;
class LLDBProcessEventHandlerThread : public wxThread
{
    CodeLiteLLDBApp*      m_app;
    lldb::SBProcess       m_process;
    lldb::SBDebugger&     m_debugger;
    lldb::SBListener      m_listener;
    eLLDBDebugSessionType m_sessionType;

public:
    LLDBProcessEventHandlerThread(CodeLiteLLDBApp* app, 
                                  lldb::SBDebugger& debugger,
                                  lldb::SBProcess process, 
                                  eLLDBDebugSessionType sessionType);
    virtual ~LLDBProcessEventHandlerThread();
    
    void Start() {
        Create();
        Run();
    }

protected:
    virtual void* Entry();
};
#endif // !__WXMSW__
#endif // LLDBHANDLERTHREAD_H
