#include "NewSseMCPDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"

NewSseMCPDlg::NewSseMCPDlg(wxWindow* parent)
    : NewSseMCPDlgBase(parent)
{
    GetSizer()->Fit(this);
    CenterOnParent();
}

NewSseMCPDlg::~NewSseMCPDlg() {}

std::map<std::string, std::string> NewSseMCPDlg::ProcessKeyValueEntry(wxTextCtrl* text_ctrl, const wxString& prefix)
{
    wxString value = text_ctrl->GetValue();
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

    text_ctrl->ChangeValue(new_text);
    return pairs;
}

llm::SSEMcp NewSseMCPDlg::GetData() const
{
    llm::SSEMcp mcp;
    mcp.name = m_textCtrlName->GetValue();
    mcp.base_url = m_textCtrlBaseURL->GetValue();
    mcp.endpoint = m_textCtrlEndpoint->GetValue();
    mcp.auth_token = m_textCtrlToken->GetValue();
    mcp.headers = m_headers;
    return mcp;
}

void NewSseMCPDlg::OnHeaders(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_headers = ProcessKeyValueEntry(m_textCtrlHeaders, _(R"(# Write headers in the format "Header=Value"
# with each header on a separate line
)"));
}

void NewSseMCPDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlBaseURL->IsEmpty() && !m_textCtrlEndpoint->IsEmpty() && !m_textCtrlName->IsEmpty());
}
