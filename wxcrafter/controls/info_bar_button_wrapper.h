#ifndef INFOBARBUTTONWRAPPER_H
#define INFOBARBUTTONWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class InfoBarButtonWrapper : public wxcWidget
{
public:
    InfoBarButtonWrapper();
    ~InfoBarButtonWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // INFOBARBUTTONWRAPPER_H
