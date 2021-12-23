#include "ImportThemesDialog.h"

ImportThemesDialog::ImportThemesDialog(wxWindow* parent)
    : ImportThemesDialogBase(parent)
{
}

ImportThemesDialog::~ImportThemesDialog() {}

wxString ImportThemesDialog::GetInputDirectory() const { return m_dirPickerIn->GetPath(); }

wxString ImportThemesDialog::GetOutputDirectory() const { return m_dirPickerOut->GetPath(); }
