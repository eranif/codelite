#include "precompiled_header.h"
#include "environmentconfig.h"
#include "buidltab.h"
#include "fileexplorer.h"
#include "manager.h"
#include "ctags_manager.h" 
#include "frame.h"
#include "editor.h"
#include "menumanager.h" 
#include "language.h"
#include "editor_config.h"
#include "parse_thread.h"
#include "search_thread.h"
#include "workspace.h"
#include "cpp_symbol_tree.h"
#include "fileview.h"
#include <wx/file.h>
#include "wx/arrstr.h"
#include "context_manager.h"
#include "wx/tokenzr.h"
#include "buildmanager.h"
#include "macros.h"
#include "dirtraverser.h"
#include "depends_dlg.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "editor_creator.h"
#include "algorithm"
#include "async_executable_cmd.h"
#include "fileutils.h"
#include "workspace_pane.h"
#include "close_all_dlg.h"
#include "vcimporter.h"
#include "debuggermanager.h"
#include "shell_window.h"
#include "output_pane.h"
#include "localvarstree.h"
#include "wx/regex.h"
#include "simpletable.h"
#include "quickwatchdlg.h"
#include "fileexplorertree.h"
#include "listctrlpanel.h"
#include "breakpointdlg.h"
#include "globals.h"
#include "exelocator.h"
#include "attachdbgprocdlg.h"
//#include "debuggertip.h"
#include "openwindowspanel.h"
#include "threadlistpanel.h"
#include "plugin.h"

#if defined (__WXMSW__)
static wxString CL_EXEC_WRAPPER = wxT("le_exec.exe ");
#elif defined (__WXGTK__)
static wxString CL_EXEC_WRAPPER = wxT("le_exec.sh ");
#else
static wxString CL_EXEC_WRAPPER = wxEmptyString;
#endif

#define CHECK_MSGBOX(res)										\
	if( !res )													\
	{															\
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
		return;													\
	}

#define CHECK_MSGBOX_BOOL(res)								\
	if( !res )													\
	{															\
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
		return res;												\
	}

static bool HideDebuggerPane = true;

Manager::Manager(void)
		: m_cleanRequest(NULL)
		, m_compileRequest(NULL)
		, m_asyncExeCmd(NULL)
		, m_dbgCanInteract(false)
		, m_quickWatchDlg(NULL)
		, m_frameLineno(wxNOT_FOUND)
		, m_useTipWin(false)
		//, m_debuggerTipWin(NULL)
		, m_tipWinPos(wxNOT_FOUND)
{
}

Manager::~Manager(void)
{
	UnInitialize();
//	if(m_debuggerTipWin){
//		delete m_debuggerTipWin;
//	}
	
	if (m_cleanRequest) {
		delete m_cleanRequest;
		m_cleanRequest = NULL;
	}
	if (m_compileRequest) {
		delete m_compileRequest;
		m_compileRequest = NULL;
	}
}

wxFrame *Manager::GetMainFrame()
{
	return Frame::Get();
}

bool Manager::IsWorkspaceOpen() const
{
	return WorkspaceST::Get()->GetName().IsEmpty() == false;
}

void Manager::OpenFile(const BrowseRecord &rec)
{
	OpenFile(rec.filename, rec.project, rec.lineno, rec.position);
}

bool Manager::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position)
{
	wxFileName fileName(file_name);
	wxString projName(projectName);
	wxFlatNotebook *notebook = Frame::Get()->GetNotebook();
	bool updTree = false;
	bool fileWasOpenedInNB(false);
	int selection(wxNOT_FOUND);
	wxWindow *returnFocusWin(NULL);

	wxWindow *focusWin = wxWindow::FindFocus();
	if (focusWin == Frame::Get()->GetOutputPane()->GetDebugWindow()->GetInWin()) {
		returnFocusWin = focusWin;
	}

	//make sure that the notebook is visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Editor"));
	if ( info.IsOk() && !info.IsShown()) {
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

	// Search to see if this file is already opened
	// in the notebook
	LEditor* editor = NULL;
	size_t nCount = 0;
	for (; nCount < (size_t)notebook ->GetPageCount(); nCount++) {
		editor = dynamic_cast<LEditor*>(notebook ->GetPage(nCount));
		if ( editor ) {
			wxString f1 = editor->GetFileName().GetFullPath();
			wxString f2 = fileName.GetFullPath();
			if (f1 == f2) {
				notebook ->SetSelection( nCount );
				selection = (int)nCount;
				fileWasOpenedInNB = true;
				break;
			}
			editor = NULL;
		}
	}

	//even in cases were a porject name is empty, we try
	//to match a project name to the actual file. otherwise, CC may not work
	//since it depends on a valid project name in the editor
	if (projectName.IsEmpty()) {
		projName = GetProjectNameByFile(fileName.GetFullPath());
	}

	if ( !editor ) {
		/// Open the file and read the text
		if (fileName.IsOk() == false)
			return false;

		// make sure that the file exist
		if ( !fileName.FileExists() )
			return false;

		// Create new editor and add it to the notebook
		notebook->Freeze();
		// create new instance from pool
		editor = EditorCreatorST::Get()->NewInstance();
		editor->Create(projName, fileName);
		editor->SetSyntaxHighlight(editor->GetContext()->GetName());
		notebook->AddPage(editor, fileName.GetFullName(), true);
		notebook->Thaw();
		selection = (int)notebook->GetPageCount()-1;
		updTree = true;
	}

	// Go to tag line number and gives scintilla the focus
	if ( position != wxNOT_FOUND ) {
		int lineno = editor->LineFromPosition(position);
		editor->GotoLine( lineno );
		editor->SetCaretAt( position );
	} else {
		if (!(fileWasOpenedInNB && lineno == wxNOT_FOUND)) {
			editor->GotoLine( lineno );
		}
	}

	//update the symbol tree
	if (updTree && !editor->GetProject().IsEmpty())
		Frame::Get()->GetWorkspacePane()->BuildSymbolTree(fileName);

	//update the 'Recent file' history
	AddToRecentlyOpenedFiles(fileName.GetFullPath());

	if (selection != wxNOT_FOUND) {
		notebook->SetSelection(selection);
	}

	editor->SetProject( projName );

	//Synchronize the file view tree
	if (IsWorkspaceOpen()) {
		Frame::Get()->GetWorkspacePane()->GetFileViewTree()->ExpandToPath(editor->GetProject(), editor->GetFileName());
	}
	
	//update the open files list
	Frame::Get()->GetOpenWindowsPane()->UpdateList();
	editor->SetActive();

	if (returnFocusWin) {
		returnFocusWin->SetFocus();
	}
	return true;
}

void Manager::SetPageTitle(wxWindow *page, const wxString &name)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if ( selection != -1 ) {
		Frame::Get()->GetNotebook()->SetPageText(selection, name);
	}
}

const wxString Manager::GetPageTitle(wxWindow *page)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if ( selection != -1 ) {
		return Frame::Get()->GetNotebook()->GetPageText(selection);
	} else {
		return wxEmptyString;
	}
}

void Manager::SaveAll(bool includeUntitled)
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = book->GetPageCount();

	for (size_t i=0; i<count; i++) {
		LEditor* editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if ( !editor )
			continue;

		//if 'includeUntitled' is not true, dont save new documents that have
		//not been saved to disk yet
		if (!includeUntitled && editor->GetFileName().GetFullPath().StartsWith(wxT("Untitled"))) {
			continue;
		}

		if (editor->GetModify()) {
			editor->SaveFile();
		}
	}
}

void Manager::UnInitialize()
{
	//stop the debugger
	DbgStop();
	DebuggerMgr::Free();

	// Release singleton objects
	
	//since the parser is making use of the TagsManager, 
	//it is important to release it *before* the TagsManager
	ParseThreadST::Get()->Stop();
	ParseThreadST::Free();
	
	TagsManagerST::Free();
	LanguageST::Free();
	WorkspaceST::Free();
	ContextManager::Free();
	BuildManagerST::Free();
	BuildSettingsConfigST::Free();
	
	// Stop the search thread and free its resources
	SearchThreadST::Get()->Stop();
	SearchThreadST::Free();

	//stop the creator thread
	EditorCreatorST::Free();

	wxFlatNotebook::CleanUp();
	MenuManager::Free();
	EditorConfigST::Free();
	EnvironmentConfig::Release();
	
	//free all plugins
	PluginManager::Get()->UnLoad();
}

