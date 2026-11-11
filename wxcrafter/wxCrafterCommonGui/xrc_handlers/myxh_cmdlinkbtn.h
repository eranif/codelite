#pragma once

#include <wx/xrc/xmlreshandler.h>

class wxCommandLinkButton;

class MyWxCommandLinkButtonXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxCommandLinkButtonXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
