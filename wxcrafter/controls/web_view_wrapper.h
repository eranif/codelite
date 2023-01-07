#ifndef WEBVIEWWRAPPER_H
#define WEBVIEWWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class WebViewWrapper : public wxcWidget
{
public:
    WebViewWrapper();
    virtual ~WebViewWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // WEBVIEWWRAPPER_H
