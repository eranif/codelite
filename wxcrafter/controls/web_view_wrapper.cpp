#include "web_view_wrapper.h"

#include "allocator_mgr.h"
#include "wxgui_helpers.h"

WebViewWrapper::WebViewWrapper()
    : wxcWidget(ID_WXWEBVIEW)
{
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_NAVIGATING", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_NAVIGATING event, generated before trying to get a resource. This "
                    "event may be vetoed to prevent navigating to this resource. Note that if the displayed HTML "
                    "document has several frames, one such event will be generated per frame"));
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_NAVIGATED", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_NAVIGATED event generated after it was confirmed that a resource "
                    "would be requested. This event may not be vetoed. Note that if the displayed HTML document has "
                    "several frames, one such event will be generated per frame."));
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_LOADED", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_LOADED event generated when the document is fully loaded and "
                    "displayed. Note that if the displayed HTML document has several frames, one such event will be "
                    "generated per frame."));
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_ERROR", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_ERROR event generated when a navigation error occurs. The integer "
                    "associated with this event will be a wxWebNavigationError item. The string associated with this "
                    "event may contain a backend-specific more precise error message/code"));
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_NEWWINDOW", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_NEWWINDOW event, generated when a new window is created. You must "
                    "handle this event if you want anything to happen, for example to load the page in a new window or "
                    "tab."));
    RegisterEvent("wxEVT_COMMAND_WEBVIEW_TITLE_CHANGED", "wxWebViewEvent",
                  _("Process a wxEVT_COMMAND_WEBVIEW_TITLE_CHANGED event, generated when the page title changes. Use "
                    "GetString to get the title."));

    SetPropertyString(_("Common Settings"), "wxWebView");
    AddProperty(new StringProperty(PROP_URL, _("about:blank"),
                                   _("URL to load by default in the web view.\nNote that the designer will display "
                                     "about:blank. The preview and generated code will use the actual URL")));

    EnableSizerFlag("wxEXPAND", true);
    m_sizerItem.SetProportion(1);

    m_namePattern = "m_webView";
    SetName(GenerateName());

    // All generated code by this class should be wrapped with '#if wxUSE_WEBVIEW' blocks
    // we do this by simply telling wxcWidget that this class requires this macro definition
    SetIfBlockCond("wxUSE_WEBVIEW");
}

WebViewWrapper::~WebViewWrapper() {}

wxcWidget* WebViewWrapper::Clone() const { return new WebViewWrapper(); }

wxString WebViewWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << GetName() << " = wxWebView::New(" << GetWindowParent() << ", " << GetId() << ", "
            << wxCrafter::UNDERSCORE(PropertyString(PROP_URL)) << ", "
            << "wxDefaultPosition, " << SizeAsString() << ", "
            << "wxWebViewBackendDefault, " << StyleFlags("0") << ");\n";
    cppCode << CPPCommonAttributes();
    return cppCode;
}

void WebViewWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/webview.h>"); }

wxString WebViewWrapper::GetWxClassName() const { return "wxWebView"; }

void WebViewWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {
        text << XRCUnknown();
    } else if(type == XRC_PREVIEW) {
        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << "<url>"
             << wxCrafter::CDATA(PropertyString(PROP_URL)) << "</url>" << XRCSuffix();
    } else {
        // Designer
        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes() << "<url>about:blank</url>"
             << XRCSuffix();
    }
}
