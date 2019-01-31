#include "MediaCtrl.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "myxh_mediactrl.h"
#include <wx/panel.h>

MyWxMediaCtrlXmlHandler::MyWxMediaCtrlXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject* MyWxMediaCtrlXmlHandler::DoCreateResource()
{
    MediaCtrl* panel = new MediaCtrl(m_parentAsWindow);
    panel->SetName(GetName());
    panel->Bitmap()->SetName(GetName());
    SetupWindow(panel);
    return panel;
}

bool MyWxMediaCtrlXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxMediaCtrl")); }
