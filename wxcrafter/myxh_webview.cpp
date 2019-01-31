#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if defined(__WXMSW__) || defined(__WXMAC__)
#define USE_WEBVIEW 0
#else
#define USE_WEBVIEW 0
#endif

#include "myxh_webview.h"
#if USE_WEBVIEW
#include <wx/webview.h>
#else
#include <wx/html/htmlwin.h>
#endif

MyWxWebViewXmlHandler::MyWxWebViewXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject* MyWxWebViewXmlHandler::DoCreateResource()
{
#if USE_WEBVIEW
    wxString url = GetText("url", false);
    if(url.IsEmpty()) { url = "http://www.google.com"; }
#if wxCHECK_VERSION(2, 9, 5)
    wxWebView* webview = wxWebView::New(m_parentAsWindow, GetID(), url, GetPosition(), GetSize(),
                                        wxWebViewBackendDefault, GetStyle(wxT("style")), GetName());
#else
    wxWebView* webview = wxWebView::New(m_parentAsWindow, GetID(), url, GetPosition(), GetSize(),
                                        wxWEB_VIEW_BACKEND_DEFAULT, GetStyle(wxT("style")), GetName());
#endif
#else
    wxHtmlWindow* webview =
        new wxHtmlWindow(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(wxT("style")), GetName());
    webview->SetPage("<html><body><b>wxWebView</b></body></html>");
#endif

    SetupWindow(webview);
    return webview;
}

bool MyWxWebViewXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxWebView")); }
