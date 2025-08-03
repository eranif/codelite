#include "NodeJSNewWorkspaceDlg.h"
#include "cl_standard_paths.h"

NodeJSNewWorkspaceDlg::NodeJSNewWorkspaceDlg(wxWindow* parent)
    : NodeJSNewWorkspaceDlgBase(parent)
{
    m_dirPickerFolder->SetPath(clStandardPaths::Get().GetDocumentsDir());
    CenterOnParent();
}

void NodeJSNewWorkspaceDlg::OnFolderSelected(wxFileDirPickerEvent& event)
{
    event.Skip();
    if(m_textCtrllName->IsEmpty()) {
        wxFileName fn(m_dirPickerFolder->GetPath(), "");
        if(fn.GetDirCount()) {
            m_textCtrllName->ChangeValue(fn.GetDirs().Last());
        }
    }
    UpdatePreview();
}

void NodeJSNewWorkspaceDlg::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName fn(m_staticTextPreview->GetLabel());
    event.Enable(fn.IsOk() && !m_textCtrllName->GetValue().IsEmpty());
}

void NodeJSNewWorkspaceDlg::OnTextUpdate(wxCommandEvent& event)
{
    event.Skip();
    UpdatePreview();
}

void NodeJSNewWorkspaceDlg::UpdatePreview()
{
    // An empty new workspace
    wxFileName fn(m_dirPickerFolder->GetPath(), m_textCtrllName->GetValue());
    fn.SetExt("workspace");
    if(m_checkBoxNewFolder->IsChecked() && !m_textCtrllName->GetValue().IsEmpty()) {
        fn.AppendDir(m_textCtrllName->GetValue());
    }
    m_staticTextPreview->SetLabel(fn.GetFullPath());
    GetSizer()->Layout();
}

void NodeJSNewWorkspaceDlg::OnCheckNewFolder(wxCommandEvent& event) { UpdatePreview(); }
void NodeJSNewWorkspaceDlg::OnExistingFolderSelected(wxFileDirPickerEvent& event) { UpdatePreview(); }
