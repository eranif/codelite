#pragma once

#include "codelite_exports.h"

#include <wx/any.h>
#include <wx/string.h>

/// A key value with an optional comment associated with it
struct WXDLLIMPEXP_CL clINIKeyValue {
    void SetName(const wxString& name) { this->m_name = name; }
    void SetValue(const wxString& value) { this->m_value = value; }
    const wxString& GetKey() const { return m_name; }
    const wxString& GetValue() const { return m_value; }
    void Clear()
    {
        m_name.clear();
        m_value.clear();
    }

private:
    wxString m_name;
    wxString m_value;
};

/// INI section. A section can have multiple key-value pairs
/// associated with it. A section without a name is the
/// the global section. There can be only one global section
struct WXDLLIMPEXP_CL clINISection {
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
    void Clear() { m_name.clear(); }
    void Serialise(std::ostream& string) const;
    void AddKeyValue(const wxString& key, const wxString& value);
    void AddComment(const wxString& comment);
    clINIKeyValue operator[](const char* key_name) const;
    clINIKeyValue operator[](const wxString& key_name) const;

private:
    wxString m_name;
    std::vector<wxAny> m_entries;
};

class WXDLLIMPEXP_CL clINIParser
{
public:
    clINIParser();
    virtual ~clINIParser();

    bool ParseFile(const wxString& filepath);
    bool ParseString(const wxString& content);
    void Serialise(std::ostream& string) const;

    const clINISection& operator[](const char* section) const;
    const clINISection& operator[](const wxString& section) const;
    bool HasSection(const wxString& section_name) const;

private:
    std::vector<clINISection> m_sections;
};
