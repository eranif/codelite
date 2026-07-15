#ifndef _WX_MYXH_GLCANVAS_H_
#define _WX_MYXH_GLCANVAS_H_

#include <wx/xrc/xmlreshandler.h>

class MyWxGLCanvasXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxGLCanvasXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};

#endif // _WX_MYXH_GLCANVAS_H_
