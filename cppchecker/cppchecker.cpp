#include <wx/app.h>
#include "imanager.h"
#include "procutils.h"
#include "cppcheckreportpage.h"
#include "cppchecksettingsdlg.h"
#include <wx/process.h>
#include <wx/dir.h>
#include "custom_notebook.h"
#include "workspace.h"
#include "project.h"
#include "fileextmanager.h"
#include "jobqueue.h"
#include "cppchecker.h"
#include "cl_process.h"
#include <wx/stdpaths.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/log.h>

static CppCheckPlugin* thePlugin = NULL;

const wxEventType wxEVT_CPPCHECK_START_DAEMON = XRCID("cppcheck_report");

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new CppCheckPlugin(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah & Jérémie (jfouche)"));
	info.SetName(wxT("CppChecker"));
	info.SetDescription(wxT("CppCheker integration for CodeLite IDE"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

CppCheckPlugin::CppCheckPlugin(IManager *manager)
		: IPlugin(manager)
		, m_cppcheckProcess(NULL)
		, m_canRestart( true )
		, m_explorerSepItem(NULL)
		, m_workspaceSepItem(NULL)
		, m_projectSepItem(NULL)
		, m_view(NULL)
		, m_analysisInProgress(false)
		, m_fileCount(0)
#ifdef __WXMSW__
		, m_restartRequired(false)
#endif // __WXMSW__
		, m_fileProcessed(1)
{
	FileExtManager::Init();

	m_longName = wxT("CppCheck intergration for CodeLite IDE");
	m_shortName = wxT("CppCheck");

	// Load settings
	m_mgr->GetConfigTool()->ReadObject(wxT("CppCheck"), &m_settings);

	// Connect events
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

	m_mgr->GetTheApp()->Connect(wxEVT_CPPCHECKJOB_STATUS_MESSAGE,  wxCommandEventHandler(CppCheckPlugin::OnStatusMessage),  NULL, this);
	m_mgr->GetTheApp()->Connect(wxEVT_CPPCHECKJOB_CHECK_COMPLETED, wxCommandEventHandler(CppCheckPlugin::OnCheckCompleted), NULL, this);
	m_mgr->GetTheApp()->Connect(wxEVT_CPPCHECKJOB_REPORT,          wxCommandEventHandler(CppCheckPlugin::OnReport),         NULL, this);
	m_mgr->GetTheApp()->Connect(wxEVT_INIT_DONE,                   wxCommandEventHandler(CppCheckPlugin::OnStartDaemon),    NULL, this);
	m_mgr->GetTheApp()->Connect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);

	m_view = new CppCheckReportPage(m_mgr->GetOutputPaneNotebook(), m_mgr, this);
	m_mgr->GetOutputPaneNotebook()->AddPage(m_view, wxT("CppCheck"), wxT("CppCheck"), LoadBitmapFile(wxT("cppcheck.png")), false);
}

CppCheckPlugin::~CppCheckPlugin()
{
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

	m_mgr->GetTheApp()->Disconnect(wxEVT_CPPCHECKJOB_STATUS_MESSAGE,  wxCommandEventHandler(CppCheckPlugin::OnStatusMessage),  NULL, this);
	m_mgr->GetTheApp()->Disconnect(wxEVT_CPPCHECKJOB_CHECK_COMPLETED, wxCommandEventHandler(CppCheckPlugin::OnCheckCompleted), NULL, this);
	m_mgr->GetTheApp()->Disconnect(wxEVT_CPPCHECKJOB_REPORT,          wxCommandEventHandler(CppCheckPlugin::OnReport),         NULL, this);
	m_mgr->GetTheApp()->Disconnect(wxEVT_INIT_DONE,                   wxCommandEventHandler(CppCheckPlugin::OnStartDaemon),    NULL, this);
	m_mgr->GetTheApp()->Disconnect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);
}

wxToolBar *CppCheckPlugin::CreateToolBar(wxWindow *parent)
{
	// Create the toolbar to be used by the plugin
	wxToolBar *tb(NULL);
	return tb;
}

void CppCheckPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu* menu = new wxMenu();
	wxMenuItem* item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, _("CppCheck"), menu);
}

void CppCheckPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Append items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {

		if (!menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"))) {
			m_explorerSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_EXPLORER_POPUP"), wxT("CppCheck"), CreateFileExplorerPopMenu());
		}

	} else if (type == MenuTypeFileView_Workspace) {

		if (!menu->FindItem(XRCID("CPPCHECK_WORKSPACE_POPUP"))) {
			m_workspaceSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_WORKSPACE_POPUP"), wxT("CppCheck"), CreateWorkspacePopMenu());
		}

	} else if (type == MenuTypeFileView_Project) {

		if (!menu->FindItem(XRCID("CPPCHECK_PROJECT_POPUP"))) {
			m_projectSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_PROJECT_POPUP"), wxT("CppCheck"), CreateProjectPopMenu());
		}

	} else if (type == MenuTypeFileView_Folder) {
		//TODO::Append items for the file view/Virtual folder context menu
	} else if (type == MenuTypeFileView_File) {
		//TODO::Append items for the file view/file context menu
	}
}

void CppCheckPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Unhook items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {

		wxMenuItem *item = menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_explorerSepItem);
			m_explorerSepItem = NULL;
		}

	} else if (type == MenuTypeFileView_Workspace) {

		wxMenuItem *item = menu->FindItem(XRCID("CPPCHECK_WORKSPACE_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_workspaceSepItem);
			m_workspaceSepItem = NULL;
		}

	} else if (type == MenuTypeFileView_Project) {

		wxMenuItem *item = menu->FindItem(XRCID("CPPCHECK_PROJECT_POPUP"));
		if (item) {
			menu->Destroy(item);
			menu->Destroy(m_projectSepItem);
			m_projectSepItem = NULL;
		}

	}
}

void CppCheckPlugin::UnPlug()
{
	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
			m_mgr->GetOutputPaneNotebook()->RemovePage(i, false);
			m_view->Destroy();
			break;
		}
	}

	// terminate the cppcheck daemon
	if ( m_cppcheckProcess ) {
		wxLogMessage(wxT("CppCheckPlugin: Terminating cppcheck daemon..."));
		m_canRestart = false;

		// detach the process from the plugin and terminate it
		m_cppcheckProcess->Disconnect(m_cppcheckProcess->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(CppCheckPlugin::OnCppCheckTerminated), NULL, this);
		m_cppcheckProcess->Terminate();
	}
}

wxMenu* CppCheckPlugin::CreateFileExplorerPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_fileexplorer_item"), wxT("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}


