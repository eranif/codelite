#include "NewMCPServerDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"
#include "StringUtils.h"
#include "globals.h"

namespace
{
enum ServerType {
    kStdio,
    kSSE,
};

const wxString kName = _("Name");
const wxString kCommand = _("Command");
const wxString kBaseURL = _("Base URL");
const wxString kEndpoint = _("Endpoint");
const wxString kAuthToken = _("Auth token");
const wxString kHeaders = _("Headers");
const wxString kEnvVariables = _("Environment variables");

} // namespace

NewMCPServerDlg::NewMCPServerDlg(wxWindow* parent)
    : NewMCPServerDlgBase(parent)
{
    m_view = new clPropertiesPage(this);
    m_mainSizer->Add(m_view, wxSizerFlags(1).Expand().Border(10));
    m_choiceServerType->SetSelection(ServerType::kStdio);
    m_view->Bind(wxEVT_PROPERTIES_PAGE_ACTION_BUTTON, &NewMCPServerDlg::OnActioButton, this);
    Initialise();
    ::clSetDialogBestSizeAndPosition(this);
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

std::map<std::string, std::string> NewMCPServerDlg::ProcessKeyValueEntry(clPropertiesPageEvent& event,
                                                                         const wxString& prefix)
{
    wxString value = m_view->GetView()->GetItemText(event.GetItem(), 1);
    std::map<std::string, std::string> pairs;
    if (value.empty()) {
        value = prefix;
    } else {
        value.Prepend(prefix);
    }
    auto lexer = ColoursAndFontsManager::Get().GetLexer("properties");

    // use EditDlg directly and not clGetTextFromUser() method
    // since empty string is a valid value
    EditDlg dlg(this, value, lexer);
    if (dlg.ShowModal() != wxID_OK) {
        return {};
    }

    auto lines = StringUtils::SplitString(dlg.GetText());
    wxString new_text;
    for (auto& line : lines) {
        if (line.starts_with(";") || line.starts_with("#")) {
            continue;
        }

        wxString name = line.BeforeFirst('=').Trim().Trim(false);
        wxString value = line.AfterFirst('=').Trim().Trim(false);

        if (name.empty() || line.empty()) {
            continue;
        }

        pairs.insert({name.ToStdString(wxConvUTF8), value.ToStdString(wxConvUTF8)});
        new_text << name << "=" << value << "\n";
    }

    m_view->GetView()->SetItemText(event.GetItem(), new_text, 1);
    return pairs;
}

void NewMCPServerDlg::UpdateUIForStdio()
{
    m_view->Clear();
    m_view->AddHeader(_("Common"));
    m_view->AddProperty(kName, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString name;
        if (!value.GetAs(&name)) {
            return;
        }
        m_name = name.ToStdString(wxConvUTF8);
    });

    m_view->AddProperty(kCommand, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString command;
        if (!value.GetAs(&command)) {
            return;
        }
        m_command = StringUtils::ToStdStrings(StringUtils::BuildArgv(command));
    });
    m_view->AddProperty(kEnvVariables, wxString{}, nullptr);
}

void NewMCPServerDlg::UpdateUIForSSE()
{
    m_view->Clear();
    m_view->AddHeader(_("Common"));
    m_view->AddProperty(kName, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString name;
        if (!value.GetAs(&name)) {
            return;
        }
        m_name = name.ToStdString(wxConvUTF8);
    });

    m_view->AddProperty(kBaseURL, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString base_url;
        if (!value.GetAs(&base_url)) {
            return;
        }
        m_baseUrl = base_url.ToStdString(wxConvUTF8);
    });

    m_view->AddProperty(kEndpoint, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString endpoint;
        if (!value.GetAs(&endpoint)) {
            return;
        }
        m_endpoint = endpoint.ToStdString(wxConvUTF8);
    });
    m_view->AddProperty(kAuthToken, wxString{}, [this](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        wxString token;
        if (!value.GetAs(&token)) {
            return;
        }
        m_authtoken = token.ToStdString(wxConvUTF8);
    });
    m_view->AddProperty(kHeaders, wxString{}, nullptr);
}

void NewMCPServerDlg::OnActioButton(clPropertiesPageEvent& event)
{
    if (event.GetLineKind()->line_kind != LineKind::TEXT_EDIT) {
        event.Skip();
        return;
    }

    wxString value = m_view->GetView()->GetItemText(event.GetItem(), 1);
    if (event.GetLabel() == kHeaders) {
        const wxString prefix =
            _("# Write headers in the format of Header=Value\n# Each header should be placed on its line\n");
        m_headers = ProcessKeyValueEntry(event, prefix);
    } else if (event.GetLabel() == kEnvVariables) {
        const wxString prefix = _("# Write environment variables in the format of EnvName=Value\n# Each variable "
                                  "should be placed on its line\n");
        m_envVariables = ProcessKeyValueEntry(event, prefix);
    } else {
        event.Skip();
    }
}

void NewMCPServerDlg::OnServerTypeChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Initialise();
}

bool NewMCPServerDlg::IsStdioServer() const { return m_choiceServerType->GetSelection() == ServerType::kStdio; }
bool NewMCPServerDlg::IsSSEServer() const { return m_choiceServerType->GetSelection() == ServerType::kSSE; }

llm::LocalMcp NewMCPServerDlg::GetLocalMcpData() const
{
    llm::LocalMcp mcp;
    mcp.name = GetServerName();
    mcp.command = GetCommand();
    mcp.env = GetEnvVariables();
    return mcp;
}

llm::SSEMcp NewMCPServerDlg::GetSSEMcpData() const
{
    llm::SSEMcp mcp;
    mcp.name = GetServerName();
    mcp.base_url = GetBaseUrl();
    mcp.endpoint = GetEndpoint();
    mcp.headers = GetHeaders();
    mcp.auth_token = GetAuthtoken();
    return mcp;
}
