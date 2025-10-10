#include "IndicatorPanel.hpp"

#include "clSystemSettings.h"
#include "file_logger.h"

#include <wx/sizer.h>

IndicatorPanel::IndicatorPanel(wxWindow* parent, const wxString& initialText)
{
    wxSize panel_size{wxNOT_FOUND, wxSize(parent->GetTextExtent("Tp")).GetHeight()};
    panel_size.IncBy(0, 10); // The borders
    if (!wxControl::Create(parent, wxID_ANY, wxDefaultPosition, panel_size)) {
        return;
    }

    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_statusMessage = new wxStaticText(this, wxID_ANY, initialText);
    m_activityIndicator = new wxActivityIndicator(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(16, 16)));
    GetSizer()->Add(m_activityIndicator, wxSizerFlags(0).CenterVertical().Border(wxALL, 5));
    GetSizer()->Add(m_statusMessage, wxSizerFlags(0).CenterVertical().Border(wxALL, 5));
    m_activityIndicator->Hide();
    SetSizeHints(panel_size);
    GetSizer()->Fit(this);
}

void IndicatorPanel::Start(const wxString& message)
{
    m_activityIndicator->Show();
    m_activityIndicator->Start();
    SetMessage(message);
    SendSizeEvent();
}

void IndicatorPanel::Stop(const wxString& message)
{
    m_activityIndicator->Stop();
    m_activityIndicator->Hide();
    SetMessage(message);
    SendSizeEvent();
}

bool IndicatorPanel::IsRunning() const { return m_activityIndicator->IsRunning(); }

void IndicatorPanel::SetMessage(const wxString& message) { m_statusMessage->SetLabel(message); }
