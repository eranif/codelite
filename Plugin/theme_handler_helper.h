#ifndef THEMEHANDLERHELPER_H
#define THEMEHANDLERHELPER_H

#include <wx/event.h>
#include <wx/window.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeHandlerHelper : public wxEvtHandler
{
    wxWindow* m_window;
protected:
    void DoUpdateColours(wxWindow* win, const wxColour& bg, const wxColour& fg);
    
public:
    ThemeHandlerHelper(wxWindow* win);
    virtual ~ThemeHandlerHelper();
    void OnThemeChanged(wxCommandEvent &e);
};

#endif // THEMEHANDLERHELPER_H
