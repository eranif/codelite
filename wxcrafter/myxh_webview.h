#ifndef _WX_MYXH_WEBVIEW_H_
#define _WX_MYXH_WEBVIEW_H_

#include <wx/xrc/xmlres.h>

class MyWxWebViewXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxWebViewXmlHandler();

    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_WEBVIEW_H_
