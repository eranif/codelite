#include "copyrights_options_dlg.h"
#include <wx/filedlg.h>
#include "copyrightsconfigdata.h"
#include "iconfigtool.h"

CopyrightsOptionsDlg::CopyrightsOptionsDlg( wxWindow* parent, IConfigTool *config )
		: CopyrightsOptionsBaseDlg( parent )
		, m_conf(config)
{
	CopyrightsConfigData data;
	m_conf->ReadObject(wxT("CopyrightsConfig"), &data);
	
	m_textCtrlFileMaksing->SetValue( data.GetFileMasking());
	m_textCtrlFileName->SetValue(data.GetTemplateFilename());
	m_checkBoxBackup->SetValue( data.GetBackupFiles() );
	m_textCtrlFileName->SetFocus();
	
	Centre();
}

void CopyrightsOptionsDlg::OnSelectFile( wxCommandEvent& event )
{
	// open file selection
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Choose a file:"));
	if(dlg->ShowModal() == wxID_OK)
	{
		// Get the dirname
		wxString path = dlg->GetPath();
		m_textCtrlFileName->SetValue(path);
	}
	dlg->Destroy();
}

void CopyrightsOptionsDlg::OnButtonSave(wxCommandEvent& event)
{
	wxUnusedVar(event);

	CopyrightsConfigData data;
	data.SetFileMasking( m_textCtrlFileMaksing->GetValue() );
	data.SetTemplateFilename( m_textCtrlFileName->GetValue() );
	data.SetBackupFiles( m_checkBoxBackup->IsChecked() );
	
	m_conf->WriteObject(wxT("CopyrightsConfig"), &data);
	EndModal(wxID_OK);
}
