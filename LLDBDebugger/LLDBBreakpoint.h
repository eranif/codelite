#ifndef LLDBBREAKPOINT_H
#define LLDBBREAKPOINT_H

#include <wx/string.h>
#include <vector>
#include <wx/filename.h>
#include "debugger.h"
#include "json_node.h"

class LLDBBreakpoint
{
public:
    // Breakpoint type
    enum {
        kInvalid = -1,
        kFileLine,
        kFunction
    };
    typedef std::vector<LLDBBreakpoint> Vec_t;
    
protected:
    int      m_id;
    int      m_type;
    wxString m_name;
    wxString m_filename;
    int      m_lineNumber;
    
public:
    class Equal {
        const LLDBBreakpoint& m_src;
    public:
        Equal(const LLDBBreakpoint& src) : m_src(src) {}
        bool operator()(const LLDBBreakpoint& other) const {
            return m_src == other;
        }
    };
    
    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& element);
    
public:
    LLDBBreakpoint() : m_id(wxNOT_FOUND), m_type(kInvalid), m_lineNumber(wxNOT_FOUND) {}
    LLDBBreakpoint(const wxString &name);
    LLDBBreakpoint(const wxFileName& filename, int line);
    LLDBBreakpoint(const LLDBBreakpoint& other);
    bool operator==(const LLDBBreakpoint& other) const;
    LLDBBreakpoint& operator=(const LLDBBreakpoint& other);
    virtual ~LLDBBreakpoint();
    
    /**
     * @brief convert list of gdb breakpoints into LLDBBreakpoint vector
     */
    static BreakpointInfo::Vec_t ToBreakpointInfoVector(const LLDBBreakpoint::Vec_t& breakpoints);
    /**
     * @brief convert list of lldb breakpoints into gdb's breakpoint list
     * @param breakpoints
     */
    static LLDBBreakpoint::Vec_t FromBreakpointInfoVector(const BreakpointInfo::Vec_t& breakpoints);
    
    /**
     * @brief return a string representation for this breakpoint
     */
    wxString ToString() const;
    
    bool IsApplied() const {
        return m_id != wxNOT_FOUND;
    }
    
    void Invalidate() {
        m_id = wxNOT_FOUND;
    }
    
    void SetFilename(const wxString& filename) {
        this->m_filename = filename;
    }
    void SetLineNumber(int lineNumber) {
        this->m_lineNumber = lineNumber;
    }
    void SetName(const wxString& name) {
        this->m_name = name;
    }
    void SetType(int type) {
        this->m_type = type;
    }
    const wxString& GetFilename() const {
        return m_filename;
    }
    int GetLineNumber() const {
        return m_lineNumber;
    }
    const wxString& GetName() const {
        return m_name;
    }
    int GetType() const {
        return m_type;
    }
    void SetId(int id) {
        this->m_id = id;
    }
    int GetId() const {
        return m_id;
    }
    
    // ---------------------------------------
    // serialization
    // ---------------------------------------
    static wxFileName GetBreakpointsConfigFile();
    
    /**
     * @brief load breakpoints from the file system.
     * The path searched is always the workspace-path/.codelite/lldb-breakpoints.conf
     * If no workspace is opened, return empty array
     */
    static LLDBBreakpoint::Vec_t LoadFromDisk();
    /**
     * @brief save breakpoints to the file system. If no workspace is opened, this function
     * does nothing
     */
    static void SaveToDisk(const LLDBBreakpoint::Vec_t& breakpoints);
};

#endif // LLDBBREAKPOINT_H
