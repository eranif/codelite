#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <wx/dialog.h>

class MyDialog : public wxDialog
{
public:
    MyDialog();
    // Constructor with no modal flag - the new convention.
    MyDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
        : wxDialog(parent, id, title, pos, size, style, name)
    {
    }
    virtual ~MyDialog();
};

#endif // MYDIALOG_H
