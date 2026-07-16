#ifndef _WX_MYXH_STC_H_
#define _WX_MYXH_STC_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxStcXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxStcXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_STC_H_
