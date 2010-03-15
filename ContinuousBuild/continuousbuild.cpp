#include "continuousbuild.h"
#include "buildmanager.h"
#include "globals.h"
#include "builder.h"
#include "processreaderthread.h"
#include "fileextmanager.h"
#include "build_settings_config.h"
#include "workspace.h"
#include "custombuildrequest.h"
#include "compile_request.h"
#include <wx/app.h>
#include "continousbuildpane.h"
#include <wx/xrc/xmlres.h>
#include "continousbuildconf.h"
#include <wx/log.h>
#include <wx/imaglist.h>

#if 0
#define PRINT_MESSAGE(x) wxPrintf(x);
#else
#define PRINT_MESSAGE(x) (void)x;
#endif

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
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  ContinuousBuild::OnBuildProcessOutput)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, ContinuousBuild::OnBuildProcessEnded)
END_EVENT_TABLE()

static const wxString CONT_BUILD = wxT("BuildQ");

ContinuousBuild::ContinuousBuild(IManager *manager)
		: IPlugin(manager)
		, m_buildInProgress(false)
{
	m_longName = wxT("Continuous build plugin which compiles files on save and report errors");
	m_shortName = wxT("ContinuousBuild");
	m_view = new ContinousBuildPane(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

	wxBookCtrlBase *book = m_mgr->GetOutputPaneNotebook();
	int imgId = book->GetImageList()->Add(LoadBitmapFile(wxT("compfile.png")));
	
	// add our page to the output pane notebook
	book->AddPage(m_view, CONT_BUILD, false, imgId);

	m_topWin = m_mgr->GetTheApp();
	m_topWin->Connect(wxEVT_FILE_SAVED,               wxCommandEventHandler(ContinuousBuild::OnFileSaved),           NULL, this);
	m_topWin->Connect(wxEVT_FILE_SAVE_BY_BUILD_START, wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved),     NULL, this);
	m_topWin->Connect(wxEVT_FILE_SAVE_BY_BUILD_END,   wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

ContinuousBuild::~ContinuousBuild()
{
}

clToolBar *ContinuousBuild::CreateToolBar(wxWindow *parent)
{
	// Create the toolbar to be used by the plugin
	clToolBar *tb(NULL);
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
			m_mgr->GetOutputPaneNotebook()->RemovePage(i);
			m_view->Destroy();
			break;
		}
	}
	m_topWin->Disconnect(wxEVT_FILE_SAVED,               wxCommandEventHandler(ContinuousBuild::OnFileSaved),           NULL, this);
	m_topWin->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_START, wxCommandEventHandler(ContinuousBuild::OnIgnoreFileSaved),     NULL, this);
	m_topWin->Disconnect(wxEVT_FILE_SAVE_BY_BUILD_END,   wxCommandEventHandler(ContinuousBuild::OnStopIgnoreFileSaved), NULL, this);
}

void ContinuousBuild::OnFileSaved(wxCommandEvent& e)
{
	e.Skip();
	PRINT_MESSAGE(wxT("ContinuousBuild::OnFileSaved\n"));
	// Dont build while the main build is in progress
	if (m_buildInProgress) {
		PRINT_MESSAGE(wxT("Build already in progress, skipping\n"));
		return;
	}

	ContinousBuildConf conf;
	m_mgr->GetConfigTool()->ReadObject(wxT("ContinousBuildConf"), &conf);

	if (conf.GetEnabled()) {
		wxString *fileName = (wxString*) e.GetClientData();
		if(fileName)
			DoBuild(*fileName);
	} else {
		PRINT_MESSAGE(wxT("ContinuousBuild is disabled\n"));
	}
}

