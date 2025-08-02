#ifndef WXCOLLAPSIBLEPANEPANEWRAPPER_H
#define WXCOLLAPSIBLEPANEPANEWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class wxCollapsiblePanePaneWrapper : public wxcWidget
{
public:
    wxCollapsiblePanePaneWrapper();
    ~wxCollapsiblePanePaneWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // WXCOLLAPSIBLEPANEPANEWRAPPER_H
