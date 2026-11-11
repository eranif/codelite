#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxWebViewXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxWebViewXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
