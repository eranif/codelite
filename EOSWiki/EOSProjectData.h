#ifndef EOSPROJECTDATA_H
#define EOSPROJECTDATA_H

#include <wx/string.h>

class EOSProjectData
{
    wxString m_toolchainPath;
    wxString m_name;
    wxString m_path;

public:
    EOSProjectData();
    virtual ~EOSProjectData();
    
    void SetName(const wxString& name) { this->m_name = name; }
    void SetPath(const wxString& path) { this->m_path = path; }
    void SetToolchainPath(const wxString& toolchainPath) { this->m_toolchainPath = toolchainPath; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetPath() const { return m_path; }
    const wxString& GetToolchainPath() const { return m_toolchainPath; }
};

#endif // EOSPROJECTDATA_H
