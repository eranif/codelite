#ifndef _WX_MYXH_CMDLINKBTN_H_
#define _WX_MYXH_CMDLINKBTN_H_

#include <wx/xrc/xmlres.h>

class wxCommandLinkButton;

class MyWxCommandLinkButtonXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxCommandLinkButtonXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_CMDLINKBTN_H_
