#include "BrowserPanel.h"
#if wxUSE_WEBVIEW

BrowserPanel::BrowserPanel(wxWindow* parent)
    : BrowserPanelBase(parent)
{
}

BrowserPanel::~BrowserPanel()
{
}


void BrowserPanel::OnTitleChanged(wxWebViewEvent& event)
{
    event.Skip();
    m_banner->SetText(event.GetString(), "");
    m_textCtrlAddressBar->ChangeValue( event.GetURL() );
}
void BrowserPanel::OnNewWindow(wxWebViewEvent& event)
{
    m_infobar->ShowMessage("Popup window was blocked!", wxICON_WARNING|wxOK|wxOK_DEFAULT);
}

void BrowserPanel::LoadURL(const wxString& url)
{
    m_webView->LoadURL( url );
}

void BrowserPanel::OnGoUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlAddressBar->IsEmpty() );
}

void BrowserPanel::OnGoURL(wxCommandEvent& event)
{
    m_webView->LoadURL( m_textCtrlAddressBar->GetValue() );
}

#endif
