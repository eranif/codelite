#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyTextCtrlXrcHandler : public wxXmlResourceHandler
{
public:
    MyTextCtrlXrcHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
