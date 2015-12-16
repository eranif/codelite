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
    std::for_each(editors.begin(), editors.end(),
        [&](IEditor* editor) { m_listBox16->Append(editor->GetFileName().GetFullPath()); });
}

NewFileComparison::~NewFileComparison() {}

void NewFileComparison::OnBrowse(wxCommandEvent& event)
{
    wxString file = wxFileSelector(wxT("Select file:"));
    if(!file.IsEmpty()) {
        m_textCtrlFileName->ChangeValue(file);
    }
}

void NewFileComparison::OnOKUI(wxUpdateUIEvent& event)
{
    wxFileName fn(m_textCtrlFileName->GetValue());
    event.Enable(fn.Exists());
}

void NewFileComparison::OnFileSelected(wxCommandEvent& event)
{
    wxFileName fn(m_listBox16->GetStringSelection());
    m_textCtrlFileName->ChangeValue(fn.GetFullPath());
}
