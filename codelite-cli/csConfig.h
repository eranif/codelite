#ifndef CSCONFIG_H
#define CSCONFIG_H

#include <wx/string.h>

class csConfig
{
    wxString m_command;
    wxString m_options;

public:
    csConfig();
    virtual ~csConfig();
    void Load();

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetOptions(const wxString& options) { this->m_options = options; }
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetOptions() const { return m_options; }
};

#endif // CSCONFIG_H
