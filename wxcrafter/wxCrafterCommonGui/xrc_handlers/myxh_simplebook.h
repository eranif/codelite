#ifndef _WX_XH_SIMPLEBOOK_H
#define _WX_XH_SIMPLEBOOK_H

#include <wx/xrc/xmlreshandler.h>

class wxSimplebook;

class MyWxSimplebookXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxSimplebookXmlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    bool m_isInside = false;
    wxSimplebook* m_notebook = nullptr;
};

#endif //_WX_XH_SIMPLEBOOK_H
