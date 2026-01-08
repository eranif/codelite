#include "NewMCPServerDlg.hpp"

namespace
{
enum ServerType {
    kStdio,
    kSSE,
};
}

NewMCPServerDlg::NewMCPServerDlg(wxWindow* parent)
    : NewMCPServerDlgBase(parent)
{
    m_view = new clPropertiesPage(this);
    Initialise();
    m_mainSizer->Add(m_view, wxSizerFlags(1).Expand().Border(10));
}

NewMCPServerDlg::~NewMCPServerDlg() {}

void NewMCPServerDlg::Initialise()
{
    switch (m_choiceServerType->GetSelection()) {
    case ServerType::kStdio:
        UpdateUIForStdio();
        break;
    case ServerType::kSSE:
        UpdateUIForSSE();
        break;
    }
}

void NewMCPServerDlg::UpdateUIForStdio()
{
    m_view->Clear();
    m_view->AddProperty(_("Command"), wxString{}, [](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxUnusedVar(value);
    });
}

void NewMCPServerDlg::UpdateUIForSSE() {}