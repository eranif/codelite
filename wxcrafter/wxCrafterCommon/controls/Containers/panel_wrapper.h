#ifndef PANELWRAPPER_H
#define PANELWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase

class PanelWrapper : public wxcWidget
{
public:
    PanelWrapper();
    ~PanelWrapper() override = default;

public:
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    wxString CppCtorCode() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    bool IsTopWindow() const override;
    wxcWidget* Clone() const override { return new PanelWrapper(); }
    bool IsContainer() const override { return true; }
};

#endif // PANELWRAPPER_H
