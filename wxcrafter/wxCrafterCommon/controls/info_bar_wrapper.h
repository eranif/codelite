#pragma once

#include "wxc_widget.h" // Base class: wxcWidget

class InfoBarWrapper : public wxcWidget
{
public:
    InfoBarWrapper();
    ~InfoBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};
