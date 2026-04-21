#ifndef _WX_MYXH_WEBVIEW_H_
#define _WX_MYXH_WEBVIEW_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxWebViewXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxWebViewXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_WEBVIEW_H_
