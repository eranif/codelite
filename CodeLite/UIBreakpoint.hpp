#ifndef UIBREAKPOINT_HPP
#define UIBREAKPOINT_HPP

#include "codelite_exports.h"

#include <wx/string.h>

enum class UIBreakpointType {
    INVALID = -1,
    FUNCTION,
    SOURCE,
};

class WXDLLIMPEXP_CL UIBreakpoint
{

protected:
    wxString m_file;
    wxString m_function;
    int m_line;
    UIBreakpointType m_type = UIBreakpointType::INVALID;

public:
    UIBreakpoint();
    ~UIBreakpoint();

    void SetFile(const wxString& file) { this->m_file = file; }
    void SetFunction(const wxString& function) { this->m_function = function; }
    void SetLine(int line) { this->m_line = line; }
    void SetType(UIBreakpointType type) { this->m_type = type; }
    const wxString& GetFile() const { return m_file; }
    const wxString& GetFunction() const { return m_function; }
    int GetLine() const { return m_line; }
    UIBreakpointType GetType() const { return m_type; }

    bool SameAs(const UIBreakpoint& other) const;

    // aliases
    bool IsFunctionBreakpoint() const { return GetType() == UIBreakpointType::FUNCTION; }
    bool IsSourceBreakpoint() const { return GetType() == UIBreakpointType::SOURCE; }
};

#endif // UIBREAKPOINT_HPP