void Manager::CreateWorkspace(const wxString &name, const wxString &path)
{
	// make sure that the workspace pane is visible
	ShowWorkspacePane(WorkspacePane::FILE_VIEW);

	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateWorkspace(name, path, errMsg);
	CHECK_MSGBOX(res);
	OpenWorkspace(path + PATH_SEP + name + wxT(".workspace"));
}

void Manager::CreateProject(ProjectData &data)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateProject(data.m_name,
	           data.m_path,
	           data.m_srcProject->GetSettings()->GetProjectType(),
	           errMsg);
	CHECK_MSGBOX(res);
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(data.m_name, errMsg);
	//copy the project settings to the new one
	proj->SetSettings(data.m_srcProject->GetSettings());
	ProjectSettingsPtr settings = proj->GetSettings();

	//set the compiler type
	ProjectSettingsCookie cookie;
	BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
	while (bldConf) {
		bldConf->SetCompilerType(data.m_cmpType);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
	proj->SetSettings(settings);
	RebuildFileView();
}

void Manager::CloseWorkspace()
{
	//close any debugging session that is currently opened
	DbgStop();

	//clear any pending & actual breakpoints from the debugger manager
	//DebuggerMgr::Get().DelAllBreakpoints();

	WorkspaceST::Get()->CloseWorkspace();
	//clear the 'buid' tab
	Frame::Get()->GetOutputPane()->GetBuildTab()->Clear();
	
	//clear the navigation bar
	Frame::Get()->GetMainBook()->Clear();
	
	//close all open files, and clear trees
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	int count = book->GetPageCount();
	for (int i=count-1; i>=0; i--) {
		LEditor *page = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (page) {
			book->DeletePage((size_t)i);
		}
	}

	Frame::Get()->GetNotebook()->Refresh();
	Frame::Get()->GetWorkspacePane()->BuildFileTree();
	SetStatusMessage(wxEmptyString, 1);
}

void Manager::OpenWorkspace(const wxString &path)
{
	CloseWorkspace();

	// make sure that the workspace pane is visible
	ShowWorkspacePane(WorkspacePane::FILE_VIEW);

	wxString errMsg;
	bool res = WorkspaceST::Get()->OpenWorkspace(path, errMsg);
	CHECK_MSGBOX(res);

	// update status bar
	wxString dbfile = WorkspaceST::Get()->GetStringProperty(wxT("Database"), errMsg);

	wxFileName fn(dbfile);
	SetStatusMessage(wxString::Format(wxT("Workspace DB: '%s'"), fn.GetFullName().GetData()), 1);

	// load ctags options
	wxBusyCursor cursor;

	//initialize some environment variable to be available for this workspace
	Frame::Get()->GetWorkspacePane()->BuildFileTree();

	//Update the configuration choice on the toolbar
	DoUpdateConfigChoiceControl();

	//update the 'Recent Workspace' history
	AddToRecentlyOpenedWorkspaces(path);

	FileExplorer *fe = Frame::Get()->GetFileExplorer();
	wxFileName filename(path);
	fe->GetFileTree()->ExpandToPath(filename.GetPath() + wxT("/"));

	SendCmdEvent(wxEVT_WORKSPACE_LOADED);
}

void Manager::DoUpdateConfigChoiceControl()
{
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxComboBox *choice = Frame::Get()->GetConfigChoice();

	choice->Enable(true);
	choice->Freeze();

	std::list<WorkspaceConfigurationPtr> confs = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin();
	choice->Clear();
	for (; iter != confs.end(); iter++) {
		choice->Append((*iter)->GetName());
	}

	if (choice->GetCount() > 0) {
		int where = choice->FindString(matrix->GetSelectedConfigurationName());
		if (where != wxNOT_FOUND) {
			choice->SetSelection(where);
		} else {
			choice->SetSelection(0);
		}
	}

	choice->Thaw();
}

ProjectTreePtr Manager::GetProjectFileViewTree(const wxString &projectName)
{
	if ( !IsWorkspaceOpen() ) {
		return NULL;
	}

	wxString err_msg;
	ProjectPtr prj = WorkspaceST::Get()->FindProjectByName(projectName, err_msg);
	if ( !prj ) {
		return NULL;
	}
	return prj->AsTree();
}

void Manager::GetProjectList(wxArrayString &list)
{
	WorkspaceST::Get()->GetProjectList(list);
}

void Manager::AddProject(const wxString & path)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddProject(path, errMsg);
	CHECK_MSGBOX(res);

	//retag the newly added project
	wxFileName fn(path);
	RetagProject(fn.GetName());
	RebuildFileView();
}

void Manager::RebuildFileView()
{
	// update symbol tree
	WorkspacePane *wp = Frame::Get()->GetWorkspacePane();
	wp->BuildFileTree();
}

bool Manager::RemoveProject(const wxString &name)
{
	if ( name.IsEmpty() ) {
		return false;
	}

	ProjectPtr proj = GetProject(name);

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveProject(name, errMsg);
	CHECK_MSGBOX_BOOL(res);

	if (proj) {
		//remove symbols from the database
		std::vector<wxFileName> projectFiles;
		proj->GetFiles(projectFiles, true);
		TagsManagerST::Get()->DeleteFilesTags(projectFiles);
		RemoveProjectNameFromOpenFiles(projectFiles);
	} // if(proj)

	Frame::Get()->GetWorkspacePane()->GetFileViewTree()->BuildTree();
	return true;
}

void Manager::RemoveProjectNameFromOpenFiles(const std::vector<wxFileName> &project_files)
{
	//go over all open tabs, if a file belongs to the closed
	//project, remove the project name from the container editor,
	//this will pervent the parser thread to parse this file after
	//its parent project was removed
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if (editor) {
			wxString openFileFP = editor->GetFileName().GetFullPath();
			for (size_t j=0; j<project_files.size(); j++) {
				if (openFileFP == project_files.at(j).GetFullPath()) {
					editor->SetProject(wxEmptyString);
					break;
				}
			}
		}
	}
}

wxString Manager::GetActiveProjectName()
{
	return WorkspaceST::Get()->GetActiveProjectName();
}

void Manager::SetActiveProject(const wxString &name)
{
	WorkspaceST::Get()->SetActiveProject(WorkspaceST::Get()->GetActiveProjectName(), false);
	WorkspaceST::Get()->SetActiveProject(name, true);
}


void Manager::AddVirtualDirectory(const wxString &virtualDirFullPath)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateVirtualDirectory(virtualDirFullPath, errMsg);
	CHECK_MSGBOX(res);
}

void Manager::RemoveVirtualDirectory(const wxString &virtualDirFullPath)
{
	wxString errMsg;
	wxString project = virtualDirFullPath.BeforeFirst(wxT(':'));
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if ( !p ) {
		return;
	}

	// Update symbol tree and database
	wxString vdPath = virtualDirFullPath.AfterFirst(wxT(':'));
	wxArrayString files;
	p->GetFilesByVirtualDir(vdPath, files);
	for (size_t i=0; i<files.Count(); i++) {
		RemoveFileFromSymbolTree(files.Item(i), p->GetName());
	}

	//and finally, remove the virtual dir from the workspace
	bool res = WorkspaceST::Get()->RemoveVirtualDirectory(virtualDirFullPath, errMsg);
	CHECK_MSGBOX(res);
}

void Manager::SaveWorkspace()
{
	WorkspaceST::Get()->Save();
}

bool Manager::AddNewFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxFile file;
	if (!file.Create(fileName.GetData(), true))
		return false;

	if (file.IsOpened()) {
		file.Close();
	}

	return AddFileToProject(fileName, vdFullPath, openIt);
}

bool Manager::AddFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxString project;
	project = vdFullPath.BeforeFirst(wxT(':'));

	// Add the file to the project
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddNewFile(vdFullPath, fileName, errMsg);
	if (!res) {
		//file or virtual dir does not exist
		return false;
	}

	if ( openIt ) {
		OpenFile(fileName, project);
	}

	TagTreePtr ttp;
	if ( project.IsEmpty() == false ) {
		std::vector<DbRecordPtr> comments;
		if (TagsManagerST::Get()->GetParseComments()) {
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName, &comments);
			TagsManagerST::Get()->StoreComments(comments);
		} else {
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName);
		}
		TagsManagerST::Get()->Store(ttp);
	}
	
	//send notification command event that files was added to 
	//project
	wxArrayString files;
	files.Add(fileName);
	SendCmdEvent(wxEVT_PROJ_FILE_ADDED, (void*)&files);
	return true;
}

