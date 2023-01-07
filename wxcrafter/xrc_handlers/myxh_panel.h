#ifndef _WX_MYXH_PANEL_H_
#define _WX_MYXH_PANEL_H_

#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>

class MyWxPanelXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxPanelXmlHandler)

public:
    MyWxPanelXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_PANEL_H_
