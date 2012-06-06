#include "imanager.h"
#include "callgraph.h"
#include "uisettingsdlg.h"
#include <wx/filedlg.h>
#include "windowattrmanager.h"

uisettingsdlg::uisettingsdlg( wxWindow* parent, IManager *mgr)//, CallGraph *plugin )
	: uisettings(parent)
	, m_mgr(mgr)
{
	//ConfCallGraph confData;
	// read last stored value for paths gprof and dot
	m_mgr->GetConfigTool()->ReadObject(wxT("CallGraph"), &confData);
	m_textCtrl_path_gprof->SetValue(confData.GetGprofPath());
	m_textCtrl_path_dot->SetValue(confData.GetDotPath());
	m_spinCtrl_treshold_edge->SetValue(confData.GetTresholdEdge());
	m_spinCtrl_treshold_node->SetValue(confData.GetTresholdNode());
	m_spinCtrl_colors_node->SetValue(confData.GetColorsNode());
	m_spinCtrl_colors_edge->SetValue(confData.GetColorsEdge());
	m_checkBox_Names->SetValue(confData.GetBoxName());
	m_checkBox_Parametrs->SetValue(confData.GetBoxParam());
	//
	if(m_checkBox_Names->IsChecked()) m_checkBox_Parametrs->Disable();
	if(m_checkBox_Parametrs->IsChecked()) m_checkBox_Names->Disable();
	//

	GetSizer()->Fit(this);
	WindowAttrManager::Load(this, wxT("CGUiSettingsDlg"), NULL);

}

uisettingsdlg::~uisettingsdlg()
{
	WindowAttrManager::Save(this, wxT("CGUiSettingsDlg"), NULL);
}

void uisettingsdlg::OnButton_click_select_gprof( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_gprof_path = wxFileSelector(_("Select gprof..."), m_textCtrl_path_gprof->GetValue().c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (!new_gprof_path.IsEmpty()) {
		m_textCtrl_path_gprof->SetValue(new_gprof_path);
	}
}

void uisettingsdlg::OnButton_click_select_dot( wxCommandEvent& event )
{
	wxUnusedVar(event);
	wxString new_dot_path = wxFileSelector(_("Select dot..."), m_textCtrl_path_dot->GetValue().c_str(), wxT(""), wxT(""), wxFileSelectorDefaultWildcardStr, 0, this);
	if (!new_dot_path.IsEmpty()) {
		m_textCtrl_path_dot->SetValue(new_dot_path);
	}
}

void uisettingsdlg::OnButton_click_ok( wxCommandEvent& event )
{
	wxUnusedVar(event);
	// store values
	//ConfCallGraph confData;
	confData.SetGprofPath(m_textCtrl_path_gprof->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
	//
	confData.SetDotPath(m_textCtrl_path_dot->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
	//
	confData.SetTresholdNode(m_spinCtrl_treshold_node->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
	//
	confData.SetTresholdEdge(m_spinCtrl_treshold_edge->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
	//
	confData.SetColorsEdge(m_spinCtrl_colors_edge->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);
	//
	confData.SetColorsNode(m_spinCtrl_colors_node->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);

	confData.SetBoxName(m_checkBox_Names->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);

	confData.SetBoxParam(m_checkBox_Parametrs->GetValue());
	m_mgr->GetConfigTool()->WriteObject(wxT("CallGraph"), &confData);


	if ((wxFileExists(m_textCtrl_path_gprof->GetValue())) && (wxFileExists(m_textCtrl_path_dot->GetValue()))) {
		EndModal(wxID_OK);
	} else {
		wxMessageBox(_("Please check settings for plugin."), wxT("CallGraph"), wxOK | wxICON_INFORMATION, m_mgr->GetTheApp()->GetTopWindow());
	}

	//event.Skip();
}

void uisettingsdlg::OnButton_click_cancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void uisettingsdlg::OnCheckName(wxCommandEvent& event)
{
	if(m_checkBox_Names->IsChecked())
		m_checkBox_Parametrs->Disable();
	else m_checkBox_Parametrs->Enable();
}

void uisettingsdlg::OnCheckParam(wxCommandEvent& event)
{
	if(m_checkBox_Parametrs->IsChecked())
		m_checkBox_Names->Disable();
	else m_checkBox_Names->Enable();
}
