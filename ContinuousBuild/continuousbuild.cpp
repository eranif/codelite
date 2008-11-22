#include "continuousbuild.h"
#include "workspace.h"
#include "custombuildrequest.h"
#include "compile_request.h"
#include <wx/app.h>
#include "continousbuildpane.h"
#include <wx/xrc/xmlres.h>
#include "continousbuildconf.h"

static ContinuousBuild* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new ContinuousBuild(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("eran"));
	info.SetName(wxT("ContinuousBuild"));
	info.SetDescription(wxT("Continuous build plugin which compiles files on save and report errors"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

BEGIN_EVENT_TABLE(ContinuousBuild, IPlugin)
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_ADDLINE, ContinuousBuild::OnShellAddLine)
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_STARTED, ContinuousBuild::OnShellBuildStarted)
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_PROCESS_ENDED, ContinuousBuild::OnShellProcessEnded)
	EVT_COMMAND(wxID_ANY, wxEVT_SHELL_COMMAND_STARTED_NOCLEAN, ContinuousBuild::OnShellBuildStartedNoClean)
END_EVENT_TABLE()

ContinuousBuild::ContinuousBuild(IManager *manager)
		: IPlugin(manager)
		, m_shellProcess(NULL)
{
	m_longName = wxT("Continuous build plugin which compiles files on save and report errors");
	m_shortName = wxT("ContinuousBuild");
	m_view = new ContinousBuildPane(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

	// add our page to the output pane notebook
	m_mgr->GetOutputPaneNotebook()->AddPage(m_view, wxT("Continous Build"), LoadBitmapFile(wxT("compfile.png")), false);

	m_topWin = m_mgr->GetTheApp();
	m_topWin->Connect(wxEVT_FILE_SAVED, wxCommandEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
}

ContinuousBuild::~ContinuousBuild()
{
	if(m_shellProcess) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}
}

wxToolBar *ContinuousBuild::CreateToolBar(wxWindow *parent)
{
	// Create the toolbar to be used by the plugin
	wxToolBar *tb(NULL);
	return tb;
}

void ContinuousBuild::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxUnusedVar(pluginsMenu);
}

void ContinuousBuild::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void ContinuousBuild::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(type);
	wxUnusedVar(menu);
}

void ContinuousBuild::UnPlug()
{
	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
			m_mgr->GetOutputPaneNotebook()->RemovePage(i, false);
			m_view->Destroy();
			break;
		}
	}
	m_topWin->Disconnect(wxEVT_FILE_SAVED, wxCommandEventHandler(ContinuousBuild::OnFileSaved), NULL, this);
}

void ContinuousBuild::OnFileSaved(wxCommandEvent& e)
{
	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

	if (conf.GetEnabled()) {
		wxString *fileName = (wxString*) e.GetClientData();
		if (fileName) {
			DoBuild(*fileName);
		}
	}
}

void ContinuousBuild::DoBuild(const wxString& fileName)
{
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		// add the build to the queue
		if(m_files.Index(fileName) == wxNOT_FOUND){
			m_files.Add(fileName);
			// update the UI
			m_view->AddFile(fileName);
		}
		return;
	}
	
	if ( m_shellProcess ) {
		delete m_shellProcess;
		m_shellProcess = NULL;
	}

	wxString conf;
	
	// get the file's project name
	wxString projectName = m_mgr->GetProjectNameByFile(fileName);
	if(projectName.IsEmpty()) {
		return;
	}
	
	// get the selected configuration to be built
	BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf ( projectName, wxEmptyString );
	if ( !bldConf ) {
		return;
	}
	
	m_currentBuildInfo.project = projectName;
	m_currentBuildInfo.file = fileName;
	
	m_view->AddFile(fileName);
	
	// construct a build command
	QueueCommand info ( projectName, conf, false, QueueCommand::Build );
	if ( bldConf && bldConf->IsCustomBuild() ) {
		info.SetCustomBuildTarget ( wxT ( "Compile Single File" ) );
		info.SetKind ( QueueCommand::CustomBuild );
	}

	switch ( info.GetKind() ) {
	case QueueCommand::Build:
		m_shellProcess = new CompileRequest ( this, info, fileName, false );
		break;
	case  QueueCommand::CustomBuild:
		m_shellProcess = new CustomBuildRequest ( this, info, fileName );
		break;
	}
	m_shellProcess->Process(m_mgr);
}

void ContinuousBuild::OnShellAddLine(wxCommandEvent& e)
{
	// add line to the output pane
	m_currentBuildInfo.output.Add(e.GetString());
}

void ContinuousBuild::OnShellBuildStarted(wxCommandEvent& e)
{
	m_view->SetStatusMessage(_("Compiling file: ") + m_currentBuildInfo.file);
}

void ContinuousBuild::OnShellBuildStartedNoClean(wxCommandEvent& e)
{
	m_view->SetStatusMessage(_("Compiling file: ") + m_currentBuildInfo.file);
}

void ContinuousBuild::OnShellProcessEnded(wxCommandEvent& e)
{
	// remove the file from the UI
	m_view->RemoveFile(m_currentBuildInfo.file);
	m_view->SetStatusMessage(wxEmptyString);
	
	// TODO:: handle the output here
	m_currentBuildInfo.Clear();
	
	// if the queue is not empty, start another build
	if(m_files.IsEmpty() == false) {
		
		wxString fileName = m_files.Item(0);
		m_files.RemoveAt(0);
		
		DoBuild(fileName);
	}
}

void ContinuousBuild::StopAll()
{
	// empty the queue
	m_files.Clear();
	
	if ( m_shellProcess && m_shellProcess->IsBusy() ) {
		m_shellProcess->Stop();
	}
}
