#include "newunittestdlg.h"

NewUnitTestDlg::NewUnitTestDlg( wxWindow* parent )
:
NewUnitTestBaseDlg( parent )
{
	m_textCtrlTestName->SetFocus();
}

void NewUnitTestDlg::OnUseFixture( wxCommandEvent& event )
{
	if(event.IsChecked()) {
		m_textCtrlFixtureName->Enable();
		m_staticText32->Enable();
	} else {
		m_textCtrlFixtureName->Disable();
		m_staticText32->Disable();
	}
}
