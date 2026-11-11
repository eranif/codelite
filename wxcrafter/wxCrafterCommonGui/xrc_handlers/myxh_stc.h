#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxStcXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxStcXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