void Manager::AddFilesToProject(const wxArrayString &files, const wxString &vdFullPath, wxArrayString &actualAdded)
{
	wxString project;
	project = vdFullPath.BeforeFirst(wxT(':'));

	// Add the file to the project
	wxString errMsg;
	//bool res = true;
	size_t i=0;

	//try to find this file in the workspace
	for (i=0; i<files.GetCount(); i++) {
		wxString projName = this->GetProjectNameByFile(files.Item(i));
		//allow adding the file, only if it does not already exist under the current project 
		//(it can be already exist under the different project)
		if (projName.IsEmpty() || projName != project) {
			actualAdded.Add(files.Item(i));
		}
	}

	for (i=0; i<actualAdded.GetCount(); i++) {
		Workspace *wsp = WorkspaceST::Get();
		wsp->AddNewFile(vdFullPath, actualAdded.Item(i), errMsg);
	}

	//convert wxArrayString to vector for the ctags api
	std::vector<wxFileName> vFiles;
	for (size_t i=0; i<actualAdded.GetCount(); i++) {
		vFiles.push_back(actualAdded.Item(i));
	}

	//retag the added files
	if (vFiles.empty() == false) {
		TagsManagerST::Get()->RetagFiles(vFiles);
	}
	
	SendCmdEvent(wxEVT_PROJ_FILE_ADDED, (void*)&actualAdded);
}

bool Manager::RemoveFile(const wxString &fileName, const wxString &vdFullPath)
{
	// First, close any open tab with this file
	wxFlatNotebook* nb = dynamic_cast<wxFlatNotebook*>(Frame::Get()->GetNotebook());
	if ( !nb ) {
		return false;
	}

	wxString project = vdFullPath.BeforeFirst(wxT(':'));
	wxFileName absPath(fileName);
	absPath.MakeAbsolute(GetProjectCwd(project));

	int count = nb->GetPageCount();
	for (int i=0; i<count; i++) {
		LEditor *editor = dynamic_cast<LEditor*>(nb->GetPage(static_cast<size_t>(i)));
		if ( editor ) {
			wxString fn = editor->GetFileName().GetFullPath();

			if (fn == absPath.GetFullPath() && editor->GetProject() == project) {
				nb->DeletePage(static_cast<size_t>(i));
				break;
			}
		}
	}

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile(vdFullPath, fileName, errMsg);
	CHECK_MSGBOX_BOOL(res);

	RemoveFileFromSymbolTree(absPath, project);
	return true;
}

void Manager::RemoveFileFromSymbolTree(const wxFileName &fileName, const wxString &project)
{
	if ( project.IsEmpty() == false ) {
		// Remove the file from the tags database as well
		TagsManagerST::Get()->Delete(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), fileName.GetFullPath());
	}
}

wxString Manager::GetProjectCwd(const wxString &project) const
{
	wxString errMsg;
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if ( !p ) {
		return wxGetCwd();
	}

	wxFileName projectFileName(p->GetFileName());
	projectFileName.MakeAbsolute();
	return projectFileName.GetPath();
}

void Manager::ShowOutputPane(wxString focusWin, bool commit)
{
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
		if (commit) {
			Frame::Get()->GetDockingManager().Update();
		}
	}

	// set the selection to focus win
	OutputPane *pane = Frame::Get()->GetOutputPane();
	int index = pane->CaptionToIndex(focusWin);
	if ( index != wxNOT_FOUND && index != pane->GetNotebook()->GetSelection()) {
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}


void Manager::ShowDebuggerPane(bool commit)
{
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
		if (commit) {
			Frame::Get()->GetDockingManager().Update();
		}
	}
}

void Manager::HidePane(const wxString &paneName, bool commit)
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(paneName);
	if ( info.IsOk() && info.IsShown()) {
		info.Hide();
		if (commit) {
			Frame::Get()->GetDockingManager().Update();
		}
	}
}

void Manager::ShowWorkspacePane(wxString focusWin, bool commit)
{
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
	if ( info.IsOk() && !info.IsShown() ) {
		info.Show();
		if (commit) {
			Frame::Get()->GetDockingManager().Update();
		}
	}

	// set the selection to focus win
	WorkspacePane *pane = Frame::Get()->GetWorkspacePane();
	int index = pane->CaptionToIndex(focusWin);
	if ( index != wxNOT_FOUND && index != pane->GetNotebook()->GetSelection()) {
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}

ContextBasePtr Manager::NewContextByFileName(const wxFileName &fileName, LEditor *parent) const
{
	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	for (; iter != EditorConfigST::Get()->LexerEnd(); iter++) {
		LexerConfPtr lexer = iter->second;
		wxString lexExt = lexer->GetFileSpec();
		wxStringTokenizer tkz(lexExt, wxT(";"));
		while (tkz.HasMoreTokens()) {
			if (wxMatchWild(tkz.NextToken(), fileName.GetFullName())) {
				return ContextManager::Get()->NewContext(parent, lexer->GetName());
			}
		}
	}

	// return the default context
	return ContextManager::Get()->NewContext(parent, wxT("Text"));
}

void Manager::ApplySettingsChanges()
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = (size_t)book->GetPageCount();
	for (size_t i=0; i<count; i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if (editor) {
			// the open page is of type LEditor
			editor->SetSyntaxHighlight(editor->GetContext()->GetName());
		}
	}
}

ProjectSettingsPtr Manager::GetProjectSettings(const wxString &projectName) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if ( !proj ) {
		wxMessageBox(errMsg, wxT("CodeLite"), wxOK | wxICON_HAND);
		return NULL;
	}

	return proj->GetSettings();
}

void Manager::SetProjectSettings(const wxString &projectName, ProjectSettingsPtr settings)
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if ( !proj ) {
		wxMessageBox(errMsg, wxT("CodeLite"), wxOK | wxICON_HAND);
		return;
	}

	proj->SetSettings(settings);
}

BuildMatrixPtr Manager::GetWorkspaceBuildMatrix() const
{
	return WorkspaceST::Get()->GetBuildMatrix();
}

void Manager::SetWorkspaceBuildMatrix(BuildMatrixPtr matrix)
{
	WorkspaceST::Get()->SetBuildMatrix(matrix);
}

void Manager::TogglePanes()
{
	static bool workspaceShown = false;
	static bool outputShown = false;
	static bool debuggerShown = false;
	static bool fileExplorerShown = false;
	static bool toggled = false;

	Frame::Get()->Freeze();
	if (!toggled) {
		wxAuiPaneInfo info;
		info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
		if ( info.IsOk() ) {
			outputShown = info.IsShown();
		}
		info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
		if ( info.IsOk() ) {
			workspaceShown = info.IsShown();
		}

		info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
		if ( info.IsOk() ) {
			debuggerShown = info.IsShown();
		}

		info = Frame::Get()->GetDockingManager().GetPane(wxT("File Explorer"));
		if ( info.IsOk() ) {
			fileExplorerShown = info.IsShown();
		}

		HidePane(wxT("Output"), false);
		HidePane(wxT("Workspace"), false);
		HidePane(wxT("Debugger"), false);
		HidePane(wxT("File Explorer"), false);
		
		//update changes
		Frame::Get()->GetDockingManager().Update();
		toggled = true;

	} else {

		if (outputShown) {
			ShowOutputPane(wxEmptyString, false);
		}

		if (workspaceShown) {
			ShowWorkspacePane(wxEmptyString, false);
		}

		if (debuggerShown) {
			ShowDebuggerPane(false);
		}

		if (fileExplorerShown) {
			wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("File Explorer"));
			if ( info.IsOk() && !info.IsShown() ) {
				info.Show();
			}
		}
		
		Frame::Get()->GetDockingManager().Update();
		toggled = false;
	}
	Frame::Get()->Thaw();
}

bool Manager::CreateDefaultNewCompiler(const wxString &name)
{
	if (BuildSettingsConfigST::Get()->IsCompilerExist(name)) {
		wxMessageBox(wxT("A compiler with this name already exist"), wxT("Error"), wxOK | wxICON_HAND);
		return false;
	}

	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(name);
	cmp->SetName(name);
	BuildSettingsConfigST::Get()->SetCompiler(cmp);
	return true;
}

