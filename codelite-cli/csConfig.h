#ifndef CSCONFIG_H
#define CSCONFIG_H

#include <wx/string.h>

class csConfig
{
    wxString m_connectionString;

public:
    csConfig();
    virtual ~csConfig();
    
    void Load();
    void SetConnectionString(const wxString& connectionString) { this->m_connectionString = connectionString; }
    const wxString& GetConnectionString() const { return m_connectionString; }
};

#endif // CSCONFIG_H
