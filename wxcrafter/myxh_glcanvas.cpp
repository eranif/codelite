#include "OpenGLCanvas.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "myxh_glcanvas.h"
#include <wx/panel.h>

MyWxGLCanvasXmlHandler::MyWxGLCanvasXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject* MyWxGLCanvasXmlHandler::DoCreateResource()
{
    OpenGLCanvas* panel = new OpenGLCanvas(m_parentAsWindow);
    panel->SetName(GetName());
    panel->Bitmap()->SetName(GetName());
    SetupWindow(panel);
    return panel;
}

bool MyWxGLCanvasXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxGLCanvas")); }
