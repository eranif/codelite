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
	int cpus = wxThread::GetCPUCount();
	for (int i=1; i<=cpus-1; i++) {
		m_choiceNumberOfJobs->Append(wxString::Format(wxT("%d"), i));
	}
	m_choiceNumberOfJobs->SetSelection(0);
	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);
	
	m_checkBox1->SetValue(conf.GetEnabled());
}

void ContinousBuildPane::OnEnableCB( wxCommandEvent& event )
{
	if(event.IsChecked() && wxThread::GetCPUCount() == 1) {
		if(wxMessageBox(_("Your computer only has one CPU, are you sure you want to enable the continous build feature?"), wxT("CodeLite"), wxYES_NO|wxICON_QUESTION|wxCENTER) == wxNO) {
			return;
		}
	}
	
	DoUpdateConf();
}

void ContinousBuildPane::OnChoiceNumberOfJobs( wxCommandEvent& event )
{
	// TODO: Implement OnChoiceNumberOfJobs
}

void ContinousBuildPane::OnChoiceNumberOfJobsUI( wxUpdateUIEvent& event )
{
//	event.Enable(m_checkBox1->IsChecked());
	event.Enable(false);
}

void ContinousBuildPane::OnStopAll( wxCommandEvent& event )
{
	m_listBoxQueue->Clear();
	m_plugin->StopAll();
}

void ContinousBuildPane::OnStopUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBoxQueue->IsEmpty() == false);
}

void ContinousBuildPane::DoUpdateConf()
{
	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

	conf.SetEnabled(m_checkBox1->IsChecked());
	long cpus(1);
	m_choiceNumberOfJobs->GetStringSelection().ToLong(&cpus);
	
	conf.SetParallelProcesses(cpus);
	m_mgr->GetConfigTool()->WriteObject(wxT("ContinousBuildConf"), &conf);
}

void ContinousBuildPane::RemoveFile(const wxString& file)
{
	int where = m_listBoxQueue->FindString(file);
	if(where != wxNOT_FOUND){
		m_listBoxQueue->Delete((unsigned int)where);
	}
}

void ContinousBuildPane::AddFile(const wxString& file)
{
	if(m_listBoxQueue->FindString(file) == wxNOT_FOUND){
		m_listBoxQueue->Append(file);
	}
}

void ContinousBuildPane::SetStatusMessage(const wxString& msg)
{
	m_staticTextStatus->SetLabel(msg);
}
