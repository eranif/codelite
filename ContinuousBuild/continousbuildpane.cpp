#include "continousbuildpane.h"
#include "continuousbuild.h"
#include "continousbuildconf.h"
#include <wx/msgdlg.h>
#include "imanager.h"

ContinousBuildPane::ContinousBuildPane( wxWindow* parent, IManager *manager, ContinuousBuild *plugin )
		: ContinousBuildBasePane( parent )
		, m_mgr(manager)
		, m_plugin(plugin)
{
	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);
	m_checkBox1->SetValue(conf.GetEnabled());
}

void ContinousBuildPane::OnStopAll( wxCommandEvent& event )
{
	m_listBoxQueue->Clear();
	m_listBoxFailedFiles->Clear();
	m_plugin->StopAll();
}

void ContinousBuildPane::OnStopUI( wxUpdateUIEvent& event )
{
	event.Enable(!m_listBoxQueue->IsEmpty()/* || !m_listBoxFailedFiles->IsEmpty()*/);
}

void ContinousBuildPane::DoUpdateConf()
{
	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

	conf.SetEnabled(m_checkBox1->IsChecked());
	m_mgr->GetConfigTool()->WriteObject(wxT("ContinousBuildConf"), &conf);
}

void ContinousBuildPane::RemoveFile(const wxString& file)
{
	int where = m_listBoxQueue->FindString(file);
	if (where != wxNOT_FOUND) {
		m_listBoxQueue->Delete((unsigned int)where);
	}
}

void ContinousBuildPane::AddFile(const wxString& file)
{
	if (m_listBoxQueue->FindString(file) == wxNOT_FOUND) {
		m_listBoxQueue->Append(file);
	}
}

void ContinousBuildPane::ClearAll()
{
	m_listBoxQueue->Clear();
	m_listBoxFailedFiles->Clear();
}

void ContinousBuildPane::AddFailedFile(const wxString& file)
{
	if (m_listBoxFailedFiles->FindString(file) == wxNOT_FOUND) {
		m_listBoxFailedFiles->Append(file);
	}
}

void ContinousBuildPane::OnEnableContBuildUI(wxUpdateUIEvent& event)
{
	event.Enable(m_checkBox1->IsChecked());
}

void ContinousBuildPane::OnEnableCB(wxCommandEvent& event)
{
	ContinousBuildConf conf;
	conf.SetEnabled(event.IsChecked());
	m_mgr->GetConfigTool()->WriteObject(wxT("ContinousBuildConf"), &conf);
}
