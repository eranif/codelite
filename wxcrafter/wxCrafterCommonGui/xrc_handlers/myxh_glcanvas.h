#pragma once

#include <wx/xrc/xmlreshandler.h>

class MyWxGLCanvasXmlHandler : public wxXmlResourceHandler
{
public:
    MyWxGLCanvasXmlHandler();

    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;
};
