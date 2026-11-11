#pragma once

#include <wx/infobar.h>
#include <wx/xrc/xmlreshandler.h>

class wxInfoBar;

class MyWxInfoBarCtrlHandler : public wxXmlResourceHandler
{
protected:
    wxInfoBar* HandleInfoBar();
    void HandleButton();

public:
    MyWxInfoBarCtrlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
