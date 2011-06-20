#include <wx/app.h>
#include "processreaderthread.h"
#include <wx/imaglist.h>
#include <wx/ffile.h>
#include "imanager.h"
#include "procutils.h"
#include "cppcheckreportpage.h"
#include "cppchecksettingsdlg.h"
#include <wx/process.h>
#include <wx/dir.h>
#include "notebook_ex.h"
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
	info.SetDescription(_("CppChecker integration for CodeLite IDE"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}


BEGIN_EVENT_TABLE(CppCheckPlugin, wxEvtHandler)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ,  CppCheckPlugin::OnCppCheckReadData)
	EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, CppCheckPlugin::OnCppCheckTerminated)
END_EVENT_TABLE()

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
		, m_fileProcessed(1)
{
	FileExtManager::Init();

	m_longName = _("CppCheck integration for CodeLite IDE");
	m_shortName = wxT("CppCheck");

	// Load settings
	m_mgr->GetConfigTool()->ReadObject(wxT("CppCheck"), &m_settings);

	// Connect events
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Connect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

	m_mgr->GetTheApp()->Connect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);

	m_view = new CppCheckReportPage(m_mgr->GetOutputPaneNotebook(), m_mgr, this);

//	wxBookCtrlBase *book = m_mgr->GetOutputPaneNotebook();
	m_mgr->GetOutputPaneNotebook()->AddPage(m_view, wxT("CppCheck"), false, LoadBitmapFile(wxT("cppcheck.png")));
}

CppCheckPlugin::~CppCheckPlugin()
{
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_settings_item"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnSettingsItem),          NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_fileexplorer_item"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckFileExplorerItem), NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_workspace_item"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckWorkspaceItem),    NULL, (wxEvtHandler*)this);
	m_mgr->GetTheApp()->Disconnect(XRCID("cppcheck_project_item"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CppCheckPlugin::OnCheckProjectItem),      NULL, (wxEvtHandler*)this);

	m_mgr->GetTheApp()->Disconnect(wxEVT_WORKSPACE_CLOSED,            wxCommandEventHandler(CppCheckPlugin::OnWorkspaceClosed),NULL, this);
}

clToolBar *CppCheckPlugin::CreateToolBar(wxWindow *parent)
{
	// Create the toolbar to be used by the plugin
	clToolBar *tb(NULL);
	return tb;
}

void CppCheckPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu* menu = new wxMenu();
	wxMenuItem* item = new wxMenuItem(menu, XRCID("cppcheck_settings_item"), _("Settings"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("CppCheck"), menu);
}

void CppCheckPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Append items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {

		if (!menu->FindItem(XRCID("CPPCHECK_EXPLORER_POPUP"))) {
			m_explorerSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_EXPLORER_POPUP"), _("CppCheck"), CreateFileExplorerPopMenu());
		}

	} else if (type == MenuTypeFileView_Workspace) {

		if (!menu->FindItem(XRCID("CPPCHECK_WORKSPACE_POPUP"))) {
			m_workspaceSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_WORKSPACE_POPUP"), _("CppCheck"), CreateWorkspacePopMenu());
		}

	} else if (type == MenuTypeFileView_Project) {

		if (!menu->FindItem(XRCID("CPPCHECK_PROJECT_POPUP"))) {
			m_projectSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("CPPCHECK_PROJECT_POPUP"), _("CppCheck"), CreateProjectPopMenu());
		}

	}
}

void CppCheckPlugin::UnPlug()
{
	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_view == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
			m_mgr->GetOutputPaneNotebook()->RemovePage(i);
			m_view->Destroy();
			break;
		}
	}

	// terminate the cppcheck daemon
	if ( m_cppcheckProcess ) {
		wxLogMessage(_("CppCheckPlugin: Terminating cppcheck daemon..."));
		delete m_cppcheckProcess;
		m_cppcheckProcess = NULL;
	}
}

wxMenu* CppCheckPlugin::CreateFileExplorerPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_fileexplorer_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}


