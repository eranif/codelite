#include "NewLocalMCPDlg.hpp"

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"
#include "StringUtils.h"
#include "globals.h"

NewLocalMCPDlg::NewLocalMCPDlg(wxWindow* parent)
    : NewLocalMCPDlgBase(parent)
{
    GetSizer()->Fit(this);
    CenterOnParent();
}

NewLocalMCPDlg::~NewLocalMCPDlg() {}

void NewLocalMCPDlg::OnCommand(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditDlg dlg(this, m_textCtrlCommand->GetValue(), nullptr);

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    m_command = StringUtils::ToStdStrings(StringUtils::BuildArgv(dlg.GetText()));
    m_textCtrlCommand->ChangeValue(dlg.GetText());
}

void NewLocalMCPDlg::OnEnvVariables(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_envVariables =
        ProcessKeyValueEntry(m_textCtrlEnv,
                             _("# Write environment variables in the format of EnvName=Value\n# Each variable "
                               "should be placed on its line\n"));
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
    mcp.command = m_command;
    mcp.env = m_envVariables;
    return mcp;
}

void NewLocalMCPDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlName->IsEmpty() && !m_textCtrlCommand->IsEmpty());
}