bool Manager::DeleteCompiler(const wxString &name)
{
	if (wxMessageBox(wxT("Remove Compiler?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION) == wxYES) {
		BuildSettingsConfigST::Get()->DeleteCompiler(name);
		return true;
	}
	return false;
}

void Manager::GetProjectTemplateList(std::list<ProjectPtr> &list)
{
	wxString tmplateDir = m_startupDir + PATH_SEP + wxT("templates");

	//read all files under this directory
	DirTraverser dt(wxT("*.project"));

	wxDir dir(tmplateDir);
	dir.Traverse(dt);

	wxArrayString &files = dt.GetFiles();

	if (files.GetCount() > 0) {
		for (size_t i=0; i<files.GetCount(); i++) {
			ProjectPtr proj(new Project());
			proj->Load(files.Item(i));
			list.push_back(proj);
		}
	} else {
		//create 3 default empty projects
		ProjectPtr exeProj(new Project());
		ProjectPtr libProj(new Project());
		ProjectPtr dllProj(new Project());
		libProj->Create(wxT("Static Library"), tmplateDir, Project::STATIC_LIBRARY);
		dllProj->Create(wxT("Dynamic Library"), tmplateDir, Project::DYNAMIC_LIBRARY);
		exeProj->Create(wxT("Executable"), tmplateDir, Project::EXECUTABLE);
		list.push_back(libProj);
		list.push_back(dllProj);
		list.push_back(exeProj);
	}
}

void Manager::SaveProjectTemplate(ProjectPtr proj, const wxString &name)
{
	//create new project
	wxString tmplateDir = m_startupDir + PATH_SEP + wxT("templates");

	ProjectPtr cloned(new Project());
	cloned->Create(name, tmplateDir);

	//copy project settings
	cloned->SetSettings(proj->GetSettings());
}

ProjectPtr Manager::GetProject(const wxString &name) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(name, errMsg);
	if ( !proj ) {
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);
		return NULL;
	}
	return proj;
}


void Manager::PopupProjectDependsDlg(const wxString &projectName)
{
	DependenciesDlg *dlg = new DependenciesDlg(GetMainFrame(), projectName);
	dlg->ShowModal();
	dlg->Destroy();
}

void Manager::CleanProject(const wxString &projectName, bool projectOnly)
{
	if ( m_cleanRequest && m_cleanRequest->IsBusy() ) {
		return;
	}

	if ( m_cleanRequest ) {
		delete m_cleanRequest;
	}

	m_cleanRequest = new CleanRequest(GetMainFrame(), projectName, projectOnly);
	m_cleanRequest->Process();
}

bool Manager::IsBuildEndedSuccessfully() const
{
	//build is still running?
	if ( m_compileRequest && m_compileRequest->IsBusy() ) {
		return false;
	}

	wxArrayString lines;
	if (m_compileRequest) {
		if (!m_compileRequest->GetLines(lines)) {
			return false;
		}

		//check every line to see if we got an error/warning
		wxString project(m_compileRequest->GetProjectName());
		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(project);
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(bldConf->GetCompilerType());
		wxString errPattern = cmp->GetErrPattern();
		wxString warnPattern = cmp->GetWarnPattern();

		wxRegEx reErr(errPattern);
		wxRegEx reWarn(warnPattern);
		for (size_t i=0; i<lines.GetCount(); i++) {
			if (reWarn.IsValid() && reWarn.Matches(lines.Item(i))) {
				//skip warnings
				continue;
			}

			if (reErr.IsValid() && reErr.Matches(lines.Item(i))) {
				return false;
			}
		}
	}
	return true;
}

void Manager::BuildProject(const wxString &projectName, bool projectOnly)
{
	if ( m_compileRequest && m_compileRequest->IsBusy() ) {
		return;
	}

	if ( m_compileRequest ) {
		delete m_compileRequest;
		m_compileRequest = NULL;
	}

	//save all files before compiling, but dont saved new documents
	SaveAll(false);

	//If a debug session is running, stop it.
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		if (wxMessageBox(wxT("This would terminate the current debug session, continue?"),  wxT("Confirm"), wxICON_QUESTION|wxYES_NO|wxCANCEL) != wxYES) {
			return;
		} else {
			DbgStop();
		}
	}
	m_compileRequest = new CompileRequest(GetMainFrame(), projectName, projectOnly);
	m_compileRequest->Process();
}

void Manager::CompileFile(const wxString &projectName, const wxString &fileName)
{
	if ( m_compileRequest && m_compileRequest->IsBusy() ) {
		return;
	}

	if ( m_compileRequest ) {
		delete m_compileRequest;
		m_compileRequest = NULL;
	}

	//save all files before compiling, but dont saved new documents
	SaveAll(false);

	//If a debug session is running, stop it.
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		if (wxMessageBox(wxT("This would terminate the current debug session, continue?"),  wxT("Confirm"), wxICON_QUESTION|wxYES_NO|wxCANCEL) != wxYES) {
			return;
		} else {
			DbgStop();
		}
	}

	m_compileRequest = new CompileRequest(GetMainFrame(), projectName, false, fileName);
	m_compileRequest->Process();
}

void Manager::StopBuild()
{
	if ( m_compileRequest && !m_compileRequest->IsBusy() ) {
		return;
	}

	if ( m_compileRequest ) {
		m_compileRequest->Stop();
	}
}

bool Manager::IsBuildInProgress() const
{
	return (m_cleanRequest && m_cleanRequest->IsBusy()) || (m_compileRequest && m_compileRequest->IsBusy());
}

bool Manager::IsProgramRunning() const
{
	if (m_asyncExeCmd == NULL)
		return false;

	return (m_asyncExeCmd && m_asyncExeCmd->IsBusy());
}

void Manager::DebugMessage(wxString msg)
{
	static wxString lastMessage(wxEmptyString);
	msg = msg.Trim(false);
	if(lastMessage != msg){
		Frame::Get()->GetOutputPane()->GetDebugWindow()->AppendLine(msg);
		lastMessage = msg;
	}
}

void Manager::ExecuteNoDebug(const wxString &projectName)
{
	//an instance is already running
	if (m_asyncExeCmd && m_asyncExeCmd->IsBusy()) {
		return;
	}

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(projectName);
	if(!bldConf) {
		wxLogMessage(wxT("failed to find project configuration for project '") + projectName);
		return;
	}
	
	//expand variables
	wxString cmd = bldConf->GetCommand();
	cmd = ExpandVariables(cmd, GetProject(projectName));

	wxString cmdArgs = bldConf->GetCommandArguments();
	cmdArgs = ExpandVariables(cmdArgs, GetProject(projectName));

	//execute command & cmdArgs
	wxString execLine(cmd + wxT(" ") + cmdArgs);
	wxString wd = bldConf->GetWorkingDirectory();
	wd = ExpandVariables(wd, GetProject(projectName));

	//change directory to the working directory
	DirSaver ds;

	ProjectPtr proj = GetProject(projectName);
	//print the current directory
	::wxSetWorkingDirectory(proj->GetFileName().GetPath());
	
	//now set the working directory according to working directory field from the
	//project settings
	::wxSetWorkingDirectory(wd);
	
	//execute the command line
	//the async command is a one time executable object,
	m_asyncExeCmd = new AsyncExeCmd(GetMainFrame());

#if defined(__WXMAC__)
    execLine = wxString( wxT("osascript -e 'tell application \"Terminal\"'")) +
        wxT(" -e   'activate'") +
        wxT(" -e   'do script with command \"cd ") + proj->GetFileName().GetPath() + wxT(" && cd ") + wd + wxT(" && ") + execLine + wxT("\"'") +
        wxT(" -e  'end tell'");
#elif defined(__WXGTK__)
	//set a console to the execute target
	wxString term;
	term << wxT("xterm -title ");
	term << wxT("'") << execLine << wxT("'");
	term << wxT(" -e ");
	
	if ( bldConf->GetPauseWhenExecEnds() ) {
		term << CL_EXEC_WRAPPER;
	}
	
	term << execLine;
	execLine = term;
#elif defined (__WXMSW__)
	
	if (bldConf->GetPauseWhenExecEnds() ) {
		execLine.Prepend(CL_EXEC_WRAPPER);
	}
	
#endif

	//execute the program:
	//- no hiding the console
	//- no redirection of the stdin/out
	EnvironmentConfig::Instance()->ApplyEnv();
	m_asyncExeCmd->Execute(execLine, false, false);
	if (m_asyncExeCmd->GetProcess()) {
		
		m_asyncExeCmd->GetProcess()->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(Manager::OnProcessEnd), NULL, this);
	} else {
		EnvironmentConfig::Instance()->UnApplyEnv();
	}
}

