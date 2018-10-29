#ifndef NODEJSDEBUGGERBASE_H
#define NODEJSDEBUGGERBASE_H

#include "NodeJS.h"
#include "NodeJSDebuggerBreakpointManager.h"
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

class NodeJSDebuggerBase : public wxEvtHandler
{
public:
    typedef wxSharedPtr<NodeJSDebuggerBase> Ptr_t;

protected:
    NodeJSBptManager m_bptManager;

public:
    NodeJSDebuggerBase();
    virtual ~NodeJSDebuggerBase();
    template <typename T> T* Cast() { return dynamic_cast<T*>(this); }

    NodeJSBptManager* GetBreakpointsMgr() { return &m_bptManager; }

    /**
     * @brief start the debugger using the given command
     */
    virtual void StartDebugger(const wxString& command, const wxString& workingDirectory) = 0;
    /**
     * @brief lookup handles
     */
    virtual void Lookup(const std::vector<int>& handles, eNodeJSContext context) {}
    /**
     * @brief select the desired frame
     */
    virtual void SelectFrame(int index) {}
    
    /**
     * @brief trigger a "callstack" request
     */
    virtual void Callstack() {}
};

#endif // NODEJSDEBUGGERBASE_H
