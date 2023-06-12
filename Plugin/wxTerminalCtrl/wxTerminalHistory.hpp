#ifndef WXTERMINALHISTORY_HPP
#define WXTERMINALHISTORY_HPP

#include "codelite_exports.h"

#include <wx/arrstr.h>

struct WXDLLIMPEXP_SDK wxTerminalHistory {
    wxArrayString m_commands;
    int m_current = wxNOT_FOUND;
    
    void Add(const wxString& command)
    {
        m_commands.Insert(command, 0);
        m_current = wxNOT_FOUND;
    }

    void Up()
    {
        if(m_commands.empty()) {
            return;
        }
        ++m_current;
        if(m_current >= (int)m_commands.size()) {
            m_current = (m_commands.size() - 1);
        }
    }

    void Down()
    {
        if(m_commands.empty()) {
            return;
        }
        --m_current;
        if(m_current < 0) {
            m_current = 0;
        }
    }

    wxString Get() const
    {
        if(m_current < 0 || m_current >= (int)m_commands.size()) {
            return "";
        }
        return m_commands.Item(m_current);
    }

    void Clear()
    {
        m_current = wxNOT_FOUND;
        m_commands.clear();
    }

    const wxArrayString& GetCommands() const { return m_commands; }
    void SetCommands(const wxArrayString& commands)
    {
        m_commands = commands;
        m_current = wxNOT_FOUND;
    }
};

#endif // WXTERMINALHISTORY_HPP
