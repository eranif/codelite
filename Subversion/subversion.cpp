#include "subversion.h"
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

int SubversionPlugin::SvnConflictImageId;
int SubversionPlugin::SvnModifiedImageId;
int SubversionPlugin::SvnOkImageId;
int SubversionPlugin::CppOK;
int SubversionPlugin::CppModified;
int SubversionPlugin::CppConflict;
int SubversionPlugin::CConflict;
int SubversionPlugin::COK;
int SubversionPlugin::CModified;
int SubversionPlugin::TextOK;
int SubversionPlugin::TextModified;
int SubversionPlugin::TextConflict;
int SubversionPlugin::HeaderOK;
int SubversionPlugin::HeaderModified;
int SubversionPlugin::HeaderConflict;

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
		topWin->Connect(wxEVT_FILE_EXP_REFRESHED, wxCommandEventHandler(SubversionPlugin::OnRefreshFolderStatus), NULL, this);
		topWin->Connect(wxEVT_FILE_EXP_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnFileExplorerInitDone), NULL, this);
		topWin->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SubversionPlugin::OnRefreshFolderStatus), NULL, this);
		topWin->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SubversionPlugin::OnProjectFileAdded), NULL, this);
		topWin->Connect(wxEVT_INIT_DONE, wxCommandEventHandler(SubversionPlugin::OnAppInitDone), NULL, this);
	}

	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	tree->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(SubversionPlugin::OnTreeExpanded), NULL, this);
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

	item = new wxMenuItem(menu, XRCID("svn_refresh"), wxT("Re&fresh SVN Status"), wxEmptyString, wxITEM_NORMAL);
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

void SubversionPlugin::OnTreeExpanded(wxTreeEvent &event)
{
	VALIDATE_SVNPATH();
	if(!m_initIsDone){ return; }
		
	//dont allow any user interaction until refresh is done
	wxWindowDisabler disabler;
	wxTreeItemId item = event.GetItem();
	if (IsItemSvnDir(item)) {
		RefreshTreeStatus(&item);
	}
	event.Skip();
}

