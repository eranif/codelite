#ifndef RIBBONPANELWRAPPER_H
#define RIBBONPANELWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class RibbonPanelWrapper : public wxcWidget
{
public:
    RibbonPanelWrapper();
    ~RibbonPanelWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // RIBBONPANELWRAPPER_H
