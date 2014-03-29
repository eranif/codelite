#ifndef LLDBBACKTRACE_H
#define LLDBBACKTRACE_H

#include <wx/string.h>
#include <vector>

#if defined(__APPLE__)
#   include <LLDB/LLDB.h>
#else
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
#endif

/**
 * @class LLDBBacktrace
 * Construct a human readable backtrace from lldb::SBThread object
 */
class LLDBBacktrace
{
public:
    struct Entry {
        int      id;
        int      line;
        wxString filename;
        wxString functionName;
        wxString address;
        
        Entry() : id(0), line(0) {}
    };
    typedef std::vector<LLDBBacktrace::Entry> EntryVec_t;

protected:
    int m_threadId;
    LLDBBacktrace::EntryVec_t m_callstack;

public:
    LLDBBacktrace(lldb::SBThread &thread);
    LLDBBacktrace() : m_threadId (0) {}
    virtual ~LLDBBacktrace();
    void SetCallstack(const LLDBBacktrace::EntryVec_t& callstack) {
        this->m_callstack = callstack;
    }
    void SetThreadId(int threadId) {
        this->m_threadId = threadId;
    }
    const LLDBBacktrace::EntryVec_t& GetCallstack() const {
        return m_callstack;
    }
    int GetThreadId() const {
        return m_threadId;
    }
    
    wxString ToString() const;
};

#endif // LLDBBACKTRACE_H
