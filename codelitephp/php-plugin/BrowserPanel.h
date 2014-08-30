#ifndef BROWSERPANEL_H
#define BROWSERPANEL_H
#include "php_ui.h"

#if wxUSE_WEBVIEW
class BrowserPanel : public BrowserPanelBase
{
public:
    BrowserPanel(wxWindow* parent);
    virtual ~BrowserPanel();

protected:

    virtual void OnNewWindow(wxWebViewEvent& event);
    virtual void OnTitleChanged(wxWebViewEvent& event);

    virtual void OnGoUI(wxUpdateUIEvent& event);
    virtual void OnGoURL(wxCommandEvent& event);

public:
    void LoadURL(const wxString &url);
};

#endif
#endif // BROWSERPANEL_H
