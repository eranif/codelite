#ifndef WXTERMINALOPTIONS_H
#define WXTERMINALOPTIONS_H

#include "JSON.h"

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
    wxString m_logfile;
    wxString m_ttyfile;
    // seriliazable fields
    wxFont m_font;
    wxColour m_bgColour;
    wxColour m_textColour;
    wxArrayString m_history;

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

    static wxTerminalOptions& Get();
    wxTerminalOptions& Load();
    wxTerminalOptions& Save();

    wxTerminalOptions& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }

    void SetTtyfile(const wxString& ttyfile) { this->m_ttyfile = ttyfile; }
    const wxString& GetTtyfile() const { return m_ttyfile; }

    void SetBgColour(const wxColour& bgColour) { this->m_bgColour = bgColour; }
    void SetFont(const wxFont& font) { this->m_font = font; }
    void SetTextColour(const wxColour& textColour) { this->m_textColour = textColour; }
    const wxColour& GetBgColour() const { return m_bgColour; }
    const wxFont& GetFont() const { return m_font; }
    const wxColour& GetTextColour() const { return m_textColour; }
    wxTerminalOptions& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }

    void FromJSON(const JSONItem& json);
    JSONItem ToJSON() const;

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
    void SetCommandFromFile(const wxString& command);

    void SetLogfile(const wxString& logfile) { this->m_logfile = logfile; }
    const wxString& GetLogfile() const { return m_logfile; }

    void SetHistory(const wxArrayString& history);
    const wxArrayString& GetHistory() const { return m_history; }
};

#endif // WXTERMINALOPTIONS_H