void Manager::OnProcessEnd(wxProcessEvent &event)
{
	m_asyncExeCmd->ProcessEnd(event);
	m_asyncExeCmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(Manager::OnProcessEnd), NULL, this);
	delete m_asyncExeCmd;
	m_asyncExeCmd = NULL;
	
	//unset the environment variables
	EnvironmentConfig::Instance()->UnApplyEnv();
	
	//return the focus back to the editor
	if(GetActiveEditor()) {
		GetActiveEditor()->SetActive();
	}
}

void Manager::SetWorkspaceConfigurationName(const wxString &name)
{
	BuildMatrixPtr matrix = GetWorkspaceBuildMatrix();
	matrix->SetSelectedConfigurationName(name);
	SetWorkspaceBuildMatrix(matrix);
}

void Manager::ShowMainToolbar(bool show)
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Standard Toolbar"));
	if (info.IsOk() && show && !info.IsShown()) {
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	} else if (info.IsOk() && !show && info.IsShown()) {
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}
}

BuildConfigPtr Manager::GetActiveProjectBuildConf()
{
	BuildMatrixPtr matrix = GetWorkspaceBuildMatrix();
	wxString projConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), GetActiveProjectName());
	ProjectSettingsPtr settings = GetProjectSettings(GetActiveProjectName());
	return settings->GetBuildConfiguration(projConf);
}

void Manager::SetExternalDatabase(const wxFileName &dbname)
{
	//close any opened external database
	CloseExternalDatabase();

	// build the external database
	TagsManagerST::Get()->OpenExternalDatabase(dbname);

	if (TagsManagerST::Get()->GetExtDatabase()->IsOpen()) {
		SetStatusMessage(wxString::Format(wxT("External DB: '%s'"), dbname.GetFullName().GetData()), 2);
		EditorConfigST::Get()->SetTagsDatabase(dbname.GetFullPath());
	}
}

void Manager::GetWorkspaceFiles(std::vector<wxFileName> &files, bool absPath)
{
	wxArrayString projects;
	GetProjectList(projects);
	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = GetProject(projects.Item(i));
		p->GetFiles(files, absPath);
	}
}

bool Manager::IsFileInWorkspace(const wxString &fileName)
{
	wxFileName findme(fileName);
	std::vector<wxFileName> files;

	GetWorkspaceFiles(files);
	std::vector<wxFileName>::const_iterator iter = std::find(files.begin(), files.end(), findme);
	return iter != files.end();
}

void Manager::RetagProject(const wxString &projectName)
{
	//get the project file list
	ProjectPtr proj = GetProject(projectName);
	if ( proj ) {
		//change the directory to the project dir
		std::vector<wxFileName> projectFiles;
		proj->GetFiles(projectFiles, true);

		//call tags manager for retagging
		TagsManagerST::Get()->RetagFiles(projectFiles);
	}
}

void Manager::RetagWorkspace()
{
	wxArrayString projects;
	GetProjectList(projects);
	std::vector<wxFileName> projectFiles;

	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr proj = GetProject(projects.Item(i));
		if ( proj ) {
			//change the directory to the project dir
			proj->GetFiles(projectFiles, true);
		}
	}
	//call tags manager for retagging
	TagsManagerST::Get()->RetagFiles(projectFiles);
}

void Manager::WriteProgram(const wxString &line)
{
	if (!IsProgramRunning())
		return;

	if(m_asyncExeCmd->GetProcess()->GetRedirect() == false){
		return;
	}
	
	wxOutputStream *out = m_asyncExeCmd->GetOutputStream();
	if ( out ) {
		wxString cmd(line);
		cmd += wxT("\n");

		const wxCharBuffer pWriteData = _C(cmd);
		out->Write(pWriteData.data(), cmd.Length());
	}
}

void Manager::KillProgram()
{
	if (!IsProgramRunning())
		return;

	m_asyncExeCmd->Terminate();
}

wxString Manager::GetProjectNameByFile(const wxString &fullPathFileName)
{
	wxArrayString projects;
	GetProjectList(projects);

	std::vector<wxFileName> files;
	for (size_t i=0; i<projects.GetCount(); i++) {
		files.clear();
		ProjectPtr proj = GetProject(projects.Item(i));
		proj->GetFiles(files, true);

		for (size_t xx=0; xx<files.size(); xx++) {
			if (files.at(xx).GetFullPath() == fullPathFileName) {
				return proj->GetName();
			}
		}
	}

	return wxEmptyString;
}

/*
void Manager::ImportFromMakefile(const wxString &path)
{
	OutputMessage(path + wxT("\n"));

	wxFileName fileName = path;
	OutputMessage(fileName.GetPath() + wxT("\n"));

	VariableLexer expander(path);
	wxArrayString expanded = expander.getResult();

	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();

	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	CreateWorkspace(wxT("import_from_") + fileName.GetName(), fileName.GetPath());

	wxArrayString extentions;
	extentions.Add(wxT(".h"));
	extentions.Add(wxT(".hpp"));
	extentions.Add(wxT(".c"));
	extentions.Add(wxT(".cc"));
	extentions.Add(wxT(".cpp"));
	extentions.Add(wxT(".cxx"));

	for (size_t i = 0; i < lexed.size(); i++) {
		Target targ = lexed[i];
		wxArrayString deps = targ.getDeps();

		ProjectPtr proj(new Project());
		proj->Create(targ.getName(), fileName.GetPath(), wxT("importedProject"));
		proj->SetSettings(new ProjectSettings(NULL));

		for (size_t j = 0; j < deps.size(); j++) {
			wxString dep = deps[j];
			if (dep.Right(2) == wxT(".o")) { // string ends with .o!
				wxString file = dep.Left(dep.size()-2);

				for (size_t k = 0; k < extentions.size(); k++) {
					wxString ext = extentions[k];
					wxFileName fileName = proj->GetFileName().GetPathWithSep() + file + ext;
					if (fileName.FileExists()) {
						bool added = proj->AddFile(fileName.GetFullPath(), wxT("Source Files"));
						if (!added) {
							DebugMessage(wxT("WHOOPS WRONG BAD NOT GOOD!\n"));
						}
					}
				}
			} else if (deps.Index(dep, false) != wxNOT_FOUND) {
				wxArrayString dependencies= proj->GetDependencies();
				dependencies.Add(dep);
				proj->SetDependencies(dependencies);
			}
		}
		proj->Save();
		//AddProject(proj->GetFileName().GetFullPath());
	}
	return;
}
*/

LEditor *Manager::FindEditorByFileName(const wxString &fileName) const
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	if (!book) {
		return NULL;
	}
	book->GetPageCount();
	for (int i=0; i<book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (editor && editor->GetFileName().GetFullPath() == fileName) {
			return editor;
		}
	}
	return NULL;
}

LEditor *Manager::GetActiveEditor() const
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	if (!book) {
		return NULL;
	}

	int selected = book->GetSelection();
	if (selected == wxNOT_FOUND)
		return NULL;

	LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(selected));
	return editor;
}

void Manager::AddToRecentlyOpenedFiles(const wxString &fileName)
{
	//get list recently opened files
	wxArrayString files;
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedFies(files);

	if (files.Index(fileName) == wxNOT_FOUND) {
		//the file does not exist, add it and save the list
		files.Add(fileName);
		m_recentFiles.AddFileToHistory(fileName);
	}

	files.Empty();
	//sync between the history object and the configuration file
	m_recentFiles.GetFiles(files);
	cfg->SetRecentlyOpenedFies(files);
}

void Manager::AddToRecentlyOpenedWorkspaces(const wxString &fileName)
{
	//get list recently opened files
	wxArrayString files;
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedWorkspaces(files);

	if (files.Index(fileName) == wxNOT_FOUND) {
		//the file does not exist, add it and save the list
		files.Add(fileName);
		m_recentWorkspaces.AddFileToHistory(fileName);
	}
	files.Empty();
	//sync between the history object and the configuration file
	m_recentWorkspaces.GetFiles(files);
	cfg->SetRecentlyOpenedWorkspaces(files);
}

void Manager::GetRecentlyOpenedFiles(wxArrayString &files)
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedFies(files);
}

void Manager::GetRecentlyOpenedWorkspaces(wxArrayString &files)
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedWorkspaces(files);
}

void Manager::CloseExternalDatabase()
{
	TagsManager *mgr = TagsManagerST::Get();
	mgr->CloseExternalDatabase();
	//remove the entry from the status bar
	SetStatusMessage(wxEmptyString, 2);
}