wxMenu* CppCheckPlugin::CreateProjectPopMenu()
{
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_project_item"), wxT("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu* CppCheckPlugin::CreateWorkspacePopMenu()
{
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_workspace_item"), wxT("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

void CppCheckPlugin::OnCheckFileExplorerItem(wxCommandEvent& e)
{
	if ( m_filelist.IsEmpty() == false ) {
		wxLogMessage(wxT("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
		return;
	}

	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if ( item.m_fileName.IsDir() ) {
		GetFileListFromDir( item.m_fileName.GetFullPath() );
	} else {
		m_filelist.Add( item.m_fileName.GetFullPath() );
	}
	DoStartTest();
}

void CppCheckPlugin::OnCheckWorkspaceItem(wxCommandEvent& e)
{
	if ( m_filelist.IsEmpty() == false ) {
		wxLogMessage(wxT("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
		return;
	}

	if ( !m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen() ) {
		return;
	}

	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	if ( item.m_itemType == ProjectItem::TypeWorkspace ) {

		// retrieve complete list of source files of the workspace
		wxArrayString projects;
		wxString      err_msg;
		std::vector< wxFileName > tmpfiles;
		m_mgr->GetWorkspace()->GetProjectList(projects);

		for (size_t i=0; i< projects.GetCount(); i++) {
			ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
			if ( proj ) {
				proj->GetFiles(tmpfiles, true);
			}
		}

		// only C/C++ files
		for (size_t i=0; i< tmpfiles.size(); i++ ) {
			if (FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceC ||
				FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceCpp) {
				m_filelist.Add( tmpfiles.at(i).GetFullPath() );
			}
		}
	}
	DoStartTest();
}

void CppCheckPlugin::OnCheckProjectItem(wxCommandEvent& e)
{
	if ( m_filelist.IsEmpty() == false ) {
		wxLogMessage(wxT("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
		return;
	}
	if ( !m_mgr->GetWorkspace() || !m_mgr->IsWorkspaceOpen() ) {
		return;
	}

	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
	if ( item.m_itemType == ProjectItem::TypeProject) {

		// retrieve complete list of source files of the workspace
		wxString                  project_name (item.m_text);
		wxString                  err_msg;
		std::vector< wxFileName > tmpfiles;

		ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(project_name, err_msg);
		if ( !proj ) {
			return;
		}

		proj->GetFiles(tmpfiles, true);

		// only C/C++ files
		for (size_t i=0; i< tmpfiles.size(); i++ ) {
			if ( FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceC ||
			     FileExtManager::GetType( tmpfiles.at(i).GetFullPath() ) == FileExtManager::TypeSourceCpp ) {
				m_filelist.Add( tmpfiles.at(i).GetFullPath() );
			}
		}
	}
	DoStartTest();
}

bool CppCheckPlugin::StartCppCheckDaemon()
{
#ifdef __WXMSW__
	// determine the codelite_cppcheck path:
	wxFileName exePath( wxStandardPaths::Get().GetExecutablePath() );
	m_cppcheckPath = exePath.GetPath();

	m_cppcheckPath << wxFileName::GetPathSeparator() << wxT("codelite_cppcheck");

	// build the command
	wxString cmd;
	cmd << wxT("\"") << m_cppcheckPath << wxT("\" ");
	cmd << m_settings.GetOptions();
	cmd << wxT("--daemon=") << wxGetProcessId() << wxT(" --pid ") << wxT(" --xml");
	m_cppcheckProcess = new clProcess(wxNewId(), cmd, false);

	// Launch it!
	m_cppcheckProcess->Start();

	if ( m_cppcheckProcess->GetPid() <= 0 ) {
		m_cppcheckProcess = NULL;
		return false;
	}

	// attach the termination event to the tags manager class
	m_cppcheckProcess->Connect(m_cppcheckProcess->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(CppCheckPlugin::OnCppCheckTerminated), NULL, this);
#else
	return true;
#endif
}

void CppCheckPlugin::OnCppCheckTerminated(wxProcessEvent& e)
{
#ifdef __WXMSW__
	if ( m_canRestart ) {
		wxUnusedVar(e);
		m_cppcheckProcess->Disconnect(m_cppcheckProcess->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(CppCheckPlugin::OnCppCheckTerminated), NULL, this);
		StartCppCheckDaemon();
	}
#else
	wxUnusedVar(e);
#endif
}

/**
 * @brief Send by the worker thread when it has done compelted processing a file
 * the current behavior is to to process the next file on the list
 * @param e
 */
void CppCheckPlugin::OnCheckCompleted(wxCommandEvent& e)
{
	wxUnusedVar(e);
	ProcessNextFromList();
}

/**
 * @brief called by the worker thread, when a complete report for a file
 * is retrieved
 * @param e event
 */
void CppCheckPlugin::OnReport(wxCommandEvent& e)
{
	m_view->AddResults(e.GetString());
}

/**
 * @brief called by the worker thread when it wants to report a message (informative one)
 * @param e
 */
void CppCheckPlugin::OnStatusMessage(wxCommandEvent& e)
{
	m_view->SetStatus(e.GetString());
}

/**
 *
 */
void CppCheckPlugin::OnSettingsItem(wxCommandEvent &e)
{
	CppCheckSettingsDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), &m_settings, m_mgr->GetConfigTool());
	if (dlg.ShowModal() == wxID_OK) {
		m_mgr->GetConfigTool()->WriteObject(wxT("CppCheck"), &m_settings);
#ifdef __WXMSW__
		m_restartRequired = true;
#endif
	}
}

void CppCheckPlugin::GetFileListFromDir(const wxString& root)
{
	m_filelist.Clear();
	wxArrayString tmparr;
	wxDir::GetAllFiles(root, &tmparr);

	for (size_t i=0; i<tmparr.GetCount(); i++) {
		switch (FileExtManager::GetType( tmparr.Item(i) ) ) {
		case FileExtManager::TypeSourceC:
		case FileExtManager::TypeSourceCpp: {
			m_filelist.Add( tmparr.Item(i) );
			break;
		}

		default:
			break;
		}
	}
}

void CppCheckPlugin::ProcessNextFromList()
{
	m_analysisInProgress = !m_filelist.IsEmpty();
	if ( !m_analysisInProgress ) {
		// no more files to process
		m_view->SetStatus(wxT("Done"));

		if ( m_view->GetErrorCount() == 0 ) {
			wxMessageBox(_("No errors were found"), _("cppcheck"));
		}
	}

#ifdef __WXMSW__
	// Restart codelite_cppcheck because we changed settings
	if (m_restartRequired) {
		m_cppcheckProcess->Terminate();
		m_restartRequired = false;
	}
#endif
	DoProcess(1);
}

void CppCheckPlugin::DoProcess(size_t count)
{
	for (size_t i=0; i<count; i++) {
		if ( m_filelist.IsEmpty() == false ) {
			// get the next file from the list and remove it
			wxString next_file = m_filelist.Item(0);
			m_filelist.RemoveAt(0);
			m_fileProcessed++;
			CppCheckJob *j = new CppCheckJob(this, &m_settings);
			j->SetFilename( next_file );
			m_view->SetStatus(wxString::Format(_("Processing %s"), next_file.c_str()));
			m_mgr->GetJobQueue()->PushJob( j );

		}
	}
}

void CppCheckPlugin::OnStartDaemon(wxCommandEvent& e)
{
	// On Windows we work with "daemon" mode since
	// its pretty expensive to launch it for each file
#ifdef __WXMSW__
	StartCppCheckDaemon();
#endif
	e.Skip();
}

/**
 * Ensure that the CppCheck tab is visible
 */
void CppCheckPlugin::SetTabVisible(bool clearContent)
{
	// Make sure that the Output pane is visible
	wxAuiManager *aui = m_mgr->GetDockingManager();
	if (aui) {
		wxAuiPaneInfo &info = aui->GetPane(wxT("Output View"));
		if (info.IsOk() && !info.IsShown()) {
			info.Show();
			aui->Update();
		}
	}

	// Set the focus to the CppCheck tab
	Notebook *book = m_mgr->GetOutputPaneNotebook();
	if (book->GetPageText((size_t)book->GetSelection()) != wxT("CppCheck")) {
		for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
			if (book->GetPageText(i) == wxT("CppCheck")) {
				book->SetSelection(i);
				break;
			}
		}
	}

	// clear the view contents
	if ( clearContent ) {
		m_view->Clear();
		m_fileCount = m_filelist.GetCount();
		m_fileProcessed = 1;
	}
}

void CppCheckPlugin::StopAnalysis()
{
	// Clear the files queue
	m_filelist.Clear();
	m_fileCount = 0;
	m_fileProcessed = 1;

	if (m_cppcheckProcess) {
		// terminate the m_cppcheckProcess
		m_cppcheckProcess->Terminate();
	} else {
#if defined (__WXGTK__) || defined (__WXMAC__)
		wxArrayString out;
		ProcUtils::SafeExecuteCommand(wxT("pidof codelite_cppcheck"), out);
		for( size_t i=0; i<out.GetCount(); i++ ) {
			long pid (0);
			out.Item(i).ToLong( &pid );
			if ( pid > 0 ) {
				wxProcess::Kill(pid, wxSIGKILL);
			}
		}
#endif
	}
}

size_t CppCheckPlugin::GetProgress()
{
	double progress = (((double) m_fileProcessed) / (double) m_fileCount) * 100;
	return (size_t) progress;
}

void CppCheckPlugin::RemoveExcludedFiles()
{
	wxArrayString exclude = m_settings.GetExcludeFiles();

	wxArrayString tmpfiles ( m_filelist );
	m_filelist.Clear();

	for ( size_t i=0; i<tmpfiles.GetCount(); i++ ) {
		wxFileName fn ( tmpfiles.Item(i) );
		if ( exclude.Index(fn.GetFullPath()) == wxNOT_FOUND ) {
			// file does not exist in the excluded files list
			// add it
			m_filelist.Add( tmpfiles.Item(i) );
		}
	}

}


void CppCheckPlugin::SkipCurrentFile()
{
	if (m_cppcheckProcess) {
		// terminate the m_cppcheckProcess
		m_cppcheckProcess->Terminate();
	} else {
#if defined (__WXGTK__) || defined (__WXMAC__)
		wxArrayString out;
		ProcUtils::SafeExecuteCommand(wxT("pidof codelite_cppcheck"), out);
		if ( out.GetCount() == 1 ) {
			long pid (0);
			out.Item(0).ToLong( &pid );
			if ( pid > 0 ) {
				wxProcess::Kill(pid, wxSIGKILL);
			}
		}
#endif
	}
}

void CppCheckPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
	m_view->Clear();
	e.Skip();
}


void CppCheckPlugin::DoStartTest()
{
	RemoveExcludedFiles();
	SetTabVisible(true);
	m_view->Clear();
	ProcessNextFromList();
}
