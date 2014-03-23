#ifndef LLDBDEBUGGERTHREAD_H
#define LLDBDEBUGGERTHREAD_H

#include <wx/thread.h>
#include <list>
#include <wx/string.h>

#if defined(__APPLE__)
#   include <LLDB/LLDB.h>
#else
#   include "lldb/API/SBListener.h"
#   include "lldb/API/SBEvent.h"
#   include "lldb/API/SBProcess.h"
#endif

class LLDBDebugger;
class LLDBDebuggerThread : public wxThread
{
    friend class LLDBDebugger;
    
    LLDBDebugger*    m_owner;
    lldb::SBListener m_listener;
    lldb::SBProcess  m_process;
    
public:
    LLDBDebuggerThread(LLDBDebugger* owner, lldb::SBListener listener, lldb::SBProcess process);
    virtual ~LLDBDebuggerThread();

    void Start() {
        Create();
        Run();
    }
    
    virtual void* Entry();
};

#endif // LLDBDEBUGGERTHREAD_H
