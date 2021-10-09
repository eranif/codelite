#include "NewFileComparison.h"
#include <wx/filedlg.h>
#include <algorithm>
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"

NewFileComparison::NewFileComparison(wxWindow* parent, const wxFileName& leftFile)
    : NewFileComparisonBase(parent)
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    m_textCtrlLeftFile->ChangeValue(leftFile.GetFullPath());
    std::for_each(editors.begin(), editors.end(), [&](IEditor* editor) {
        if(editor->GetFileName() != leftFile) {
            m_listBox16->Append(editor->GetFileName().GetFullPath());
        }
    });
}

NewFileComparison::~NewFileComparison() {}

void NewFileComparison::OnBrowse(wxCommandEvent& event)
{
    static wxString lastPath;
    wxString initialPath;
    m_textCtrlFileName->IsEmpty() ? initialPath = lastPath : initialPath =
                                                                 wxFileName(m_textCtrlFileName->GetValue()).GetPath();
    wxString file = wxFileSelector(_("Select file:"), initialPath);
    if(!file.IsEmpty()) {
        wxFileName selectedFile(file);
        lastPath = selectedFile.GetPath();
        m_textCtrlFileName->ChangeValue(selectedFile.GetFullPath());
    }
}

void NewFileComparison::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName fn(m_textCtrlFileName->GetValue());
    event.Enable(fn.Exists() || m_textCtrlFileName->GetValue().StartsWith(_("Untitled")));
}

void NewFileComparison::OnFileSelected(wxCommandEvent& event)
{
    wxFileName fn(m_listBox16->GetStringSelection());
    m_textCtrlFileName->ChangeValue(fn.GetFullPath());
}
