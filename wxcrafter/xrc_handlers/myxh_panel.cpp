#include "myxh_panel.h"
#include "my_panel.h"
#include <wx/panel.h>

IMPLEMENT_DYNAMIC_CLASS(MyWxPanelXmlHandler, wxXmlResourceHandler)

MyWxPanelXmlHandler::MyWxPanelXmlHandler()
    : wxXmlResourceHandler()
{
#if WXWIN_COMPATIBILITY_2_6 && defined(wxNO_3D)
    XRC_ADD_STYLE(wxNO_3D);
#endif // WXWIN_COMPATIBILITY_2_6
    XRC_ADD_STYLE(wxTAB_TRAVERSAL);
    XRC_ADD_STYLE(wxWS_EX_VALIDATE_RECURSIVELY);

    AddWindowStyles();
}

wxObject* MyWxPanelXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(panel, MyPanel)

    panel->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(wxT("style"), wxTAB_TRAVERSAL),
                  GetName());

    SetupWindow(panel);
    CreateChildren(panel);

    return panel;
}

bool MyWxPanelXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxPanel")); }