bool SubversionPlugin::IsItemSvnDir(wxTreeItemId &item)
{
	if (item.IsOk()) {
		VdtcTreeItemBase *b;
		wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
		b = (VdtcTreeItemBase*)tree->GetItemData(item);
		if (b && b->IsDir()) {
			wxFileName svnDir(tree->GetFullPath(item));
			svnDir.AppendDir(wxT(".svn"));
			if (svnDir.DirExists()) {
				return true;
			}
		}
	}
	return false;
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
	//refresh tree status
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (item.m_item.IsOk()) {
		RefreshTreeStatus(&item.m_item);
	}
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
	if(!m_initIsDone){ return; }
	TreeItemInfo info = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	if (IsItemSvnDir(info.m_item)) {
		
		wxBusyCursor bc;
		RefreshTreeStatus(&info.m_item);
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
	if (m_options.GetFlags() & SVN_UPDATE_ON_SAVE) {
		RefreshTreeStatus(NULL);
	}
	e.Skip();
}

void SubversionPlugin::RefreshTreeStatus(wxTreeItemId *tree_item)
{
	VALIDATE_SVNPATH();
	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	wxTreeItemId first;
	std::map<wxString, wxTreeItemId> svnRepos;

	if (!tree_item) {
		//get list of all SVN root directories
		//in the tree
		first = tree->GetFirstVisibleItem();
		ScanForSvnDirs(first, svnRepos, true);
	} else {
		if (IsSvnDirectory(tree->GetFullPath(*tree_item))) {
			svnRepos[tree->GetFullPath(*tree_item).GetFullPath()] = *tree_item;
		} else {
			return;
		}
	}

	//execute the status command for every directory in the list
	wxString output;

	std::map< wxString, wxTreeItemId >::const_iterator iter = svnRepos.begin();
	for ( ; iter != svnRepos.end() ; iter ++ ) {
		output.Clear();
		wxTreeItemId item = iter->second;
		wxString fullpath = iter->first;

		//try to determine whether this is a directory or file
		if (wxDir::Exists(fullpath)) {
			fullpath << wxT("/");
		}

		wxFileName tmpfn(fullpath);
		m_svn->ExecStatusCommand(tmpfn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR), output);

		DirSaver ds;
		wxSetWorkingDirectory(tmpfn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));

		//update the tree status
		wxArrayString cdirs;
		wxArrayString mdirs;
		wxArrayString unverfiles;

		//get list of files modified/conflicts
		SvnXmlParser::GetFiles(output, cdirs, SvnXmlParser::StateConflict);
		SvnXmlParser::GetFiles(output, mdirs, SvnXmlParser::StateModified);
		SvnXmlParser::GetFiles(output, unverfiles, SvnXmlParser::StateUnversioned);

		//update UI
		tree->Freeze();
		//set all icons as OK
		std::map<wxString, wxTreeItemId>::iterator it = svnRepos.begin();
		for (; it != svnRepos.end(); it++) {
			wxTreeItemId rootNode = it->second;

			VdtcTreeItemBase *a = (VdtcTreeItemBase *) tree->GetItemData(rootNode);
			if (a) {
				tree->SetItemImage(rootNode, GetOkIcon(a));
			}

			//get all visible children of this node and mark them as OK as well
			std::list< wxTreeItemId > children;
			GetAllChildren(rootNode, children);
			std::list< wxTreeItemId >::const_iterator i = children.begin();
			for (; i != children.end(); i++) {
				VdtcTreeItemBase *b = (VdtcTreeItemBase *) tree->GetItemData(*i);
				if (b) {
					int iconId = GetOkIcon(b);
					tree->SetItemImage((*i), iconId);
				}
			}
		}

		for (size_t i=0; i< unverfiles.GetCount(); i++) {
			//set all modified dirs first

			wxString tmppath(unverfiles.Item(i));

			if (wxDir::Exists(unverfiles.Item(i))) {
				//it is a directory
				tmppath << wxT("/");
			}

			wxFileName fn(tmppath);
			fn.MakeAbsolute();
			wxString dirpath = fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
			dirpath << fn.GetFullName();

			wxTreeItemId item_ = tree->GetItemByFullPath(dirpath);
			if (item_.IsOk()) {
				VdtcTreeItemBase *data = (VdtcTreeItemBase*)tree->GetItemData(item_);
				if (data) {
					tree->SetItemImage(item_, data->GetIconId());
				}
			}
		}


		//update modify items
		for (size_t i=0; i<mdirs.GetCount(); i++) {
			//set all modified dirs first
			wxFileName fn(mdirs.Item(i));
			fn.MakeAbsolute();

			wxString dirpath = fn.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
			dirpath << fn.GetFullName();
			wxTreeItemId item_ = tree->GetItemByFullPath(dirpath);

			if (item_.IsOk()) {
				VdtcTreeItemBase *data = (VdtcTreeItemBase*)tree->GetItemData(item_);
				if (data) {
					tree->SetItemImage(item_, GetModifiedIcon(data));
					UpdateParent(item_, data, mdirs.Item(i), SubversionPlugin::SvnModifiedImageId);
				}
			}
		}

		//update conflict items
		for (size_t i=0; i<cdirs.GetCount(); i++) {
			//set all modified dirs first
			wxFileName fn(cdirs.Item(i));
			fn.MakeAbsolute();

			wxString dirpath = fn.GetFullPath();
			wxTreeItemId item_ = tree->GetItemByFullPath(dirpath);
			if (item_.IsOk()) {
				VdtcTreeItemBase *data = (VdtcTreeItemBase*)tree->GetItemData(item_);
				if (data) {
					tree->SetItemImage(item_, GetConflictIcon(data));
					UpdateParent(item_, data, cdirs.Item(i), SubversionPlugin::SvnConflictImageId);
				}
			}
		}
		tree->Thaw();
	}

	//wxLogMessage(output);

}

void SubversionPlugin::UpdateParent(const wxTreeItemId &child, VdtcTreeItemBase *childData, const wxString &childPath, int imgId)
{
	if (!childData) {
		return;
	}

	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	wxString tmppath = childPath;
	if (childData->IsDir()) {
		tmppath << wxT("/");
	}

	wxFileName fn(tmppath);
	wxArrayString dirs = fn.GetDirs();
	size_t levelsToUpdate = dirs.GetCount()+1;
	wxTreeItemId item = child;
	for (size_t i=0; i<levelsToUpdate; i++) {
		item = tree->GetItemParent(item);
		if (item.IsOk()) {
			tree->SetItemImage(item, imgId);
		} else {
			break;
		}
	}
}

void SubversionPlugin::ScanForSvnDirs(const wxTreeItemId &item, std::map<wxString, wxTreeItemId> &svnRepos, bool allVisibles)
{
	VdtcTreeItemBase *b;
	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	wxTreeItemId child = item;
	wxTreeItemId stopAtChild;

	if (!allVisibles) {
		stopAtChild = tree->GetNextSibling(item);
	}

	while (child.IsOk())	{
		if (!tree->IsVisible(child))
			break;

		b = (VdtcTreeItemBase *)tree->GetItemData(child);
		wxFileName fn = tree->GetFullPath(child);
		wxString tmp = fn.GetFullPath();
		if (b && b->IsDir())	{
			if (IsSvnDirectory(fn)) {
				svnRepos[fn.GetFullPath()] = child;
			}
		}

		child = tree->GetNextVisible(child);
		if (!allVisibles && child == stopAtChild)
			break;
	}
}

