#ifndef ABOUTDLG_H
#define ABOUTDLG_H
#include "wxcrafter.hpp"

class wxcAboutDlg : public wxcAboutDlgBaseClass
{
public:
    wxcAboutDlg(wxWindow* parent);
    ~wxcAboutDlg() override = default;

protected:
    void OnSize(wxSizeEvent& event) override;
};
#endif // ABOUTDLG_H
