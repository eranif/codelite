#ifndef _WX_MYXH_PANEL_H_
#define _WX_MYXH_PANEL_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxPanelXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxPanelXmlHandler)

public:
    MyWxPanelXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_PANEL_H_
