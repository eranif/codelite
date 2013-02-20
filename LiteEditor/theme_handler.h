#ifndef THEMEHANDLER_H
#define THEMEHANDLER_H

#include <wx/event.h> // Base class: wxEvtHandler

class ThemeHandler : public wxEvtHandler
{
public:
    ThemeHandler();
    virtual ~ThemeHandler();
    
    void OnEditorThemeChanged(wxCommandEvent &e);
};

#endif // THEMEHANDLER_H
