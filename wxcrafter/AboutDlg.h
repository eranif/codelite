#ifndef ABOUTDLG_H
#define ABOUTDLG_H
#include "wxcrafter.h"

class wxcAboutDlg : public wxcAboutDlgBaseClass
{
public:
    wxcAboutDlg(wxWindow* parent);
    virtual ~wxcAboutDlg();

protected:
    virtual void OnSize(wxSizeEvent& event);
    void DoRefresh();
};
#endif // ABOUTDLG_H
