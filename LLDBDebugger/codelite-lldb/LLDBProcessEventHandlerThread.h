#ifndef LLDBHANDLERTHREAD_H
#define LLDBHANDLERTHREAD_H

#ifndef __WXMSW__

#include <wx/thread.h>
#include <lldb/API/SBListener.h>
#include <lldb/API/SBProcess.h>

/**
 * @class LLDBHandlerThread
 * @author eran
 * @brief Wait for the process events and report any changes to the application
 */
class CodeLiteLLDBApp;
class LLDBProcessEventHandlerThread : public wxThread
{
    CodeLiteLLDBApp* m_app;
    lldb::SBProcess  m_process;
    lldb::SBListener m_listener;
    
public:
    LLDBProcessEventHandlerThread(CodeLiteLLDBApp* app, lldb::SBListener listener, lldb::SBProcess process);
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
