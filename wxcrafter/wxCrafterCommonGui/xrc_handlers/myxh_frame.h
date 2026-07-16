#ifndef _WX_MYXH_FRAME_H_
#define _WX_MYXH_FRAME_H_

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

#endif // _WX_MYXH_FRAME_H_
