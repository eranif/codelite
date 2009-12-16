#include "svn_login_dialog.h"

SvnLoginDialog::SvnLoginDialog( wxWindow* parent )
:
SvnLoginDialogBase( parent )
{
	m_textCtrlUsername->SetFocus();
}
