#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxMediaCtrlXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxMediaCtrlXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
