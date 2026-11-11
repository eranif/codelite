#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxDialogXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxDialogXmlHandler)

public:
    MyWxDialogXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
