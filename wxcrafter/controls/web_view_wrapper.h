#ifndef WEBVIEWWRAPPER_H
#define WEBVIEWWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class WebViewWrapper : public wxcWidget
{
public:
    WebViewWrapper();
    ~WebViewWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // WEBVIEWWRAPPER_H