wxMenu* CppCheckPlugin::CreateProjectPopMenu()
{
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_project_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

wxMenu* CppCheckPlugin::CreateWorkspacePopMenu()
{
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cppcheck_workspace_item"), _("Run CppCheck"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	return menu;
}

void CppCheckPlugin::OnCheckFileExplorerItem(wxCommandEvent& e)
{
	if ( m_cppcheckProcess ) {
		wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
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
	if ( m_cppcheckProcess ) {
		wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
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
	if ( m_cppcheckProcess ) {
		wxLogMessage(_("CppCheckPlugin: CppCheck is currently busy please wait for it to complete the current check"));
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

void CppCheckPlugin::OnCppCheckTerminated(wxCommandEvent& e)
{
	m_filelist.Clear();
	delete m_cppcheckProcess;
	m_cppcheckProcess = NULL;

	m_view->PrintStatusMessage();
}

/**
 *
 */
void CppCheckPlugin::OnSettingsItem(wxCommandEvent &e)
{
	CppCheckSettingsDialog dlg(m_mgr->GetTheApp()->GetTopWindow(), &m_settings, m_mgr->GetConfigTool());
	if (dlg.ShowModal() == wxID_OK) {
		m_mgr->GetConfigTool()->WriteObject(wxT("CppCheck"), &m_settings);
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

void CppCheckPlugin::DoProcess()
{
	wxString command = DoGetCommand();
	wxLogMessage(_("Starting cppcheck: %s"), command.c_str());

	m_cppcheckProcess = CreateAsyncProcess(this, command);
	if (!m_cppcheckProcess ) {
		wxMessageBox(_("Failed to launch codelite_cppcheck process!"), _("Warning"), wxOK|wxCENTER|wxICON_WARNING);
		return;
	}
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
		for (size_t i=0; i<book->GetPageCount(); i++) {
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
	if (m_cppcheckProcess) {
		// terminate the m_cppcheckProcess
		m_cppcheckProcess->Terminate();
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
	m_view->SetGaugeRange(m_filelist.GetCount());
	// Start the test
	DoProcess();
}

wxString CppCheckPlugin::DoGetCommand()
{
	// Linux / Mac way: spawn the process and execute the command
	wxString cmd, path;
#if defined (__WXMAC__)
	path = wxStandardPaths::Get().GetDataDir();

#else // Linux / Windows
	wxFileName exePath( wxStandardPaths::Get().GetExecutablePath() );
	path = exePath.GetPath();

#endif

	path << wxFileName::GetPathSeparator() << wxT("codelite_cppcheck");
#ifdef __WXMSW__
	path << wxT(".exe");
#endif

	wxString fileList = DoGenerateFileList();
	if(fileList.IsEmpty())
		return wxT("");

	// build the command
	cmd << wxT("\"") << path << wxT("\" ");
	cmd << m_settings.GetOptions();
	cmd << wxT(" --file-list=");
	cmd << wxT("\"") << fileList << wxT("\"");
	return cmd;
}

wxString CppCheckPlugin::DoGenerateFileList()
{
	//create temporary file and save the file there
	wxString wspPath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	wxString list_file( wspPath );
	list_file << wxT("cppcheck.list");

	//create temporary file and save the file there
	wxFFile file(list_file, wxT("w+b"));
	if (!file.IsOpened()) {
		wxMessageBox(_("Failed to open temporary file ") + list_file, _("Warning"), wxOK|wxCENTER|wxICON_WARNING);
		return wxEmptyString;
	}

	wxString content;
	for(size_t i=0; i<m_filelist.GetCount(); i++) {
		content << m_filelist.Item(i) << wxT("\n");
	}

	file.Write( content );
	file.Flush();
	file.Close();

	return list_file;
}

void CppCheckPlugin::OnCppCheckReadData(wxCommandEvent& e)
{
	e.Skip();
	ProcessEventData *ped = (ProcessEventData *) e.GetClientData();
	m_view->AppendLine( ped->GetData() );

	delete ped;
}
