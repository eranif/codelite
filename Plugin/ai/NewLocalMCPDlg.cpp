#include "NewLocalMCPDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"
#include "StringUtils.h"

NewLocalMCPDlg::NewLocalMCPDlg(wxWindow* parent)
    : NewLocalMCPDlgBase(parent)
{
    GetSizer()->Fit(this);
    CenterOnParent();
}

NewLocalMCPDlg::~NewLocalMCPDlg() {}

void NewLocalMCPDlg::OnEnvVariables(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_envVariables =
        ProcessKeyValueEntry(m_textCtrlEnv, _(R"(# Environment variables should be defined in the format EnvName=Value
# with each variable declaration on a separate line
)"));
}

std::map<std::string, std::string> NewLocalMCPDlg::ProcessKeyValueEntry(wxTextCtrl* text_ctrl, const wxString& prefix)
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

llm::LocalMcp NewLocalMCPDlg::GetData() const
{
    llm::LocalMcp mcp;
    mcp.name = m_textCtrlName->GetValue();
    mcp.command = StringUtils::ToStdStrings(StringUtils::BuildArgv(m_textCtrlCommand->GetValue()));
    mcp.env = m_envVariables;
    return mcp;
}

void NewLocalMCPDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty() && !m_textCtrlCommand->IsEmpty());
}