void Manager::SetStatusMessage(const wxString &msg, int col)
{
	Frame::Get()->GetStatusBar()->SetStatusText(msg, col);
}

void Manager::CloseAllButThis(wxWindow *curreditor)
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	bool idxToDelete(0);//can be 1 or 0
	while(book->GetPageCount() > 1){
		if(book->GetPage(0) == curreditor){
			idxToDelete = 1;
		}
		book->DeletePage(idxToDelete);
	}
}

void Manager::CloseAll()
{
	bool modifyDetected = false;
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	Frame::Get()->GetOpenWindowsPane()->Clear();

	//check if any of the files is modified
	for (int i=0; i<book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (editor) {
			if (editor->GetModify()) {
				modifyDetected = true;
				break;
			}
		}
	}

	int retCode(CLOSEALL_DISCARDALL);
	CloseAllDialog *dlg(NULL);

	if (modifyDetected) {
		dlg = new CloseAllDialog(Frame::Get());
		retCode = dlg->ShowModal();
		dlg->Destroy();
	}

	switch (retCode) {
	case CLOSEALL_SAVEALL: {
		ManagerST::Get()->SaveAll();
		//and now close them all
		book->DeleteAllPages();
		book->Refresh();
	}
	break;
	case CLOSEALL_DISCARDALL: {
		book->DeleteAllPages();
		book->Refresh();
	}
	break;
	case CLOSEALL_ASKFOREACHFILE: {
		int count = book->GetPageCount();
		for (int i=0; i<count; i++) {
			LEditor* editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
			if ( !editor )
				continue;

			bool veto;
			Frame::Get()->ClosePage(editor, false, book->GetSelection(), true, veto);
		}
		//once all files have been prompted if needed, remove them all
		book->DeleteAllPages();
		book->Refresh();
	}
	break;
	default:
		break;
	}

	if (dlg) {
		dlg->Destroy();
	}

	//remove all symbol trees from the outline view
	Frame::Get()->GetWorkspacePane()->DeleteAllSymbolTrees();
}

bool Manager::MoveFileToVD(const wxString &fileName, const wxString &srcVD, const wxString &targetVD)
{
	// to move the file between targets, we need to change the file path, we do this
	// by changing the file to be in absolute path related to the src's project
	// and then making it relative to the target's project
	wxString srcProject, targetProject;
	srcProject = srcVD.BeforeFirst(wxT(':'));
	targetProject = targetVD.BeforeFirst(wxT(':'));
	wxFileName srcProjWd(GetProjectCwd(srcProject), wxEmptyString);

	//set a dir saver point
	wxFileName fn(fileName);

	//remove the file from the source project
	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile(srcVD, fileName, errMsg);
	CHECK_MSGBOX_BOOL(res);

	// Add the file to the project
	res = WorkspaceST::Get()->AddNewFile(targetVD, fn.GetFullPath(), errMsg);
	if (!res) {
		//file or virtual dir does not exist
		return false;
	}
	//if the file is currently open, replace its project name
	//in the editor container
	// First, close any open tab with this file
	wxFlatNotebook* nb = Frame::Get()->GetNotebook();
	int count = nb->GetPageCount();
	for (int i=0; i<count; i++) {
		LEditor *editor = dynamic_cast<LEditor*>(nb->GetPage(static_cast<size_t>(i)));
		if (editor) {
			wxString currFile = editor->GetFileName().GetFullPath();
			if (currFile == fn.GetFullPath()) {
				editor->SetProject(targetProject.Trim());
				break;
			}
		}
	}
	return true;
}

void Manager::GetProjectFiles(const wxString &project, wxArrayString &files)
{
	std::vector<wxFileName> fileNames;
	ProjectPtr p = GetProject(project);
	p->GetFiles(fileNames, true);

	//convert std::vector to wxArrayString
	for (std::vector<wxFileName>::iterator it = fileNames.begin(); it != fileNames.end(); it ++) {
		files.Add((*it).GetFullPath());
	}
}

void Manager::GetWorkspaceFiles(wxArrayString &files)
{
	wxArrayString projects;
	GetProjectList(projects);

	for (size_t i=0; i<projects.GetCount(); i++) {
		GetProjectFiles(projects.Item(i), files);
	}
}

void Manager::ImportMSVSSolution(const wxString &path)
{
	wxFileName fn(path);
	if (fn.FileExists() == false) {
		return;
	}
	wxString errMsg;
	VcImporter importer(path);
	if (importer.Import(errMsg)) {
		wxString wspfile;
		wspfile << fn.GetPath() << wxT("/") << fn.GetName() << wxT(".workspace");
		OpenWorkspace(wspfile);
	}
}

wxString Manager::ExpandVariables2(const wxString &expression)
{
	return WorkspaceST::Get()->ExpandVariables(expression);
}

wxString Manager::ExpandVariables(const wxString &expression, ProjectPtr proj)
{
	wxString output(expression);
	wxString project_name(proj->GetName());
	
	//make sure that the project name does not contain any spaces
	project_name.Replace(wxT(" "), wxT("_"));
	
	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(proj->GetName());
	output.Replace(wxT("$(ProjectName)"), project_name);
	output.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
	output.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
	output.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());

	LEditor *editor = GetActiveEditor();
	if (editor) {
		output.Replace(wxT("$(CurrentFileName)"), editor->GetFileName().GetName());
		output.Replace(wxT("$(CurrentFilePath)"), editor->GetFileName().GetPath());
		output.Replace(wxT("$(CurrentFileExt)"), editor->GetFileName().GetExt());
		output.Replace(wxT("$(CurrentFileFullPath)"), editor->GetFileName().GetFullPath());
	}

	//call the environment & workspace variables expand function
	output = ExpandVariables2(output);
	return output;
}

//--------------------------- Debugger API -----------------------------

void Manager::DbgStart(long pid)
{
	//set the working directory to the project directory
	DirSaver ds;
	wxString errMsg;
	wxString output;
	wxString debuggerName;
	wxString exepath;
	wxString wd;
	wxString args;
	BuildConfigPtr bldConf;
	ProjectPtr proj;
	long PID(-1);
	
	if(pid == 1){ //attach to process
		AttachDbgProcDlg *dlg = new AttachDbgProcDlg(NULL);
		if(dlg->ShowModal() == wxID_OK){
			wxString processId = dlg->GetProcessId();
			exepath   = dlg->GetExeName();
			debuggerName = dlg->GetDebugger();
			DebuggerMgr::Get().SetActiveDebugger(debuggerName);
			
			processId.ToLong(&PID);
			if(exepath.IsEmpty() == false){
				wxFileName fn(exepath);
				wxSetWorkingDirectory(fn.GetPath());
				exepath = fn.GetFullName();
				
			}
			dlg->Destroy();
		}else{
			dlg->Destroy();
			return;
		}
	}

	if(pid == wxNOT_FOUND){
		//need to debug the current project
		proj = WorkspaceST::Get()->FindProjectByName(GetActiveProjectName(), errMsg);
		if (proj) {
			wxSetWorkingDirectory(proj->GetFileName().GetPath());	
			bldConf = WorkspaceST::Get()->GetProjSelBuildConf(proj->GetName());
			if(bldConf){
				debuggerName = bldConf->GetDebuggerType();
				DebuggerMgr::Get().SetActiveDebugger(debuggerName);
			}
		}
	}

	//make sure we have an active debugger
	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	if (!dbgr) {
		//No deubgger available,
		wxString message;
		message << wxT("Failed to launch debugger '") << debuggerName << wxT("': debugger not loaded\n");
		message << wxT("Make sure that you have an open workspace and that the active project is of type 'Executable'");
		wxMessageBox(message, wxT("CodeLite"), wxOK|wxICON_WARNING);
		return;
	}

	if (dbgr->IsRunning()) {
		//debugger is already running, so issue a 'cont' command
		dbgr->Continue();
		dbgr->QueryFileLine();
		return;
	}
	
	//set the debugger information
	DebuggerInformation dinfo;
	DebuggerMgr::Get().GetDebuggerInformation(debuggerName, dinfo);
	dbgr->SetDebuggerInformation(dinfo);

	if(pid == wxNOT_FOUND){
		exepath = bldConf->GetCommand();
		args = bldConf->GetCommandArguments();
		exepath.Prepend(wxT("\""));
		exepath.Append(wxT("\""));
		wd = bldConf->GetWorkingDirectory();

		// Expand variables before passing them to the debugger
		wd = ExpandVariables(wd, proj);
		exepath = ExpandVariables(exepath, proj);
	}
	
	//get the debugger path to execute
	DebuggerInformation dbginfo;
	DebuggerMgr::Get().GetDebuggerInformation(debuggerName, dbginfo);
	wxString dbgname = dbginfo.path;

	dbgname = EnvironmentConfig::Instance()->ExpandVariables(dbgname);

	//set focus to the output debug pane
	ShowOutputPane(OutputPane::OUTPUT_DEBUG);

	//set ourselves as the observer for the debugger class
	dbgr->AddObserver(this);

	//remove any breakpoints from previous runs
	//and collect the breakpoints from the user
	DebuggerMgr::Get().DelAllBreakpoints();

	//Loop through the open editors and let each editor
	//a chance to update the debugger manager whith any line
	//changes (i.e. file was edited and breakpoints were moved)
	//this loop must take place before the debugger startup
	//or else call to UpdateBreakpoint() will yield an attempt to
	//actually add the breakpoint before Run() is called - this can
	//be a problem when adding breakpoint to dll files
	if(wxNOT_FOUND == pid){
		wxFlatNotebook *book = Frame::Get()->GetNotebook();
		for (int i=0; i<book->GetPageCount(); i++) {
			LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
			if (editor) {
				editor->UpdateBreakpoints();
			}
		}
	}

	//We can now get all the gathered breakpoints from the manager
	std::vector<BreakpointInfo> bps;
	DebuggerMgr::Get().GetBreakpoints(bps);

	if(pid == wxNOT_FOUND){
		//it is now OK to start the debugger...
		if (!dbgr->Start(dbgname, exepath, wd, bps)) {
			wxString errMsg;
			errMsg << wxT("Failed to initialize debugger: ") << dbgname << wxT("\n");
			DebugMessage(errMsg);
			return;
		}
		m_dbgWaitingFirstBp = true;
	} else {
		//Attach to process...
		if (!dbgr->Start(dbgname, exepath, PID, bps)) {
			wxString errMsg;
			errMsg << wxT("Failed to initialize debugger: ") << dbgname << wxT("\n");
			DebugMessage(errMsg);
			return;
		}
	}

	//let the active editor get the focus
	LEditor *editor = dynamic_cast<LEditor*>(GetActiveEditor());
	if (editor) {
		editor->SetActive();
	}
	
	//mark that we are waiting for the first GotControl()
	DebugMessage(output);
	DebugMessage(wxT("Debug session started successfully!\n"));
	
	//Incase we are attaching to process
	//do nothing here
	if(pid == wxNOT_FOUND){
		dbgr->Run(args);
	}
	
	//and finally make the debugger pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
	if (info.IsOk() && !info.IsShown()) {
		HideDebuggerPane = true;
		ShowDebuggerPane();
	}
}

