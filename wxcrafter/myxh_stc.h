#ifndef _WX_MYXH_STC_H_
#define _WX_MYXH_STC_H_

#include <wx/xrc/xmlres.h>

class MyWxStcXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxStcXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_STC_H_
