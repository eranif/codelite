#include "wx/html/htmlwin.h"
#include "subversion.h"
#include "procutils.h"
#include "svncommitmsgsmgr.h"
#include "wx/busyinfo.h"
#include "globals.h"
#include "wx/menu.h"
#include "wx/xrc/xmlres.h"
#include "svndriver.h"
#include "wx/app.h"
#include "virtualdirtreectrl.h"
#include "wx/treectrl.h"
#include "svnhandler.h"
#include "svnoptionsdlg.h"
#include "exelocator.h"
#include "svnxmlparser.h"
#include "dirsaver.h"

#define VALIDATE_SVNPATH()\
	{\
		ExeLocator locator;\
		wxString where;\
		if(!locator.Locate(m_options.GetExePath(), where)){\
			wxString message;\
			message << wxT("SVN plugin error: failed to locate svn client installed (searched for: ") << m_options.GetExePath() << wxT(")");\
			wxLogMessage(message);\
			return;\
		}\
	}

static SubversionPlugin* theSvnPlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (theSvnPlugin == 0) {
		theSvnPlugin = new SubversionPlugin(manager);
	}
	return theSvnPlugin;
}

SubversionPlugin::SubversionPlugin(IManager *manager)
		: IPlugin(manager)
		, m_svnMenu(NULL)
		, m_svn(NULL)
		, topWin(NULL)
		, m_initIsDone(false)
		, m_sepItem(NULL)
{
	m_svn = new SvnDriver(this, manager);

	manager->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &m_options);
	//m_timer->Start((int)m_options.GetRefreshInterval(), true);

	m_longName = wxT("Subversion");
	m_shortName = wxT("SVN");

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	wxTextCtrl *svnwin = new wxTextCtrl(m_mgr->GetOutputPaneNotebook(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER| wxTE_MULTILINE);
	svnwin->SetFont(font);

	m_mgr->GetOutputPaneNotebook()->GetImageList()->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_repo")));
	m_mgr->GetOutputPaneNotebook()->AddPage(svnwin, wxT("SVN"), false, (int)m_mgr->GetOutputPaneNotebook()->GetImageList()->GetCount()-1);

	//Connect items
	if (!topWin) {
		topWin = wxTheApp;
	}

	if (topWin) {
		topWin->Connect(wxEVT_FILE_SAVED, wxCommandEventHandler(SubversionPlugin::OnFileSaved), NULL, this);
		topWin->Connect(wxEVT_FILE_EXP_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnFileExplorerInitDone), NULL, this);
		topWin->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SubversionPlugin::OnProjectFileAdded), NULL, this);
		topWin->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnAppInitDone), NULL, this);
	}

	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	//tree->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(SubversionPlugin::OnTreeExpanded), NULL, this);
}

wxMenu *SubversionPlugin::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);
	
	item = new wxMenuItem(menu, XRCID("svn_commit_file"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_update_file"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	
	menu->AppendSeparator();
	
	item = new wxMenuItem(menu, XRCID("svn_diff_file"), wxT("&Diff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	
	item = new wxMenuItem(menu, XRCID("svn_revert_file"), wxT("&Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	
	if (!topWin) {
		topWin = wxTheApp;
	}

	topWin->Connect(XRCID("svn_commit_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitFile), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_update_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateFile), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_revert_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevertFile), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_diff_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiffFile), NULL, (wxEvtHandler*)this);
	return menu;
}

wxMenu *SubversionPlugin::CreatePopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_update"), wxT("&Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_commit"), wxT("&Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_add"), wxT("&Add"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_delete"), wxT("&Delete"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_revert"), wxT("&Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_diff"), wxT("D&iff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_changelog"), wxT("Create Change &Log"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_cleanup"), wxT("Cl&eanup"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_refresh"), wxT("Show SVN S&tatus"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_abort"), wxT("A&bort Current Operation"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	if (!topWin) {
		topWin = wxTheApp;
	}

	topWin->Connect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRefreshFolderStatus), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCleanup), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_changelog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnChangeLog), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_abort"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAbort), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_delete"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDelete), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevert), NULL, (wxEvtHandler*)this);

	return menu;
}

SubversionPlugin::~SubversionPlugin()
{
	SvnCommitMsgsMgr::Release();
	UnPlug();
}

void SubversionPlugin::OnSvnAbort(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nAborting ...\n"));
	m_svn->Abort();
}

void SubversionPlugin::OnChangeLog(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->ChangeLog();
}

void SubversionPlugin::OnCleanup(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nPerforming cleanup ...\n"));
	m_svn->Cleanup();
}

void SubversionPlugin::OnUpdate(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	m_svn->Update();
}

void SubversionPlugin::OnCommit(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	m_svn->Commit();
}

void SubversionPlugin::OnCommitFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCommitting ...\n"));
	//get the current active editor name
	IEditor *editor = m_mgr->GetActiveEditor();
	if(editor){
		m_svn->CommitFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnUpdateFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nUpdating ...\n"));
	IEditor *editor = m_mgr->GetActiveEditor();
	if(editor){
		m_svn->UpdateFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnSvnAdd(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nAdding file(s)...\n"));
	m_svn->Add();
}

void SubversionPlugin::OnDiff(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));
	m_svn->Diff();
}