void ContinuousBuild::DoBuild(const wxString& fileName)
{
	PRINT_MESSAGE(wxT("DoBuild\n"));
	// Make sure a workspace is opened
	if (!m_mgr->IsWorkspaceOpen()) {
		PRINT_MESSAGE(wxT("No workspace opened!\n"));
		return;
	}
		

	// Filter non source files
	FileExtManager::FileType type = FileExtManager::GetType(fileName);
	switch(type) {
		case FileExtManager::TypeSourceC:
		case FileExtManager::TypeSourceCpp:
		case FileExtManager::TypeResource:
			break;

		default: {
			PRINT_MESSAGE(wxT("Non source file\n"));
			return;
		}
	}

	wxString projectName = m_mgr->GetProjectNameByFile(fileName);
	if(projectName.IsEmpty()) {
		PRINT_MESSAGE(wxT("Project name is empty\n"));
		return;
	}
	
	wxString errMsg;
	ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, errMsg);
	if(!project){
		PRINT_MESSAGE(wxT("Could not find project for file\n"));
		return;
	}

	// get the selected configuration to be build
	BuildConfigPtr bldConf = m_mgr->GetWorkspace()->GetProjBuildConf( project->GetName(), wxEmptyString );
	if ( !bldConf ) {
		PRINT_MESSAGE(wxT("Failed to locate build configuration\n"));
		return;
	}

	BuilderPtr builder = m_mgr->GetBuildManager()->GetBuilder( wxT( "GNU makefile for g++/gcc" ) );
	if(!builder){
		PRINT_MESSAGE(wxT("Failed to located builder\n"));
		return;
	}

	// Only normal file builds are supported
	if(bldConf->IsCustomBuild()){
		PRINT_MESSAGE(wxT("Build is custom. Skipping\n"));
		return;
	}

	// get the single file command to use
	wxString cmd      = builder->GetSingleFileCmd(projectName, bldConf->GetName(), fileName);
	WrapInShell(cmd);

	if( m_buildProcess.IsBusy() ) {
		// add the build to the queue
		if (m_files.Index(fileName) == wxNOT_FOUND) {
			m_files.Add(fileName);

			// update the UI
			m_view->AddFile(fileName);
		}
		return;
	}
	
	PRINT_MESSAGE(wxString::Format(wxT("cmd:%s\n"), cmd.c_str()));
	if(!m_buildProcess.Execute(cmd, fileName, project->GetFileName().GetPath(), this))
		return;

	// Set some messages
	m_mgr->SetStatusMessage(wxString::Format(wxT("Compiling %s..."), wxFileName(fileName).GetFullName().c_str()), 0);

	// Add this file to the UI queue
	m_view->AddFile(fileName);
}

void ContinuousBuild::OnBuildProcessEnded(wxCommandEvent& e)
{
	// remove the file from the UI
	ProcessEventData *ped = (ProcessEventData*)e.GetClientData();
	int exitCode = ped->GetExitCode();
	delete ped;

	m_view->RemoveFile(m_buildProcess.GetFileName());
	wxLogMessage(wxT("Process terminated with exit code %d"), exitCode);
	if(exitCode != 0) {

		m_view->AddFailedFile(m_buildProcess.GetFileName());
	}

	m_mgr->SetStatusMessage(wxEmptyString, 0);

	// Release the resources allocted for this build
	m_buildProcess.Stop();

	// if the queue is not empty, start another build
	if (m_files.IsEmpty() == false) {

		wxString fileName = m_files.Item(0);
		m_files.RemoveAt(0);

		DoBuild(fileName);
	}
}

void ContinuousBuild::StopAll()
{
	// empty the queue
	m_files.Clear();
	m_buildProcess.Stop();
}

void ContinuousBuild::OnIgnoreFileSaved(wxCommandEvent& e)
{
	e.Skip();

	m_buildInProgress = true;

	// Clear the queue
	m_files.Clear();

	// Clear the view
	m_view->ClearAll();
}

void ContinuousBuild::OnStopIgnoreFileSaved(wxCommandEvent& e)
{
	e.Skip();
	m_buildInProgress = false;
}

void ContinuousBuild::OnBuildProcessOutput(wxCommandEvent& e)
{
	ProcessEventData *ped = (ProcessEventData*)e.GetClientData();
	wxLogMessage(ped->GetData());
	delete ped;
}

