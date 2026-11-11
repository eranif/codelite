#pragma once
#include "wxcrafter.hpp"

class wxcAboutDlg : public wxcAboutDlgBaseClass
{
public:
    wxcAboutDlg(wxWindow* parent);
    ~wxcAboutDlg() override = default;

protected:
    void OnSize(wxSizeEvent& event) override;
};
