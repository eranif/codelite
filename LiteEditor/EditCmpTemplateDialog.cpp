#include "EditCmpTemplateDialog.h"
#include "globals.h"

EditCmpTemplateDialog::EditCmpTemplateDialog(wxWindow* parent)
    : EditCmpTemplateDialogBase(parent)
{
    clSetDialogBestSizeAndPosition(this);
}

EditCmpTemplateDialog::~EditCmpTemplateDialog() {}

void EditCmpTemplateDialog::SetPattern(const wxString& pattern) { m_stc->SetText(pattern); }

wxString EditCmpTemplateDialog::GetPattern() const
{
    wxString pattern = m_stc->GetText();
    pattern.Trim().Trim(false);
    return pattern;
}
