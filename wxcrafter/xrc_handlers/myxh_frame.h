#ifndef _WX_MYXH_FRAME_H_
#define _WX_MYXH_FRAME_H_

#include <wx/xrc/xmlres.h>

// Frame with wxAuiManager member
class wxMyFrameXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(wxMyFrameXmlHandler)

public:
    wxMyFrameXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_FRAME_H_
