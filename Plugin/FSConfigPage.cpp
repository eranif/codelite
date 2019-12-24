#include "BuildTargetDlg.h"
#include "ColoursAndFontsManager.h"
#include "FSConfigPage.h"
#include "build_settings_config.h"
#include "debuggermanager.h"
#include "macros.h"
#include <wx/tokenzr.h>

#if USE_SFTP
#include "SFTPBrowserDlg.h"
#include "SSHAccountManagerDlg.h"
#include "sftp_settings.h"
#endif

#define OPEN_SSH_ACCOUNT_MANAGER "-- Open SSH Account Manager --"

FSConfigPage::FSConfigPage(wxWindow* parent, clFileSystemWorkspaceConfig::Ptr_t config)
    : FSConfigPageBase(parent)
{
    m_config = config;
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcCCFlags);
        lexer->Apply(m_stcEnv);
    }

    m_dvListCtrlTargets->SetSortFunction([](clRowEntry* a, clRowEntry* b) {
        const wxString& cellA = a->GetLabel(0);
        const wxString& cellB = b->GetLabel(0);
        return (cellA.CmpNoCase(cellB) < 0);
    });

    m_stcCCFlags->SetText(m_config->GetCompileFlagsAsString());
    m_textCtrlFileExt->ChangeValue(m_config->GetFileExtensions());
    m_filePickerExe->SetPath(m_config->GetExecutable());
    m_textCtrlArgs->ChangeValue(m_config->GetArgs());
    m_stcEnv->SetText(m_config->GetEnvironment());
    m_checkBoxCreateCompileFlags->SetValue(m_config->ShouldCreateCompileFlags());
    const auto& targets = m_config->GetBuildTargets();
    for(const auto& vt : targets) {
        wxDataViewItem item = m_dvListCtrlTargets->AppendItem(vt.first);
        m_dvListCtrlTargets->SetItemText(item, vt.second, 1);
    }

    wxArrayString compilers = BuildSettingsConfigST::Get()->GetAllCompilersNames();
    m_choiceCompiler->Append(compilers);
    m_choiceCompiler->SetStringSelection(m_config->GetCompiler());

    DoUpdateSSHAcounts();

    m_checkBoxEnableRemote->SetValue(config->IsRemoteEnabled());
    m_textCtrlRemoteFolder->ChangeValue(config->GetRemoteFolder());
    m_choiceDebuggers->Append(DebuggerMgr::Get().GetAvailableDebuggers());
    m_choiceDebuggers->SetStringSelection(config->GetDebugger());
    m_textCtrlExcludeFiles->ChangeValue(config->GetExcludeFilesPattern());
}

FSConfigPage::~FSConfigPage() {}

void FSConfigPage::OnDelete(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);

    m_dvListCtrlTargets->DeleteItem(m_dvListCtrlTargets->ItemToRow(item));
}

void FSConfigPage::OnDeleteUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if(item.IsOk()) {
        wxString name = m_dvListCtrlTargets->GetItemText(item, 0);
        event.Enable(name != "build" && name != "clean");

    } else {
        event.Enable(false);
    }
}

void FSConfigPage::OnEditTarget(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoTargetActivated();
}

void FSConfigPage::OnEditTargetUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlTargets->GetSelectedItemsCount());
}

void FSConfigPage::OnNewTarget(wxCommandEvent& event)
{
    BuildTargetDlg dlg(::wxGetTopLevelParent(this), "", "");
    if(dlg.ShowModal() == wxID_OK) {
        wxDataViewItem item = m_dvListCtrlTargets->AppendItem(dlg.GetTargetName());
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetCommand(), 1);
    }
}

