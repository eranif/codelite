#include "IndicatorPanel.hpp"

#include "clSystemSettings.h"
#include "file_logger.h"

#include <wx/sizer.h>

IndicatorPanel::IndicatorPanel(wxWindow* parent, const wxString& initialText)
{
    wxSize panel_size{wxNOT_FOUND, wxSize(parent->GetTextExtent("Tp")).GetHeight()};
    panel_size.IncBy(0, 10); // The borders
    if (!wxControl::Create(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_NONE)) {
        return;
    }

    SetSizer(new wxBoxSizer(wxVERTICAL));
    wxFlexGridSizer* flexGridSizer = new wxFlexGridSizer(1, 3, 0, 0);
    flexGridSizer->SetFlexibleDirection(wxBOTH);
    flexGridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
    flexGridSizer->AddGrowableCol(2);
    GetSizer()->Add(flexGridSizer, wxSizerFlags(1).Expand());
    m_statusMessage = new wxStaticText(this, wxID_ANY, initialText);
    m_secondMessage = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_activityIndicator = new wxActivityIndicator(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(16, 16)));
    flexGridSizer->Add(m_activityIndicator, wxSizerFlags(0).CenterVertical().Border(wxALL, 5));
    flexGridSizer->Add(m_statusMessage, wxSizerFlags(1).CenterVertical().Expand().Border(wxALL, 5));
    flexGridSizer->Add(m_secondMessage, wxSizerFlags(1).CenterVertical().Expand().Border(wxALL, 5));

    Bind(wxEVT_SIZE, &IndicatorPanel::OnSize, this);
    m_activityIndicator->Hide();
    SetSizeHints(panel_size);
    Layout();
    GetSizer()->Fit(this);
}

void IndicatorPanel::Start(const wxString& message)
{
    m_activityIndicator->Show();
    m_activityIndicator->Start();
    SetMessage(message);
    PostSizeEvent();
}

void IndicatorPanel::Stop(const wxString& message)
{
    m_activityIndicator->Stop();
    m_activityIndicator->Hide();
    SetMessage(message);
    PostSizeEvent();
}

bool IndicatorPanel::IsRunning() const { return m_activityIndicator->IsRunning(); }

void IndicatorPanel::Clear(size_t column) { SetMessage(wxEmptyString, column); }

void IndicatorPanel::SetMessage(const wxString& message, size_t column)
{
    switch (column) {
    case 0:
        m_statusMessage->SetLabel(message);
        break;
    case 1:
        m_secondMessage->SetLabel(message);
        break;
    default:
        break;
    }
}

void IndicatorPanel::OnSize(wxSizeEvent& event) { event.Skip(); }
