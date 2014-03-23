#ifndef LLDBBREAKPOINT_H
#define LLDBBREAKPOINT_H

#include <wx/string.h>
#include <vector>
#include <wx/filename.h>

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
    
public:
    LLDBBreakpoint() : m_id(wxNOT_FOUND), m_type(kInvalid), m_lineNumber(wxNOT_FOUND) {}
    LLDBBreakpoint(const wxString &name);
    LLDBBreakpoint(const wxFileName& filename, int line);
    bool operator==(const LLDBBreakpoint& other) const;
    virtual ~LLDBBreakpoint();
    
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
};

#endif // LLDBBREAKPOINT_H
