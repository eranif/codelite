#include "FSConfigPage.h"
#include "ColoursAndFontsManager.h"
#include <wx/tokenzr.h>
#include "BuildTargetDlg.h"
#include "macros.h"
#include "build_settings_config.h"

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
    const wxStringMap_t& targets = m_config->GetBuildTargets();
    for(const auto& vt : targets) {
        wxDataViewItem item = m_dvListCtrlTargets->AppendItem(vt.first);
        m_dvListCtrlTargets->SetItemText(item, vt.second, 1);
    }
    
    wxArrayString compilers = BuildSettingsConfigST::Get()->GetAllCompilersNames();
    m_choiceCompiler->Append(compilers);
    m_choiceCompiler->SetStringSelection(m_config->GetCompiler());
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
    wxStringMap_t targets;
    for(size_t i = 0; i < m_dvListCtrlTargets->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlTargets->RowToItem(i);
        wxString name = m_dvListCtrlTargets->GetItemText(item, 0);
        wxString command = m_dvListCtrlTargets->GetItemText(item, 1);
        targets.insert({ name, command });
    }

    m_config->SetBuildTargets(targets);
    m_config->SetCompileFlags(::wxStringTokenize(m_stcCCFlags->GetText(), "\r\n", wxTOKEN_STRTOK));
    m_config->SetFileExtensions(m_textCtrlFileExt->GetValue());
    m_config->SetExecutable(m_filePickerExe->GetPath());
    m_config->SetEnvironment(m_stcEnv->GetText());
    m_config->SetArgs(m_textCtrlArgs->GetValue());
    m_config->SetCompiler(m_choiceCompiler->GetStringSelection());
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
