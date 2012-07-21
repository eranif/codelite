#ifndef WXCRAFTER_H
#define WXCRAFTER_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>

class MainDialogBaseClass : public wxDialog
{
protected:
    wxStaticLine* m_staticLine15;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

protected:

public:
    MainDialogBaseClass(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("My Dialog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,300), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    virtual ~MainDialogBaseClass();
};

#endif
