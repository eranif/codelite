#ifndef _WX_MYXH_CMDLINKBTN_H_
#define _WX_MYXH_CMDLINKBTN_H_

#include <wx/xrc/xmlreshandler.h>

class wxCommandLinkButton;

class MyWxCommandLinkButtonXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxCommandLinkButtonXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_CMDLINKBTN_H_
