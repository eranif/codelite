#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <wx/event.h> // Base class: wxEvtHandler
#include <wx/stc/stc.h>
#include "plugin.h"

class ThemeHandler : public wxEvtHandler
{
    void DoUpdateColours( wxWindow* win, const wxColour& bg, const wxColour &fg );
    void DoUpdateSTCBgColour(wxStyledTextCtrl *stc);
    
public:
    ThemeHandler();
    virtual ~ThemeHandler();
    
    void OnEditorThemeChanged(wxCommandEvent &e);
};

#endif // THEMEHANDLER_H
