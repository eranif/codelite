#include "svn_copy_dialog.h"
#include "commit_dialog.h"  // NormalizeMessage()

SvnCopyDialog::SvnCopyDialog( wxWindow* parent )
:
SvnCopyDialogBase( parent )
{

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
	return CommitDialog::NormalizeMessage(m_textCtrlComment->GetValue());
}

wxString SvnCopyDialog::GetSourceURL()
{
	return m_textCtrlSourceURL->GetValue();
}

wxString SvnCopyDialog::GetTargetURL()
{
	return m_textCtrlTargetURL->GetValue();
}
