#ifndef WXTERMINALOPTIONS_H
#define WXTERMINALOPTIONS_H

#include <wx/string.h>

enum eTerminalOptions {
    kPrintTTY = (1 << 0),
    kPause = (1 << 1),
};

class wxTerminalOptions
{
    size_t m_flags = 0;
    wxString m_workingDirectory;
    wxString m_title;
    wxString m_command;

protected:
    void EnableFlag(bool b, eTerminalOptions flag)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

public:
    wxTerminalOptions();
    ~wxTerminalOptions();
    wxTerminalOptions& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    wxTerminalOptions& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }

    size_t GetFlags() const { return m_flags; }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }

    void SetPrintTTY(bool b) { EnableFlag(b, kPrintTTY); }
    bool IsPrintTTY() const { return m_flags & kPrintTTY; }

    void SetWaitOnExit(bool b) { EnableFlag(b, kPause); }
    bool IsWaitOnExit() const { return m_flags & kPause; }

    void SetTitle(const wxString& title) { this->m_title = title; }
    const wxString& GetTitle() const { return m_title; }

    void SetCommand(const wxString& command) { this->m_command = command; }
    const wxString& GetCommand() const { return m_command; }
};

#endif // WXTERMINALOPTIONS_H
