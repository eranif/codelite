#ifndef _MYWX_XH_DLG_H_
#define _MYWX_XH_DLG_H_

#include <wx/xrc/xmlres.h>

class MyWxDialogXmlHandler : public wxXmlResourceHandler
{
    DECLARE_DYNAMIC_CLASS(MyWxDialogXmlHandler)

public:
    MyWxDialogXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _MYWX_XH_DLG_H_
