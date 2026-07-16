#ifndef _WX_MYXH_MEDIACTRL_H_
#define _WX_MYXH_MEDIACTRL_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxMediaCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxMediaCtrlXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_MEDIACTRL_H_
