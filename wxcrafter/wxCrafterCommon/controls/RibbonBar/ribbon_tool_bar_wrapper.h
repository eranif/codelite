#ifndef RIBBONTOOLBARWRAPPER_H
#define RIBBONTOOLBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonToolBarWrapper : public wxcWidget
{
public:
    RibbonToolBarWrapper();
    ~RibbonToolBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateCppCtorCode_End() const override;
};

#endif // RIBBONTOOLBARWRAPPER_H
