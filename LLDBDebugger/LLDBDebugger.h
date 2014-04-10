#ifndef LLDBDEBUGGER_H
#define LLDBDEBUGGER_H

#include <wx/string.h>
#include <wx/sharedptr.h>
#include <vector>
#include <wx/event.h>
#include <wx/timer.h>
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "LLDBLocalVariable.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "LLDBProtocol/LLDBEnums.h"

// LLDB headers
#   include "lldb/API/SBBlock.h"
#   include "lldb/API/SBCompileUnit.h"
#   include "lldb/API/SBDebugger.h"
#   include "lldb/API/SBFunction.h"
#   include "lldb/API/SBModule.h"
#   include "lldb/API/SBStream.h"
#   include "lldb/API/SBSymbol.h"
#   include "lldb/API/SBTarget.h"
#   include "lldb/API/SBThread.h"
#   include "lldb/API/SBProcess.h"
#   include "lldb/API/SBBreakpoint.h"
#   include "lldb/API/SBListener.h"

class LLDBDebuggerThread;
class LLDBDebugger : public wxEvtHandler
{
public:
    friend class LLDBDebuggerThread;

protected:
    lldb::SBDebugger m_debugger;
    lldb::SBTarget   m_target;
    wxString         m_tty;
    LLDBBreakpoint::Vec_t m_breakpoints;
    LLDBBreakpoint::Vec_t m_pendingDeletionBps;
    LLDBDebuggerThread *m_thread;
    int                 m_debugeePid;
    eInterruptReason    m_interruptReason;
    bool                m_canInteract;

protected:
    LLDBBreakpoint::Vec_t::iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp);
    LLDBBreakpoint::Vec_t::const_iterator FindBreakpoint(LLDBBreakpoint::Ptr_t bp) const;

    void NotifyBacktrace();
    void NotifyStopped();
    void NotifyExited();
    void NotifyStarted();
    void NotifyStoppedOnFirstEntry();
    void NotifyRunning();
    void NotifyBreakpointsUpdated();
    void NotifyAllBreakpointsDeleted();
    void NotifyFrameSelected(int frameId = 0);
    
    void Cleanup();
    bool IsValid() const;
    void DoAddBreakpoint(LLDBBreakpoint::Ptr_t);
    void DoDeletePendingDeletionBreakpoints();
    
public:
    LLDBDebugger();
    virtual ~LLDBDebugger();

    bool IsBreakpointExists(LLDBBreakpoint::Ptr_t bp) const;
    
    /**
     * @brief clear all states/cached breakpoints etc
     */
    void Clear();
    
    int GetDebugeePid() const {
        return m_debugeePid;
    }
    void SetTty(const wxString& tty) {
        this->m_tty = tty;
    }
    const wxString& GetTty() const {
        return m_tty;
    }

    bool CanInteract() const {
        return m_canInteract;
    }
    /**
     * @brief return list of all breakpoints
     */
    const LLDBBreakpoint::Vec_t& GetBreakpoints() const {
        return m_breakpoints;
    }

    static void Initialize();
    static void Terminate();

    /**
     * @brief create debugger for executable
     */
    bool Start(const wxString &filename);

    /**
     * @brief issue a "run" command
     */
    bool Run(const wxString &in,
             const wxString& out,
             const wxString &err,
             const wxArrayString& argvArr,
             const wxArrayString& envArr,
             const wxString& workingDirectory);

    /**
     * @brief stop the debugger
     */
    void Stop(bool notifyExit);
    
    /**
     * @brief resume execution until exit or next breakpoint is hit (or crash...)
     */
    bool Continue();

    /**
     * @brief perform a "step-over" command (gdb's "next" command)
     */
    bool StepOver();

    /**
     * @brief step into a function
     */
    bool StepIn();

    /**
     * @brief Execute until selected stack frame returns
     */
    bool Finish();

    /**
     * @brief add a breakpoint ( e.g. break file.cpp:12 )
     * Note that this function does not apply the breakpoints, it only keeps them in memory
     */
    void AddBreakpoint(const wxFileName& filename, int line);

    /**
     * @brief add list of breakpoints
     */
    void AddBreakpoints(const BreakpointInfo::Vec_t& breakpoints);

    /**
     * @brief add list of breakpoints
     */
    void AddBreakpoints(const LLDBBreakpoint::Vec_t& breakpoints);

    /**
     * @brief add a named breakpoint ( e.g. break main )
     * * Note that this function does not apply the breakpoints, it only keeps them in memory
     */
    void AddBreakpoint(const wxString& name);

    /**
     * @brief apply all breakpoints previously added with "AddBreakpoint" API calls
     * This function will only try to apply breakpoints with invalid breakpoint ID
     */
    void ApplyBreakpoints();

    /**
     * @brief loop over all the breakpoints and invalidate all of them (i.e. mark them as non-applied)
     */
    void InvalidateBreakpoints();

    /**
     * @brief delete a breakpoint. The breakpoint is removed from the debugger and from the list
     */
    void DeleteBreakpoint(LLDBBreakpoint::Ptr_t bp);
    
    /**
     * @brief delete an array of breakpoints
     */
    void DeleteBreakpoints(const LLDBBreakpoint::Vec_t& bps);
    
    /**
     * @brief remove all breakpoints that are pending deletion
     */
    void DeletePendingDeletionBreakpoints();
    
    /**
     * @brief delete all breakpoints
     */
    void DeleteAllBreakpoints();

    /**
     * @brief interrupt the inferior process
     */
    void Interrupt(eInterruptReason reason);
    
    /**
     * @brief select a frame by ID
     */
    void SelectFrame(int frameId);
    
    /**
     * @brief return list of local variables for the selected frame
     */
    LLDBLocalVariable::Vect_t GetLocalVariables();
};

#endif // LLDBDEBUGGER_H