void Manager::DbgStop()
{
	if (m_quickWatchDlg) {
		m_quickWatchDlg->Destroy();
		m_quickWatchDlg = NULL;
	}

	//remove all debugger markers
	DbgUnMarkDebuggerLine();
	m_dbgCanInteract = false;
	
	IDebugger *dbgr =  DebuggerMgr::Get().GetActiveDebugger();
	if (!dbgr) {
		return;
	}

	if (!dbgr->IsRunning()) {
		return;
	}

	//clear the debugger pane
	Frame::Get()->GetDebuggerPane()->Clear();
	
	dbgr->Stop();
	DebuggerMgr::Get().SetActiveDebugger(wxEmptyString);

	DebugMessage(wxT("Debug session ended\n"));
	//update toolbar state
	UpdateStopped();
	//and finally, hide the debugger pane (if we caused it to appear)
	if (HideDebuggerPane) {
		HideDebuggerPane = false;
		HidePane(wxT("Debugger"));
	}
	//mark the debugger as non interactive
	m_dbgCanInteract = false;
}

void Manager::DbgMarkDebuggerLine(const wxString &fileName, int lineno)
{
	DbgUnMarkDebuggerLine(); 
	if (lineno < 0) {
		return;
	}

	//try to open the file 
	wxFileName fn(fileName);
	LEditor *editor = GetActiveEditor();
	if (editor && editor->GetFileName().GetFullPath() == fn.GetFullPath()) {
		editor->HighlightLine(lineno);
		editor->GotoLine(lineno-1);
	} else {
		OpenFile(fileName, wxEmptyString, lineno-1, wxNOT_FOUND);
		editor = GetActiveEditor();
		if(editor){
			editor->HighlightLine(lineno);
		}
	}
}

void Manager::DbgUnMarkDebuggerLine()
{
	//remove all debugger markers from all editors
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	for (int i=0; i<book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (editor) {
			editor->UnHighlightAll();
		}
	}
}

void Manager::OnOutputWindow(wxCommandEvent &e)
{
	if (e.GetEventType() == wxEVT_SHELLWIN_LINE_ENTERED) {
		WriteProgram(e.GetString());
	} else if (e.GetEventType() == wxEVT_SHELLWIN_CTRLC) {
		m_asyncExeCmd->Terminate();
	}
}

void Manager::OnDebuggerWindow(wxCommandEvent &e)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		if (e.GetEventType() == wxEVT_SHELLWIN_LINE_ENTERED) {
			wxString cmd(e.GetString());
			dbgr->ExecuteCmd(cmd);
		} else if (e.GetEventType() == wxEVT_SHELLWIN_CTRLC) {
			DbgDoSimpleCommand(DBG_PAUSE);
		}
	}
}

void Manager::DbgDeleteBreakpoint(const BreakpointInfo &bp)
{
	DebuggerMgr::Get().DelBreakpoint(bp);
	//update the editor
	LEditor *editor = FindEditorByFileName(bp.file);
	if (editor) {
		editor->DelBreakpointMarker(bp.lineno-1);
	}
}

void Manager::DbgDeleteAllBreakpoints()
{
	if (!DbgCanInteract()) {
		return;
	}

	DebuggerMgr::Get().DelAllBreakpoints();
	//update the editor
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	if (!book) {
		return;
	}

	for (int i=0; i<book->GetPageCount(); i++) {
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage((size_t)i));
		if (editor) {
			editor->DelAllBreakpointMarkers();
		}
	}
}

void Manager::DbgDoSimpleCommand(int cmd)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		switch (cmd) {
		case DBG_NEXT: {
			if (dbgr->IsRunning()) {
				dbgr->Next();
			}
		}
		break;
		case DBG_STEPIN: {
			if (dbgr->IsRunning()) {
				dbgr->StepIn();
			}
		}
		break;
		case DBG_STEPOUT: {
			if (dbgr->IsRunning()) {
				dbgr->StepOut();
			}
		}
		break;
		case DBG_PAUSE: {
			if (dbgr->IsRunning()) {
				dbgr->Interrupt();
			}
		}
		break;
		default:
			break;
		}
	}
}

void Manager::DbgQuickWatch(const wxString &expression, bool useTipWin, long pos)
{ 
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if (dbgr && dbgr->IsRunning()) {
		m_useTipWin = useTipWin;
		m_tipWinPos = pos;
		dbgr->EvaluateExpressionToTree(expression);
	}
}

void Manager::DbgCancelQuickWatchTip()
{ 
	/*
	if(m_debuggerTipWin){
		m_debuggerTipWin->Dismiss();
	}
	*/
}

//-------------------------------------------------
//Event handlers from the debugger
//-------------------------------------------------

void Manager::UpdateAddLine(const wxString &line)
{
	DebugMessage(line + wxT("\n"));
}

void Manager::UpdateFileLine(const wxString &filename, int lineno)
{
	wxString fileName = filename;
	long lineNumber = lineno;
	if (m_frameLineno != wxNOT_FOUND) {
		lineNumber = m_frameLineno;
		m_frameLineno = wxNOT_FOUND;
	}

	DbgMarkDebuggerLine(fileName, lineNumber);
	UpdateDebuggerPane();
}

