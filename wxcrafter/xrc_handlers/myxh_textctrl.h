#ifndef _WX_MYXH_TEXTCTRL_H_
#define _WX_MYXH_TEXTCTRL_H_

#include <wx/xrc/xmlres.h>

class MyTextCtrlXrcHandler : public wxXmlResourceHandler
{
public:
    MyTextCtrlXrcHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_TEXTCTRL_H_