void FSConfigPage::Save()
{
    std::map<wxString, wxString> targets;
    for(size_t i = 0; i < m_dvListCtrlTargets->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlTargets->RowToItem(i);
        wxString name = m_dvListCtrlTargets->GetItemText(item, 0);
        wxString command = m_dvListCtrlTargets->GetItemText(item, 1);
        targets.insert({ name, command });
    }

    m_config->SetCreateCompileFlags(m_checkBoxCreateCompileFlags->IsChecked());
    m_config->SetBuildTargets(targets);
    m_config->SetCompileFlags(::wxStringTokenize(m_stcCCFlags->GetText(), "\r\n", wxTOKEN_STRTOK));
    m_config->SetFileExtensions(m_textCtrlFileExt->GetValue());
    m_config->SetExecutable(m_filePickerExe->GetPath());
    m_config->SetEnvironment(m_stcEnv->GetText());
    m_config->SetArgs(m_textCtrlArgs->GetValue());
    m_config->SetCompiler(m_choiceCompiler->GetStringSelection());
    m_config->SetRemoteFolder(m_textCtrlRemoteFolder->GetValue());
    m_config->SetRemoteEnabled(m_checkBoxEnableRemote->IsChecked());
    m_config->SetRemoteAccount(m_choiceSSHAccount->GetStringSelection());
    m_config->SetDebugger(m_choiceDebuggers->GetStringSelection());
    m_config->SetExcludeFilesPattern(m_textCtrlExcludeFiles->GetValue());
}

void FSConfigPage::OnTargetActivated(wxDataViewEvent& event)
{
    wxUnusedVar(event);
    DoTargetActivated();
}

void FSConfigPage::DoTargetActivated()
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);

    BuildTargetDlg dlg(::wxGetTopLevelParent(this), m_dvListCtrlTargets->GetItemText(item, 0),
                       m_dvListCtrlTargets->GetItemText(item, 1));
    if(dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetName(), 0);
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetCommand(), 1);
    }
}
void FSConfigPage::OnRemoteEnabledUI(wxUpdateUIEvent& event)
{
#if USE_SFTP
    event.Enable(m_checkBoxEnableRemote->IsChecked());
#else
    event.Enable(false);
#endif
}

void FSConfigPage::OnSSHBrowse(wxCommandEvent& event)
{
    wxUnusedVar(event);
#if USE_SFTP
    SFTPBrowserDlg dlg(GetParent(), _("Choose folder"), "", clSFTP::SFTP_BROWSE_FOLDERS);
    dlg.Initialize(m_choiceSSHAccount->GetStringSelection(), m_textCtrlRemoteFolder->GetValue());
    if(dlg.ShowModal() == wxID_OK) { m_textCtrlRemoteFolder->ChangeValue(dlg.GetPath()); }
#endif
}

void FSConfigPage::OnSSHAccountChoice(wxCommandEvent& event)
{
#if USE_SFTP
    wxString s = m_choiceSSHAccount->GetStringSelection();
    if(s == OPEN_SSH_ACCOUNT_MANAGER) {
        SSHAccountManagerDlg dlg(GetParent());
        dlg.ShowModal();
        CallAfter(&FSConfigPage::DoUpdateSSHAcounts);
    }
#endif
}

void FSConfigPage::DoUpdateSSHAcounts()
{
#if USE_SFTP
    m_choiceSSHAccount->Clear();
    // Load the SSH accounts
    SFTPSettings settings;
    settings.Load();
    const SSHAccountInfo::Vect_t& accounts = settings.GetAccounts();
    const wxString& selectedAccount = m_config->GetRemoteAccount();
    int sel = wxNOT_FOUND;
    for(const auto& v : accounts) {
        int where = m_choiceSSHAccount->Append(v.GetAccountName());
        if(sel == wxNOT_FOUND && (v.GetAccountName() == selectedAccount)) { sel = where; }
    }
    if(sel != wxNOT_FOUND) {
        m_choiceSSHAccount->SetSelection(sel);
    } else if(!m_choiceSSHAccount->IsEmpty()) {
        m_choiceSSHAccount->SetSelection(0);
    }
    m_choiceSSHAccount->Append(OPEN_SSH_ACCOUNT_MANAGER);
#endif
}
void FSConfigPage::OnEnableRemoteUI(wxUpdateUIEvent& event)
{
#if USE_SFTP
    event.Enable(true);
#else
    event.Enable(false);
#endif
}
