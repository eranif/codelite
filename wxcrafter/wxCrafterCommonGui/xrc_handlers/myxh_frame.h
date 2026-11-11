#pragma once

#include <wx/xrc/xmlreshandler.h>

// Frame with wxAuiManager member
class wxMyFrameXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxMyFrameXmlHandler)

public:
    wxMyFrameXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
