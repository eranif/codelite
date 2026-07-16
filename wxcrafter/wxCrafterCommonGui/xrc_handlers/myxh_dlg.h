#ifndef _MYWX_XH_DLG_H_
#define _MYWX_XH_DLG_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxDialogXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxDialogXmlHandler)

public:
    MyWxDialogXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _MYWX_XH_DLG_H_
