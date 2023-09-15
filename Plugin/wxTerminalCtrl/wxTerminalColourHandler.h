#ifndef WXTERMINALCOLOURHANDLER_H
#define WXTERMINALCOLOURHANDLER_H

#include "clAnsiEscapeCodeHandler.hpp"

#include <map>
#include <unordered_map>
#include <vector>
#include <wx/textctrl.h>

class wxTerminalOutputCtrl;
class wxTerminalCtrl;
class wxTerminalColourHandler : public wxEvtHandler
{
    wxTerminalOutputCtrl* m_ctrl = nullptr;
    wxSTCStyleProvider* m_style_provider = nullptr;
    wxTextAttr m_defaultAttr;
    wxString m_title;
    clAnsiEscapeCodeHandler m_ansiEscapeHandler;

protected:
    void SetCaretEnd();

public:
    wxTerminalColourHandler();
    ~wxTerminalColourHandler();

    void Append(const wxString& buffer, wxString* window_title);
    void SetCtrl(wxTerminalOutputCtrl* ctrl);
    void SetDefaultStyle(const wxTextAttr& attr);
    void Clear();
};

#endif // WXTERMINALCOLOURHANDLER_H
