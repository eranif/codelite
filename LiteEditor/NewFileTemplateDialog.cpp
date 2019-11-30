#include "NewFileTemplateDialog.h"
#include "globals.h"

NewFileTemplateDialog::NewFileTemplateDialog(wxWindow* parent)
    : NewFileTemplateDialogBase(parent)
{
    clSetDialogBestSizeAndPosition(this);
}

NewFileTemplateDialog::~NewFileTemplateDialog() {}

wxString NewFileTemplateDialog::GetPattern() const
{
    wxString pattern = m_stc->GetText();
    pattern.Trim().Trim(false);
    return pattern;
}

wxString NewFileTemplateDialog::GetKind() const { return m_choiceFileType->GetStringSelection(); }

wxString NewFileTemplateDialog::GetExtension() const { return m_textCtrlExtension->GetValue(); }

void NewFileTemplateDialog::SetPattern(const wxString& s) { m_stc->SetText(s); }

void NewFileTemplateDialog::SetKind(const wxString& k) { m_choiceFileType->SetStringSelection(k); }

void NewFileTemplateDialog::SetExtension(const wxString& ext) { m_textCtrlExtension->SetValue(ext); }
