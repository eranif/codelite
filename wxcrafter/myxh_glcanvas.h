#ifndef _WX_MYXH_GLCANVAS_H_
#define _WX_MYXH_GLCANVAS_H_

#include <wx/xrc/xmlres.h>

class MyWxGLCanvasXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxGLCanvasXmlHandler();

    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);
};

#endif // _WX_MYXH_GLCANVAS_H_
