#include "clFileSystemWorkspaceDlg.h"
#include "globals.h"
#include "ColoursAndFontsManager.h"
#include "clFileSystemWorkspace.hpp"
#include "macros.h"
#include "BuildTargetDlg.h"

clFileSystemWorkspaceDlg::clFileSystemWorkspaceDlg(wxWindow* parent)
    : clFileSystemWorkspaceDlgBase(parent)
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_stcCCFlags); }

    m_dvListCtrlTargets->SetSortFunction([](clRowEntry* a, clRowEntry* b) {
        const wxString& cellA = a->GetLabel(0);
        const wxString& cellB = b->GetLabel(0);
        return (cellA.CmpNoCase(cellB) < 0);
    });

    m_stcCCFlags->SetText(clFileSystemWorkspace::Get().GetCompileFlags());
    m_textCtrlFileExt->ChangeValue(clFileSystemWorkspace::Get().GetFileExtensions());
    const wxStringMap_t& targets = clFileSystemWorkspace::Get().GetBuildTargets();
    for(const auto& vt : targets) {
        wxDataViewItem item = m_dvListCtrlTargets->AppendItem(vt.first);
        m_dvListCtrlTargets->SetItemText(item, vt.second, 1);
    }
    ::clSetDialogBestSizeAndPosition(this);
}

clFileSystemWorkspaceDlg::~clFileSystemWorkspaceDlg() {}

void clFileSystemWorkspaceDlg::OnOK(wxCommandEvent& event)
{
    clFileSystemWorkspace::Get().SetCompileFlags(m_stcCCFlags->GetText().Trim().Trim(false));
    clFileSystemWorkspace::Get().SetFileExtensions(m_textCtrlFileExt->GetValue());

    wxStringMap_t targets;
    for(size_t i = 0; i < m_dvListCtrlTargets->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrlTargets->RowToItem(i);
        wxString name = m_dvListCtrlTargets->GetItemText(item, 0);
        wxString command = m_dvListCtrlTargets->GetItemText(item, 1);
        targets.insert({ name, command });
    }
    clFileSystemWorkspace::Get().SetBuildTargets(targets);
    clFileSystemWorkspace::Get().Save();
    EndModal(wxID_OK);
}

void clFileSystemWorkspaceDlg::OnEditTarget(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);

    BuildTargetDlg dlg(this, m_dvListCtrlTargets->GetItemText(item, 0), m_dvListCtrlTargets->GetItemText(item, 1));
    if(dlg.ShowModal() == wxID_OK) {
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetName(), 0);
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetCommand(), 1);
    }
}

void clFileSystemWorkspaceDlg::OnEditTargetUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlTargets->GetSelectedItemsCount());
}

void clFileSystemWorkspaceDlg::OnNewTarget(wxCommandEvent& event)
{
    BuildTargetDlg dlg(this, "", "");
    if(dlg.ShowModal() == wxID_OK) {
        wxDataViewItem item = m_dvListCtrlTargets->AppendItem(dlg.GetTargetName());
        m_dvListCtrlTargets->SetItemText(item, dlg.GetTargetCommand(), 1);
    }
}

void clFileSystemWorkspaceDlg::OnDelete(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    CHECK_ITEM_RET(item);

    m_dvListCtrlTargets->DeleteItem(m_dvListCtrlTargets->ItemToRow(item));
}

void clFileSystemWorkspaceDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTargets->GetSelection();
    if(item.IsOk()) {
        wxString name = m_dvListCtrlTargets->GetItemText(item, 0);
        event.Enable(name != "build" && name != "clean");

    } else {
        event.Enable(false);
    }
}
