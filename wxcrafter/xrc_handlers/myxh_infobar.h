#ifndef _WX_MYXH_INFOBAR_H_
#define _WX_MYXH_INFOBAR_H_

#include <wx/infobar.h>
#include <wx/xrc/xmlres.h>

class wxInfoBar;

class MyWxInfoBarCtrlHandler : public wxXmlResourceHandler
{
protected:
    wxInfoBar* HandleInfoBar();
    void HandleButton();

public:
    MyWxInfoBarCtrlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_INFOBAR_H_
