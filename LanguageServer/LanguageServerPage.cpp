#include "ColoursAndFontsManager.h"
#include "LanguageServerPage.h"
#include "LanguageServerProtocol.h"
#include "globals.h"
#include <algorithm>
#include <macros.h>
#include <wx/choicdlg.h>
#include <wx/dirdlg.h>
#include <wx/tokenzr.h>

#if USE_SFTP
#include "sftp_settings.h"
#endif

LanguageServerPage::LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data)
    : LanguageServerPageBase(parent)
{
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->Apply(m_stcCommand);
        lex->Apply(m_stcInitOptions);
        lex->Apply(m_stcEnvironment);
    }
    m_textCtrlName->SetValue(data.GetName());
    m_textCtrlWD->SetValue(data.GetWorkingDirectory());
    m_stcCommand->SetText(data.GetCommand());
    m_stcInitOptions->SetText(data.GetInitOptions());
    m_checkBoxEnabled->SetValue(data.IsEnabled());
    const wxArrayString& langs = data.GetLanguages();
    wxString languages = wxJoin(langs, ';');
    this->m_textCtrlLanguages->SetValue(languages);
    this->m_comboBoxConnection->SetValue(data.GetConnectionString());
    m_checkBoxDiagnostics->SetValue(data.IsDisaplayDiagnostics());
    m_sliderPriority->SetValue(data.GetPriority());
    m_checkBoxRemoteServer->SetValue(data.IsRemoteLSP());

    InitialiseSSH(data);
    const auto& env = data.GetEnv();
    if(!env.empty()) {
        wxString envString;
        for(const auto& env_entry : env) {
            envString << env_entry.first << "=" << env_entry.second << "\n";
        }
        envString.RemoveLast();
        m_stcEnvironment->SetText(envString);
    }
}

LanguageServerPage::LanguageServerPage(wxWindow* parent)
    : LanguageServerPageBase(parent)
{
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->Apply(m_stcCommand);
        lex->Apply(m_stcInitOptions);
    }
    InitialiseSSH({});
}

LanguageServerPage::~LanguageServerPage() {}

LanguageServerEntry LanguageServerPage::GetData() const
{
    LanguageServerEntry d;
    d.SetName(m_textCtrlName->GetValue());
    d.SetCommand(m_stcCommand->GetText().Trim().Trim(false));
    d.SetWorkingDirectory(m_textCtrlWD->GetValue());
    d.SetLanguages(GetLanguages());
    d.SetEnabled(m_checkBoxEnabled->IsChecked());
    d.SetConnectionString(m_comboBoxConnection->GetValue());
    d.SetPriority(m_sliderPriority->GetValue());
    d.SetDisaplayDiagnostics(m_checkBoxDiagnostics->IsChecked());
    d.SetInitOptions(m_stcInitOptions->GetText().Trim().Trim(false));
    d.SetSshAccount(m_choiceSSHAccounts->GetStringSelection());
    d.SetRemoteLSP(m_checkBoxRemoteServer->IsChecked());

    // Store the environment
    clEnvList_t envList;
    wxArrayString env_lines = wxStringTokenize(m_stcEnvironment->GetText(), "\n", wxTOKEN_STRTOK);
    for(auto& line : env_lines) {
        TrimString(line);
        if(line.empty()) {
            continue;
        }
        wxString env_name = line.BeforeFirst('=');
        wxString env_value = line.AfterFirst('=');
        TrimString(env_name);
        TrimString(env_value);
        if(env_name.empty() || env_lines.empty()) {
            continue;
        }
        envList.push_back({ env_name, env_value });
    }
    d.SetEnv(envList);
    return d;
}

wxArrayString LanguageServerPage::GetLanguages() const
{
    wxArrayString selectedLanguages;
    wxString langStr = m_textCtrlLanguages->GetValue();
    selectedLanguages = ::wxStringTokenize(langStr, ";,", wxTOKEN_STRTOK);
    return selectedLanguages;
}

void LanguageServerPage::OnSuggestLanguages(wxCommandEvent& event)
{
    const std::set<wxString>& langSet = LanguageServerProtocol::GetSupportedLanguages();
    wxArrayString arrLang;
    for(const wxString& lang : langSet) {
        arrLang.Add(lang);
    }

    wxArrayInt selections;
    int count = ::wxGetSelectedChoices(selections, _("Select the supported languages by this server:"), _("CodeLite"),
                                       arrLang, GetParent());
    if(count == wxNOT_FOUND) {
        return;
    }

    wxString newText;
    for(int sel : selections) {
        newText << arrLang.Item(sel) << ";";
    }
    m_textCtrlLanguages->ChangeValue(newText);
}
void LanguageServerPage::OnCommandUI(wxUpdateUIEvent& event) { event.Enable(true); }
void LanguageServerPage::OnBrowseWD(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString path(m_textCtrlWD->GetValue());
    wxString new_path =
        wxDirSelector(_("Select a working directory:"), path, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
    if(new_path.IsEmpty() == false) {
        m_textCtrlWD->SetValue(new_path);
    }
}
void LanguageServerPage::OnRemoteServerUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxRemoteServer->IsChecked()); }

void LanguageServerPage::InitialiseSSH(const LanguageServerEntry& data)
{
#if USE_SFTP
    SFTPSettings s;
    s.Load();
    const auto& accounts = s.GetAccounts();
    int selectedAccount = wxNOT_FOUND;
    for(const auto& account : accounts) {
        int where = m_choiceSSHAccounts->Append(account.GetAccountName());
        if(account.GetAccountName() == data.GetSshAccount()) {
            selectedAccount = where;
        }
    }
    if(selectedAccount != wxNOT_FOUND) {
        m_choiceSSHAccounts->SetSelection(selectedAccount);
    } else if(!accounts.empty()) {
        m_choiceSSHAccounts->SetSelection(0);
    }
#else
    m_checkBoxRemoteServer->SetValue(false);
    m_checkBoxRemoteServer->Enable(false);
    wxUnusedVar(data);
#endif
}
