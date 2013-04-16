#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <wx/event.h> // Base class: wxEvtHandler
#include <wx/stc/stc.h>
#include "plugin.h"

class ThemeHandler : public wxEvtHandler
{
    void DoUpdateColours( wxWindow* win, const wxColour& bg, const wxColour &fg );
    void DoUpdateAuiToolBars( wxWindow* win );
    void DoUpdateSTCBgColour(wxStyledTextCtrl *stc);
    
public:
    ThemeHandler();
    virtual ~ThemeHandler();
    
    void OnEditorThemeChanged(wxCommandEvent &e);
    void OnInitDone(wxCommandEvent &e);
};

#endif // THEMEHANDLER_H
