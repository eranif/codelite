#ifndef ABOUTDLG_H
#define ABOUTDLG_H
#include "wxcrafter.h"

class wxcAboutDlg : public wxcAboutDlgBaseClass
{
public:
    wxcAboutDlg(wxWindow* parent);
    virtual ~wxcAboutDlg() = default;

protected:
    virtual void OnSize(wxSizeEvent& event);
};
#endif // ABOUTDLG_H
