#include "svnoptionsdlg.h"

SvnOptionsDlg::SvnOptionsDlg( wxWindow* parent, const SvnOptions &options)
:
SvnOptionsBaseDlg( parent )
{
	m_options = options;
	m_filePicker->SetPath(m_options.GetExePath());
	m_checkBoxUpdateOnSave->SetValue(m_options.GetFlags() & SVN_UPDATE_ON_SAVE ? true : false);
	m_checkBoxAutoAddNewFiles->SetValue(m_options.GetFlags() & SVN_AUTO_ADD_FILE ? true : false);
	m_spinCtrl1->SetValue((int)m_options.GetRefreshInterval());
}

void SvnOptionsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	m_options.SetExePath(m_filePicker->GetPath());
	int interval = m_spinCtrl1->GetValue();
	if(interval < 500){
		interval = 500;
	}

	m_options.SetRefreshInterval((size_t)interval);
	EndModal(wxID_OK);
}

void SvnOptionsDlg::OnUpdateOnSave( wxCommandEvent& event )
{
	if(event.IsChecked()){
		m_options.SetFlags(m_options.GetFlags() | SVN_UPDATE_ON_SAVE);
	}else{
		m_options.SetFlags(m_options.GetFlags() & ~(SVN_UPDATE_ON_SAVE));
	}
}

void SvnOptionsDlg::OnAutoAddNewFiles(wxCommandEvent &event)
{
	if(event.IsChecked()){
		m_options.SetFlags(m_options.GetFlags() | SVN_AUTO_ADD_FILE);
	}else{
		m_options.SetFlags(m_options.GetFlags() & ~(SVN_AUTO_ADD_FILE));
	}
}
