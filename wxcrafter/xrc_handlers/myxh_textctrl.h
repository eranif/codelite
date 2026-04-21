#ifndef _WX_MYXH_TEXTCTRL_H_
#define _WX_MYXH_TEXTCTRL_H_

#include <wx/xrc/xmlreshandler.h>

class MyTextCtrlXrcHandler : public wxXmlResourceHandler
{
public:
    MyTextCtrlXrcHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_TEXTCTRL_H_
