#include "svn_copy_dialog.h"
#include "SvnCommitDialog.h"

SvnCopyDialog::SvnCopyDialog( wxWindow* parent )
: SvnCopyDialogBase( parent )
{
	m_textCtrlTargetURL->SetFocus();
	m_textCtrlTargetURL->SelectAll();
	m_textCtrlTargetURL->SetInsertionPointEnd();
}

void SvnCopyDialog::SetSourceURL(const wxString& url)
{
	m_textCtrlSourceURL->SetValue(url);
}

void SvnCopyDialog::SetTargetURL(const wxString& url)
{
	m_textCtrlTargetURL->SetValue(url);
}

wxString SvnCopyDialog::GetMessage()
{
	return SvnCommitDialog::NormalizeMessage(m_textCtrlComment->GetValue());
}

wxString SvnCopyDialog::GetSourceURL()
{
	return m_textCtrlSourceURL->GetValue();
}

wxString SvnCopyDialog::GetTargetURL()
{
	return m_textCtrlTargetURL->GetValue();
}
