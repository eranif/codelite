#ifndef WXTERMINALCOLOURHANDLER_H
#define WXTERMINALCOLOURHANDLER_H

#include <wx/textctrl.h>
#include <unordered_map>
#include <map>

enum class eColourHandlerState {
    kNormal = 0,
    kInEscape, // found ESC char
    kInOsc,    // Operating System Command
    kInCsi,    // Control Sequence Introducer
};

class wxTerminalColourHandler
{
    wxTextCtrl* m_ctrl = nullptr;
    wxString m_escapeSequence;
    std::unordered_map<int, wxColour> m_colours;
    wxTextAttr m_defaultAttr;

protected:
    eColourHandlerState m_state = eColourHandlerState::kNormal;
    wxColour GetColour(const wxString& colour);

protected:
    void Append(const wxString& buffer);
    void SetStyleFromEscape(const wxString& escape);
    void Clear();

public:
    wxTerminalColourHandler();
    wxTerminalColourHandler(wxTextCtrl* ctrl);
    ~wxTerminalColourHandler();

    wxTerminalColourHandler& operator<<(const wxString& buffer);
    void SetCtrl(wxTextCtrl* ctrl);
};

#endif // WXTERMINALCOLOURHANDLER_H