void SubversionPlugin::OnDiffFile(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(event);
	m_svn->PrintMessage(wxT("----\nCreating diff file...\n"));
	
	IEditor *editor = m_mgr->GetActiveEditor();
	if(editor){
		m_svn->DiffFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnRevertFile(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	IEditor *editor = m_mgr->GetActiveEditor();
	if(editor){
		m_svn->RevertFile(editor->GetFileName());
	}
}

void SubversionPlugin::OnDelete(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Delete();
}

void SubversionPlugin::OnRevert(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	wxUnusedVar(e);
	m_svn->Revert();
}

void SubversionPlugin::OnRefreshFolderStatus(wxCommandEvent &event)
{
	VALIDATE_SVNPATH();
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (item.m_item.IsOk()) {
		//Generate report for base directory
		if(item.m_fileName.IsDir()) {
			//Run the SVN command
			// Execute a sync command to get modified files 
			wxString command;
			wxArrayString output;
			
			DirSaver ds;
			wxSetWorkingDirectory(item.m_fileName.GetPath());
			
			command << wxT("\"") << this->GetOptions().GetExePath() << wxT("\" ");
			command << wxT("status --xml --non-interactive -q --no-ignore \"") << item.m_fileName.GetFullPath() << wxT("\"");
			ProcUtils::ExecuteCommand(command, output);
			
			DoGenerateReport(output);
			return;
		}
	}
	event.Skip();
}

wxToolBar *SubversionPlugin::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	return NULL;
}

void SubversionPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_options"), wxT("Options..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Subversion"), menu);

	if (!topWin) {
		topWin = wxTheApp;
	}
	topWin->Connect(XRCID("svn_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnOptions), NULL, (wxEvtHandler*)this);
}

void SubversionPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileExplorer) {
		menu->Append(XRCID("SVN_POPUP"), wxT("svn"), CreatePopMenu());
	} else if(type == MenuTypeEditor) {
		m_sepItem = menu->AppendSeparator();
		menu->Append(XRCID("SVN_EDITOR_POPUP"), wxT("svn"), CreateEditorPopMenu());
	}
}

void SubversionPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileExplorer) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_POPUP"));
		if (item) {
			menu->Destroy(item);
			if (topWin) {
				topWin->Disconnect(XRCID("svn_update"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdate), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_commit"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommit), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_diff"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiff), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_refresh"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRefreshFolderStatus), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_changelog"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnChangeLog), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_abort"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAbort), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_cleanup"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCleanup), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_add"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnSvnAdd), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_delete"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDelete), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_revert"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevert), NULL, (wxEvtHandler*)this);
			}
		}
	}
	else if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("SVN_EDITOR_POPUP"));
		if (item) {
			menu->Destroy(item);
			if (topWin) {
				topWin->Disconnect(XRCID("svn_commit_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnCommitFile), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_update_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnUpdateFile), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_revert_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnRevertFile), NULL, (wxEvtHandler*)this);
				topWin->Disconnect(XRCID("svn_diff_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SubversionPlugin::OnDiffFile), NULL, (wxEvtHandler*)this);
			}
		}
		if(m_sepItem){
			menu->Destroy(m_sepItem);
			m_sepItem = NULL;
		}
	}
}

void SubversionPlugin::OnFileSaved(wxCommandEvent &e)
{
	VALIDATE_SVNPATH();
	e.Skip();
}

void SubversionPlugin::OnOptions(wxCommandEvent &event)
{
	wxUnusedVar(event);
	SvnOptionsDlg *dlg = new SvnOptionsDlg(NULL, m_options);
	if (dlg->ShowModal() == wxID_OK) {
		m_options = dlg->GetOptions();
		m_mgr->GetConfigTool()->WriteObject(wxT("SubversionOptions"), &m_options);
	}
	dlg->Destroy();
}

void SubversionPlugin::UnPlug()
{
	if (m_svn) {
		m_svn->Shutdown();
		delete m_svn;
		m_svn = NULL;
	}

	if (m_svnMenu) {
		delete m_svnMenu;
		m_svnMenu = NULL;
	}
}

void SubversionPlugin::OnFileExplorerInitDone(wxCommandEvent &event)
{
	wxUnusedVar(event);
}

void SubversionPlugin::OnProjectFileAdded(wxCommandEvent &event)
{
	if (m_options.GetFlags() & SVN_AUTO_ADD_FILE) {
		void *cdata(NULL);
		wxArrayString files;
		cdata = event.GetClientData();
		if (cdata) {
			files = *((wxArrayString*)cdata);

			for (size_t i=0; i< files.GetCount(); i++) {
				m_svn->Add(files.Item(i));
			}

		}
	}
	event.Skip();
}

void SubversionPlugin::OnAppInitDone(wxCommandEvent &event)
{
	m_initIsDone = true;
}

void SubversionPlugin::DoGenerateReport(const wxArrayString &output)
{
	wxString path = m_mgr->GetStartupDirectory();
	wxString name = wxT("svnreport.html");
	
	wxFileName fn(path, name);
	wxHtmlWindow *reportPage = new wxHtmlWindow(m_mgr->GetMainNotebook(), wxID_ANY);
	m_mgr->GetMainNotebook()->AddPage(reportPage, wxT("SVN Status"), true);
}
