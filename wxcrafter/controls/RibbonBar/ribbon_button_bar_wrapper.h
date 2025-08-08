#ifndef RIBBONBUTTONBARWRAPPER_H
#define RIBBONBUTTONBARWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonButtonBarWrapper : public wxcWidget
{
public:
    RibbonButtonBarWrapper();
    ~RibbonButtonBarWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    wxString DoGenerateCppCtorCode_End() const override;
};

#endif // RIBBONBUTTONBARWRAPPER_H
