#include "gitFileDiffDlg.h"
#include "gitCommitEditor.h"
#include "windowattrmanager.h"

#include "icons/icon_git.xpm"
#include "globals.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

GitFileDiffDlg::GitFileDiffDlg(wxWindow* parent)
    : GitFileDiffDlgBase(parent)
{
    m_editor->InitStyles();
    SetIcon(wxICON(icon_git));
    WindowAttrManager::Load(this, wxT("GitFileDiffDlg"), NULL);
}

/*******************************************************************************/

GitFileDiffDlg::~GitFileDiffDlg()
{
    WindowAttrManager::Save(this, wxT("GitFileDiffDlg"), NULL);
}

/*******************************************************************************/
void GitFileDiffDlg::SetDiff(const wxString& diff)
{
    m_editor->SetText(diff);
    m_editor->SetReadOnly(true);
}

void GitFileDiffDlg::OnSaveAsPatch(wxCommandEvent& event)
{
    wxString path = ::wxFileSelector(_("Save as"), "", "untitled", "patch", wxFileSelectorDefaultWildcardStr, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if ( !path.IsEmpty() ) {
        ::WriteFileWithBackup(path, m_editor->GetText(), false);
        ::wxMessageBox( "Diff written to:\n" + path, "CodeLite");
        // Close the dialog
        CallAfter( &GitFileDiffDlg::EndModal, wxID_CLOSE );
    } 
    // do nothing
}

void GitFileDiffDlg::OnCloseDialog(wxCommandEvent& event)
{
    EndModal( wxID_OK );
}
