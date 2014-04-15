#ifndef TERMINALOPTIONS_H
#define TERMINALOPTIONS_H

#include <wx/string.h>

class TerminalOptions
{
public:
    enum {
        kPauseBeforeExit            = 0x00000001,
        kExitWhenInfiriorTerminates = 0x00000002,
        kPrintInfo                  = 0x00000004,
        kAlwaysOnTop                = 0x00000008,
        kDebuggerTerminal           = 0x00000010,
    };

protected:
    wxString m_command;
    size_t   m_flags;
    wxString m_title;

public:
    TerminalOptions();
    virtual ~TerminalOptions();

    bool HasFlag( int flag ) const {
        return m_flags & flag;
    }
    void EnableFlag( int flag, bool b ) {
        if ( b ) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    void SetCommand(const wxString& command) {
        this->m_command = command;
    }
    size_t GetFlags() const {
        return m_flags;
    }
    const wxString& GetCommand() const {
        return m_command;
    }

    void SetTitle(const wxString& title) {
        this->m_title = title;
    }
    const wxString& GetTitle() const {
        return m_title;
    }
};

#endif // TERMINALOPTIONS_H
