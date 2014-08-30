#ifndef XDEBUGBREAKPOINT_H
#define XDEBUGBREAKPOINT_H

#include <wx/string.h>
#include <list>
#include <json_node.h>

class XDebugBreakpoint
{
    wxString m_fileName;
    int      m_line;
    int      m_breakpointId;

public:
    typedef std::list<XDebugBreakpoint> List_t;
    
    /// A "Clear Id" for breakpoint functor
    struct ClearIdFunctor {
        void operator()(XDebugBreakpoint& bp) {
            bp.SetBreakpointId(wxNOT_FOUND);
        }
    };
    
    // Predicate class to be used with std::find_if
    struct Equal
    {
        wxString m_fileName;
        int      m_line;
    public:
        Equal(const wxString &filename, int line) : m_fileName(filename), m_line(line) {}
        bool operator()(const XDebugBreakpoint& bp) const {
            return m_fileName == bp.GetFileName() && m_line == bp.GetLine();
        }
    };

public:
    XDebugBreakpoint();
    XDebugBreakpoint(const wxString &filename, int line);
    virtual ~XDebugBreakpoint();
    bool operator==(const XDebugBreakpoint& other) const;

    bool IsApplied() const {
        return m_breakpointId != wxNOT_FOUND;
    }
    
    bool IsNull() const {
        return m_line == wxNOT_FOUND;
    }
    
    void SetBreakpointId(int breakpointId) {
        this->m_breakpointId = breakpointId;
    }
    int GetBreakpointId() const {
        return m_breakpointId;
    }
    void SetFileName(const wxString& fileName) {
        this->m_fileName = fileName;
    }
    void SetLine(int line) {
        this->m_line = line;
    }
    const wxString& GetFileName() const {
        return m_fileName;
    }
    int GetLine() const {
        return m_line;
    }
    
    /// Serialization
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& json);
};

#endif // XDEBUGBREAKPOINT_H