void Manager::UpdateDebuggerPane()
{
	//Update the debugger pane
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
	if (info.IsShown()) {
		DebuggerPane *pane = Frame::Get()->GetDebuggerPane();
		if (pane->GetNotebook()->GetCurrentPage() == (wxWindow*)pane->GetBreakpointView()) {
			pane->GetBreakpointView()->Initialize();
		}
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
			//query the watches, to improve performance, we query only
			//the visible panes
			if (pane->GetNotebook()->GetCurrentPage() == pane->GetLocalsTree()) {
				//update the locals tree
				dbgr->QueryLocals();
				
			} else if (pane->GetNotebook()->GetCurrentPage() == pane->GetWatchesTable()) {
				//update the watches table
				wxArrayString expressions = pane->GetWatchesTable()->GetExpressions();
				for (size_t i=0; i<expressions.GetCount(); i++) {
					dbgr->EvaluateExpressionToString(expressions.Item(i));
				}
			} else if (pane->GetNotebook()->GetCurrentPage() == (wxWindow*)pane->GetFrameListView()) {
				//update the stack call
				dbgr->ListFrames();
			} else if (pane->GetNotebook()->GetCurrentPage() == (wxWindow*)pane->GetBreakpointView()) {
				//update the breakpoint view
				pane->GetBreakpointView()->Initialize();
			} else if (pane->GetNotebook()->GetCurrentPage() == (wxWindow*)pane->GetThreadsView()) {
				//update the thread list
				ThreadEntryArray threads;
				dbgr->ListThreads(threads);
				pane->GetThreadsView()->PopulateList(threads);
			}
		}
	}
}

void Manager::UpdateLocals(TreeNode<wxString, NodeData> *tree)
{
	NodeData data = tree->GetData();
	static TreeNode<wxString, NodeData> *thisTree(NULL);
	if(data.name == wxT("*this")){
		//append this tree to the local's tree
		thisTree = tree;//keep the tree and wait for the other call that will come
	}else{
		//if we already have thisTree, append it as child of this tree
		if(thisTree){
			tree->AddChild(thisTree);
			thisTree = NULL;
		}
		Frame::Get()->GetDebuggerPane()->GetLocalsTree()->BuildTree(tree);
	}
}

void Manager::UpdateStopped()
{
	//do something here....
}

void Manager::UpdateGotControl(DebuggerReasons reason)
{
	//put us ontop of the z-order window
	Frame::Get()->Raise();
	m_dbgCanInteract = true;
	
	switch (reason) {
	case DBG_RECV_SIGNAL_EXC_BAD_ACCESS:
	case DBG_RECV_SIGNAL_SIGSEGV: { //program received signal sigsegv
		wxString signame = wxT("SIGSEGV");
		if(reason == DBG_RECV_SIGNAL_EXC_BAD_ACCESS){
			signame = wxT("EXC_BAD_ACCESS");
		}
		DebugMessage(wxT("Program Received signal ") + signame + wxT("\n"));
		int answer = wxMessageBox(wxT("Program Received signal ") + signame + wxT("\n")
		                          wxT("Stack trace can be viewed in the 'Stack' tab\n")
		                          wxT("Would you like to terminate the debugging session?"), wxT("CodeLite"), wxICON_ERROR|wxYES_NO|wxCANCEL);
		if (answer == wxYES) {
			DbgStop();
		} else if (answer == wxNO) {
			//Print the stack trace
			wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
			if (info.IsShown()) {
				Frame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
				UpdateDebuggerPane();
			}
		}
	}
	break;
	case DBG_END_STEPPING://finished one of the following: next/step/nexti/stepi
	case DBG_FUNC_FINISHED:
	case DBG_BP_HIT: { //breakpoint reached
		//query the current line and file
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if (dbgr && dbgr->IsRunning()) {
			dbgr->QueryFileLine();
		}
	}
	break;
	case DBG_EXITED_NORMALLY:
		//debugging finished, stop the debugger process
		DbgStop();
		break;
	default:
		break;
	}
}

void Manager::UpdateLostControl()
{
	//debugger lost control
	//hide the marker
	DbgUnMarkDebuggerLine();
	m_dbgCanInteract = false;
	DebugMessage(wxT("Continuing...\n"));
}

void Manager::UpdateBpAdded()
{
}

void Manager::UpdateExpression(const wxString &expression, const wxString &evaluated)
{
	Frame::Get()->GetDebuggerPane()->GetWatchesTable()->UpdateExpression(expression, evaluated);
}

void Manager::UpdateQuickWatch(const wxString &expression, TreeNode<wxString, NodeData> *tree)
{
	if(m_useTipWin){

#if 0		
		m_useTipWin = false;
		if(m_debuggerTipWin){
			delete m_debuggerTipWin;
		}
		m_debuggerTipWin = new DebuggerTip(GetMainFrame(), expression, tree, m_tipWinPos);
		m_debuggerTipWin->Popup(); 
#endif

	}else{
		//Display a dialog with the expression
		if (!m_quickWatchDlg) {
			//first time?
			m_quickWatchDlg = new QuickWatchDlg(GetMainFrame(), expression, tree);
			m_quickWatchDlg->ShowModal();
		} else {
			//Dialog already created
			if (m_quickWatchDlg->IsShown()) {
				//dialog is visible, just update the tree
				m_quickWatchDlg->Init(expression, tree);
			} else {
				//Update the tree
				m_quickWatchDlg->Init(expression, tree);
				m_quickWatchDlg->ShowModal();
			}
		}
	}
}

void Manager::UpdateStackList(const StackEntryArray &stackArr)
{
	Frame::Get()->GetDebuggerPane()->GetFrameListView()->Update(stackArr);
}

void Manager::DbgSetFrame(int frame, int lineno)
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Debugger"));
	if (info.IsShown()) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
			//set the frame
			dbgr->SetFrame(frame);
			m_frameLineno = lineno;
		}
	}
}

void Manager::DbgSetThread(long threadId)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if ( dbgr && dbgr->IsRunning() && DbgCanInteract() ) {
		//set the frame
		dbgr->SelectThread(threadId);
		dbgr->QueryFileLine();
	}
}

void Manager::UpdateBuildTools()
{
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(wxT( "GNU makefile for g++/gcc" ));
	wxString tool = builder->GetBuildToolName();
	wxString origTool(tool);
	
	//confirm that it exists...
	wxString path;
	bool is_ok(true);
	
	if(tool.Contains(wxT("$"))) {
		//expand 
		tool = EnvironmentConfig::Instance()->ExpandVariables(tool);
	}
	
	if (!ExeLocator::Locate(tool, path)) {
		is_ok = false;
		//failed to locate the specified build tool
		//try some default names which are commonly used on windows
		if (!is_ok && ExeLocator::Locate(wxT("mingw32-make"), path)) {
			tool = path;
			is_ok = true;
		}
		
		if (!is_ok && ExeLocator::Locate(wxT("make"), path)){
			tool = path;
			is_ok = true;
		}
	}else{
		//we are good, nothing to be done
		return;
	}
	
	wxString message;
	if (!is_ok) {
		message << wxT("Failed to locate make util '")
			    << tool << wxT("' specified by 'Build Settings'");
		wxMessageBox(message, wxT("CodeLite"), wxICON_WARNING|wxOK);
		return;
	} else {
		wxLogMessage(wxT("Updating build too to '") + tool + wxT("' from '") + origTool + wxT("'"));
	}
	
	//update the cached builders
	builder->SetBuildTool(tool);
	BuildManagerST::Get()->AddBuilder(builder);

	//update the configuration files
	BuildSystemPtr bsptr = BuildSettingsConfigST::Get()->GetBuildSystem(wxT( "GNU makefile for g++/gcc" ));
	if (!bsptr) {
		bsptr = new BuildSystem(NULL);
		bsptr->SetName(wxT( "GNU makefile for g++/gcc" ));
	}

	bsptr->SetToolPath(tool);
	BuildSettingsConfigST::Get()->SetBuildSystem(bsptr);
}

bool Manager::OpenFileAndAppend(const wxString &fileName, const wxString &text)
{
	bool ret(false);
	if(OpenFile(fileName, wxEmptyString)){
		LEditor* editor = GetActiveEditor();
		if(editor){
			//get the current line number
			int lineNum = editor->GetLineCount();
			editor->GotoLine(lineNum-1);
			editor->AppendText(text);
			ret = true;
		}
	}
	return ret;
}

void Manager::OutputMessage(wxString msg)
{
	msg = msg.Trim(false);
	Frame::Get()->GetOutputPane()->GetOutputWindow()->AppendLine(msg);
}

void Manager::RunCustomPreMakeCommand(const wxString &project)
{
	if ( m_compileRequest && m_compileRequest->IsBusy() ) {
		return;
	}

	if ( m_compileRequest ) {
		delete m_compileRequest;
		m_compileRequest = NULL;
	}

	m_compileRequest = new CompileRequest(	GetMainFrame(), //owner window
											project,	 	//project to build 
											false, 			//not project only
											wxEmptyString, 	//no file name (valid only for build file only)
											true);			//run premake step only
	m_compileRequest->Process();	
}
