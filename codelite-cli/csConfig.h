#ifndef CSCONFIG_H
#define CSCONFIG_H

#include <wx/string.h>

class csConfig
{
    wxString m_command;
    wxString m_options;
    size_t m_flags;

public:
    enum eConfigOption {
        kPrettyJSON = (1 << 0),
    };

protected:
    void EnableFlag(eConfigOption flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
    bool HasFlag(eConfigOption flag) const { return m_flags & flag; }

public:
    csConfig();
    virtual ~csConfig();
    void Load();

    void SetCommand(const wxString& command) { this->m_command = command; }
    void SetOptions(const wxString& options) { this->m_options = options; }
    const wxString& GetCommand() const { return m_command; }
    const wxString& GetOptions() const { return m_options; }
    void SetPrettyJSON(bool b) { EnableFlag(kPrettyJSON, b); }
    bool IsPrettyJSON() const { return HasFlag(kPrettyJSON); }
};

#endif // CSCONFIG_H
