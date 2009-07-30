#include "newqtprojdlg.h"
#include "qmakesettingsdlg.h"
#include "qmakeconf.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include "workspace.h"
#include <wx/dirdlg.h>

NewQtProjDlg::NewQtProjDlg( wxWindow* parent, QmakeConf *conf, IManager *mgr )
		: NewQtProjBaseDlg( parent )
		, m_conf(conf)
		, m_mgr (mgr)
{
	m_choiceQmake->Append(m_conf->GetAllConfigurations());
	if (m_choiceQmake->IsEmpty() == false) {
		m_choiceQmake->SetSelection(0);
	}

	if( m_mgr->IsWorkspaceOpen() ) {
		m_textCtrl->SetValue( m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath() );
	}

	WindowAttrManager::Load(this, wxT("NewQtProjDlg"), m_mgr->GetConfigTool());
}

NewQtProjDlg::~NewQtProjDlg()
{
	WindowAttrManager::Save(this, wxT("NewQtProjDlg"), m_mgr->GetConfigTool());
}

void NewQtProjDlg::OnBrowseProjectPath( wxCommandEvent& event )
{
	wxUnusedVar (event);

	wxString initPath;
	if ( m_mgr->IsWorkspaceOpen() ) {
		initPath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath();
	}
	wxString new_path = wxDirSelector(wxT("Select directory:"), initPath, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);
	if (new_path.IsEmpty() == false) {
		m_textCtrl->SetValue(new_path);
	}
}

void NewQtProjDlg::OnNewQmakeSettings( wxCommandEvent& event )
{
	QMakeSettingsDlg dlg(this, m_mgr, m_conf);
	if (dlg.ShowModal() == wxID_OK) {
		m_choiceQmake->Clear();
		m_choiceQmake->Append(m_conf->GetAllConfigurations());
		if (m_choiceQmake->IsEmpty() == false) {
			m_choiceQmake->SetSelection(0);
		}
	}
}

bool NewQtProjDlg::GetCreateDirectory() const
{
	return m_checkBoxUseSepDirectory->IsChecked();
}

wxString NewQtProjDlg::GetProjectKind() const
{
	return m_choiceProjKind->GetStringSelection();
}

wxString NewQtProjDlg::GetProjectName() const
{
	return m_textCtrlProjName->GetValue();
}

wxString NewQtProjDlg::GetProjectPath() const
{
	return m_textCtrl->GetValue();
}

wxString NewQtProjDlg::GetQmake() const
{
	return m_choiceQmake->GetStringSelection();
}

