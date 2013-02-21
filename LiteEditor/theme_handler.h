#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <wx/event.h> // Base class: wxEvtHandler

extern const wxEventType wxEVT_CL_THEME_CHANGED;

class ThemeHandler : public wxEvtHandler
{
    void DoUpdateColours( wxWindow* win, const wxColour& bg, const wxColour &fg );
    
public:
    ThemeHandler();
    virtual ~ThemeHandler();
    
    void OnEditorThemeChanged(wxCommandEvent &e);
};

#endif // THEMEHANDLER_H
