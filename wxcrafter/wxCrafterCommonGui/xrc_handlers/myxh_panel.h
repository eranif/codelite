#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxPanelXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxPanelXmlHandler)

public:
    MyWxPanelXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
