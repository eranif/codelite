#include "myxh_cmdlinkbtn.h"
#include <wx/commandlinkbutton.h>

MyWxCommandLinkButtonXmlHandler::MyWxCommandLinkButtonXmlHandler()
    : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxBU_LEFT);
    XRC_ADD_STYLE(wxBU_RIGHT);
    XRC_ADD_STYLE(wxBU_TOP);
    XRC_ADD_STYLE(wxBU_BOTTOM);
    XRC_ADD_STYLE(wxBU_EXACTFIT);
    AddWindowStyles();
}

wxObject* MyWxCommandLinkButtonXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(button, wxCommandLinkButton)

    button->Create(m_parentAsWindow, GetID(), GetText(wxS("label")), GetText(wxS("note")), GetPosition(), GetSize(),
                   GetStyle(), wxDefaultValidator, GetName());
    wxBitmap bmp = GetBitmap();
    if(bmp.IsOk()) { button->SetBitmap(GetBitmap(), wxLEFT); }
    SetupWindow(button);
    return button;
}

bool MyWxCommandLinkButtonXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxS("wxCommandLinkButton")); }