bool SubversionPlugin::IsSvnDirectory(const wxFileName &fn)
{
	wxFileName svnDir(fn);
	wxFileName svnDir2(fn);
	svnDir.AppendDir(wxT(".svn"));
	svnDir2.AppendDir(wxT("_svn"));
	if (svnDir.DirExists()) {
		//We have a svn directory, no need to go down futher
		return true;
	}
	//try the second form of _svn
	else if (svnDir2.DirExists()) {
		//We have a svn directory, no need to go down futher
		return true;
	}
	return false;
}

void SubversionPlugin::GetAllChildren(const wxTreeItemId &item, std::list<wxTreeItemId> &children)
{
	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);

	wxTreeItemIdValue cookie;
	wxTreeItemId child = tree->GetFirstChild(item, cookie);

	while (child.IsOk()) {
		children.push_back(child);
		child = tree->GetNextChild(item, cookie);
	}
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


//Icons methods
int SubversionPlugin::GetOkIcon(VdtcTreeItemBase *data)
{
	wxString caption = data->GetCaption();
	caption.MakeLower();

	//directory
	if (data->IsDir()) {
		return SvnOkImageId;
	}
	//file
	if (caption.EndsWith(wxT(".c++")) || caption.EndsWith(wxT(".cpp")) || caption.EndsWith(wxT(".cxx")) || caption.EndsWith(wxT(".cc"))) {
		return CppOK;
	}

	if (caption.EndsWith(wxT(".h")) || caption.EndsWith(wxT(".h++")) || caption.EndsWith(wxT(".hpp"))) {
		return HeaderOK;
	}

	if (caption.EndsWith(wxT(".c"))) {
		return COK;
	}
	return TextOK;
}

int SubversionPlugin::GetModifiedIcon(VdtcTreeItemBase *data)
{
	wxString caption = data->GetCaption();
	caption.MakeLower();

	//directory
	if (data->IsDir()) {
		return SvnModifiedImageId;
	}
	//file
	if (caption.EndsWith(wxT(".c++")) || caption.EndsWith(wxT(".cpp")) || caption.EndsWith(wxT(".cxx")) || caption.EndsWith(wxT(".cc"))) {
		return CppModified;
	}

	if (caption.EndsWith(wxT(".h")) || caption.EndsWith(wxT(".h++")) || caption.EndsWith(wxT(".hpp"))) {
		return HeaderModified;
	}

	if (caption.EndsWith(wxT(".c"))) {
		return CModified;
	}
	return TextModified;
}

int SubversionPlugin::GetConflictIcon(VdtcTreeItemBase *data)
{
	wxString caption = data->GetCaption();
	caption.MakeLower();

	//directory
	if (data->IsDir()) {
		return SvnConflictImageId;
	}
	//file
	if (caption.EndsWith(wxT(".c++")) || caption.EndsWith(wxT(".cpp")) || caption.EndsWith(wxT(".cxx")) || caption.EndsWith(wxT(".cc"))) {
		return CppConflict;
	}

	if (caption.EndsWith(wxT(".h")) || caption.EndsWith(wxT(".h++")) || caption.EndsWith(wxT(".hpp"))) {
		return HeaderConflict;
	}

	if (caption.EndsWith(wxT(".c"))) {
		return CConflict;
	}
	return TextConflict;
}

void SubversionPlugin::OnFileExplorerInitDone(wxCommandEvent &event)
{
	if(!m_initIsDone){ return; }
	
	wxVirtualDirTreeCtrl* tree =  (wxVirtualDirTreeCtrl*)m_mgr->GetTree(TreeFileExplorer);
	//add new icons to the tree control
	wxImageList *il = tree->GetImageList();
	if (il) {
		SvnOkImageId		= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_ok")));
		SvnConflictImageId	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_conflict")));
		SvnModifiedImageId	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("svn_modified")));

		CppConflict		= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("cpp_conflict")));
		CppOK	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("cpp_ok")));
		CppModified	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("cpp_modified")));

		CConflict		= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("c_conflict")));
		COK	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("c_ok")));
		CModified	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("c_modified")));

		HeaderConflict		= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("h_conflict")));
		HeaderOK	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("h_ok")));
		HeaderModified	= il->Add(wxXmlResource::Get()->LoadBitmap(wxT("h_modified")));

		TextConflict = il->Add(wxXmlResource::Get()->LoadBitmap(wxT("text_conflict")));
		TextOK = il->Add(wxXmlResource::Get()->LoadBitmap(wxT("text_ok")));
		TextModified = il->Add(wxXmlResource::Get()->LoadBitmap(wxT("text_modified")));
	}
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
	//Initialize icons
	OnFileExplorerInitDone(event);
	wxBusyInfo wait(wxT("Updating 'Explorer' view with SVN status..."));
	
	//Notify the plugin that the file explorer tree has expanded
	SendCmdEvent(wxEVT_FILE_EXP_REFRESHED);
}
