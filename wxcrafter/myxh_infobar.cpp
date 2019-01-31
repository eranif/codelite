#include "myxh_infobar.h"
#include <wx/infobar.h>

MyWxInfoBarCtrlHandler::MyWxInfoBarCtrlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
}

wxObject* MyWxInfoBarCtrlHandler::DoCreateResource()
{
    if(m_class == "wxInfoBarButton") {
        HandleButton();

    } else {
        wxASSERT_MSG(m_class == "wxInfoBar", wxT("can't handle unknown node"));

        return HandleInfoBar();
    }

    return m_parentAsWindow;
}

bool MyWxInfoBarCtrlHandler::CanHandle(wxXmlNode* node)
{
    return IsOfClass(node, "wxInfoBar") || IsOfClass(node, "wxInfoBarButton");
}

void MyWxInfoBarCtrlHandler::HandleButton()
{
    wxInfoBar* const bar = wxDynamicCast(m_parentAsWindow, wxInfoBar);
    wxCHECK_RET(bar, wxT("must have wxInfoBar parent"));

    wxString buttonLabel = GetText("label");
    int buttonId = GetID();
    if(buttonId != wxNOT_FOUND) { bar->AddButton(buttonId, buttonLabel); }
}

wxInfoBar* MyWxInfoBarCtrlHandler::HandleInfoBar()
{
    XRC_MAKE_INSTANCE(infobar, wxInfoBar)
    infobar->Create(m_parentAsWindow, GetID());
    infobar->SetName(GetName());

    wxString message = GetText("message");
    wxString iconId = GetText("icon-id");

    int nIconId = wxICON_NONE;

    if(iconId == "wxICON_NONE")
        nIconId = wxICON_NONE;

    else if(iconId == "wxICON_ERROR")
        nIconId = wxICON_ERROR;

    else if(iconId == "wxICON_INFORMATION")
        nIconId = wxICON_INFORMATION;

    else if(iconId == "wxICON_WARNING")
        nIconId = wxICON_WARNING;

    infobar->SetName(GetName());
    SetupWindow(infobar);
    CreateChildrenPrivately(infobar);
    infobar->ShowMessage(message, nIconId);
    return infobar;
}
