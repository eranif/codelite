#ifndef UIBREAKPOINT_HPP
#define UIBREAKPOINT_HPP

#include "JSON.h"
#include "codelite_exports.h"

#include <functional>
#include <wx/string.h>

enum class UIBreakpointType {
    INVALID = -1,
    FUNCTION,
    SOURCE,
};

class WXDLLIMPEXP_CL UIBreakpoint
{

protected:
    UIBreakpointType m_type = UIBreakpointType::INVALID;
    wxString m_file;
    int m_line = wxNOT_FOUND;
    wxString m_function;

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
    bool operator==(const UIBreakpoint& other) const { return SameAs(other); }

    wxString GetKey() const { return (wxString() << GetFile() << GetLine() << GetFunction()); }

    // aliases
    bool IsFunctionBreakpoint() const { return GetType() == UIBreakpointType::FUNCTION; }
    bool IsSourceBreakpoint() const { return GetType() == UIBreakpointType::SOURCE; }

    JSONItem To() const;
    void From(const JSONItem& json);
};

// provide custom hash so we will be able to use UIBreakpoint with unordered_map/unordered_set et al
namespace std
{
template <> struct hash<UIBreakpoint> {
    std::size_t operator()(const UIBreakpoint& b) const { return hash<std::wstring>{}(b.GetKey().ToStdWstring()); }
};
} // namespace std
#endif // UIBREAKPOINT_HPP
