#ifndef _WX_XH_SIMPLEBOOK_H
#define _WX_XH_SIMPLEBOOK_H

#include <wx/xrc/xmlres.h>

class wxSimplebook;

class MyWxSimplebookXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxSimplebookXmlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    bool m_isInside;
    wxSimplebook* m_notebook;
};

#endif //_WX_XH_SIMPLEBOOK_H
