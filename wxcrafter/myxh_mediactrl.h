#ifndef _WX_MYXH_MEDIACTRL_H_
#define _WX_MYXH_MEDIACTRL_H_

#include <wx/xrc/xmlres.h>

class MyWxMediaCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxMediaCtrlXmlHandler();

    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_MEDIACTRL_H_
