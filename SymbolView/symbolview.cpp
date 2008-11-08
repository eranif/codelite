#include <set>
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/log.h>
#include "macros.h"
#include "workspace.h"
#include "ctags_manager.h"
#include "symbolview.h"


//--------------------------------------------
//Plugin Interface
//--------------------------------------------

static SymbolViewPlugin* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new SymbolViewPlugin(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Scott Dolim"));
	info.SetName(wxT("SymbolView"));
	info.SetDescription(wxT("Show Symbols of File, Project, or Workspace"));
	info.SetVersion(wxT("v1.0"));

	// this plugin starts as disabled by default
	info.SetEnabled(false);
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}


//--------------------------------------------
//Constructors/Destructors
//--------------------------------------------

SymbolViewPlugin::SymbolViewPlugin(IManager *manager)
		: IPlugin(manager)
		, m_symView(NULL)
		, m_viewChoice(NULL)
		, m_viewStack(NULL)
		, m_isLinkedToEditor(true)
		, m_imagesList(NULL)
{
	m_longName = wxT("Symbols View Plugin");
	m_shortName = wxT("SymbolView");

	LoadImagesAndIndexes();
	CreateGUIControls();
	Connect();

	m_mgr->GetWorkspacePaneNotebook()->AddPage(m_symView, wxT("Symbols"));
	//new DockablePane(manager->GetDockingManager()->GetManagedWindow(), NULL, m_symView, wxT("Symbol View"), wxNullBitmap, wxSize(200, 200));
}


SymbolViewPlugin::~SymbolViewPlugin()
{
	UnPlug();
	thePlugin = NULL;
}


//--------------------------------------------
//GUI setup methods
//--------------------------------------------

void SymbolViewPlugin::LoadImagesAndIndexes()
{
	m_viewModeNames.Add(wxEmptyString, vmMax);
	m_viewModeNames[vmCurrentFile]      = wxT("Current File");
	m_viewModeNames[vmCurrentProject]   = wxT("Current Project");
	m_viewModeNames[vmCurrentWorkspace] = wxT("Current Workspace");

	m_imagesList = new wxImageList(16, 16);

	// ATTN: the order of the images in this list determines the sorting of tree children (along with the child names).
	// That is why we sometimes reload the same image rather than just referring to the prior index.

	m_image[wxT("workspace")]           = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("workspace")));
	m_image[wxT("project")]             = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("project")));

	m_image[wxT("h")]                   = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_h")));
	m_image[wxT("hpp")]                 = m_image[wxT("h")];

	m_image[wxT("c")]                   = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_c")));
	m_image[wxT("cpp")]                 = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_cplusplus")));
	m_image[wxT("cxx")]                 = m_image[wxT("cpp")];
	m_image[wxT("c++")]                 = m_image[wxT("cpp")];
	m_image[wxT("cc")]                  = m_image[wxT("cpp")];

	m_image[wxT("file")]                = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("page_white_text")));

	m_image[wxT("class_view")]          = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("class_view")));
	m_image[wxT("globals")]             = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("globals")));
	m_image[wxT("namespace")]           = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("namespace")));

	m_image[wxT("macro")]               = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("typedef")), wxColor(0, 128, 128));
	m_image[wxT("macro_protected")]     = m_image[wxT("macro")];
	m_image[wxT("macro_public")]        = m_image[wxT("macro")];
	m_image[wxT("macro_private")]       = m_image[wxT("macro")];

	m_image[wxT("interface")]           = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("class")));
	m_image[wxT("interface_public")]    = m_image[wxT("interface")];
	m_image[wxT("interface_protected")] = m_image[wxT("interface")];
	m_image[wxT("interface_private")]   = m_image[wxT("interface")];

	m_image[wxT("class")]               = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("class")));
	m_image[wxT("class_public")]        = m_image[wxT("class")];
	m_image[wxT("class_protected")]     = m_image[wxT("class")];
	m_image[wxT("class_private")]       = m_image[wxT("class")];

	m_image[wxT("struct")]              = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("struct")));
	m_image[wxT("struct_public")]       = m_image[wxT("struct")];
	m_image[wxT("struct_protected")]    = m_image[wxT("struct")];
	m_image[wxT("struct_private")]      = m_image[wxT("struct")];

	m_image[wxT("union")]               = m_image[wxT("struct")];
	m_image[wxT("union_public")]        = m_image[wxT("union")];
	m_image[wxT("union_protected")]     = m_image[wxT("union")];
	m_image[wxT("union_private")]       = m_image[wxT("union")];

	m_image[wxT("enum")]                = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("enum")), wxColor(0, 128, 128));
	m_image[wxT("enum_public")]         = m_image[wxT("enum")];
	m_image[wxT("enum_protected")]      = m_image[wxT("enum")];
	m_image[wxT("enum_private")]        = m_image[wxT("enum")];

	m_image[wxT("typedef")]             = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("typedef")), wxColor(0, 128, 128));
	m_image[wxT("typedef_public")]      = m_image[wxT("typedef")];
	m_image[wxT("typedef_protected")]   = m_image[wxT("typedef")];
	m_image[wxT("typedef_private")]     = m_image[wxT("typedef")];

	m_image[wxT("prototype_public")]    = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_public")));
	m_image[wxT("prototype_protected")] = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_protected")));
	m_image[wxT("prototype_private")]   = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_private")));
	m_image[wxT("prototype")]           = m_image[wxT("prototype_public")];

	m_image[wxT("function_public")]     = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_public")));
	m_image[wxT("function_protected")]  = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_protected")));
	m_image[wxT("function_private")]    = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("func_private")));
	m_image[wxT("function")]            = m_image[wxT("function_public")];

	m_image[wxT("method_public")]       = m_image[wxT("function_public")];
	m_image[wxT("method_protected")]    = m_image[wxT("function_protected")];
	m_image[wxT("method_private")]      = m_image[wxT("function_private")];
	m_image[wxT("method")]              = m_image[wxT("method_public")];

	m_image[wxT("member_public")]       = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_public")));
	m_image[wxT("member_protected")]    = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_protected")));
	m_image[wxT("member_private")]      = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_private")));
	m_image[wxT("member")]              = m_image[wxT("member_public")];
	m_image[wxT("variable")]            = m_image[wxT("member_public")];

	m_image[wxT("enumerator")]          = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("enumerator")));

	m_image[wxT("default")]             = m_imagesList->Add(wxXmlResource::Get()->LoadBitmap(wxT("struct"))); // fallback for anything else
}

void SymbolViewPlugin::CreateGUIControls()
{
	m_symView = new wxPanel(m_mgr->GetDockingManager()->GetManagedWindow());

	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	m_symView->SetSizer(sz);

	wxToolBar *tb = new wxToolBar(m_symView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
	tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), wxT("Link Editor"), wxITEM_CHECK);
	tb->ToggleTool(XRCID("link_editor"), m_isLinkedToEditor);
	tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), wxT("Collapse All"), wxITEM_NORMAL);
	tb->AddTool(XRCID("gohome"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), wxT("Go to Active Editor Symbolss"), wxITEM_NORMAL);
	tb->Realize();
	sz->Add(tb, 0, wxEXPAND);

	// sizer for the drop button and view-mode choice box
	wxBoxSizer *ch = new wxBoxSizer(wxHORIZONTAL);
	sz->Add(ch, 0, wxEXPAND|wxALL, 1);

	m_viewStack = new WindowStack(m_symView);
	sz->Add(m_viewStack, 1, wxEXPAND|wxALL, 1);
	for (int i = 0; i < vmMax; i++) {
		m_viewStack->Add(new WindowStack(m_viewStack), m_viewModeNames[i]);
	}
	m_viewStack->Select(m_viewModeNames[vmCurrentFile]);

	m_viewChoice = new wxChoice(m_symView, wxID_ANY);
	m_viewChoice->AppendString(m_viewModeNames[vmCurrentFile]);
	m_viewChoice->Select(0);
	ch->Add(m_viewChoice, 1, wxEXPAND|wxALL, 1);

	m_stackChoice = new StackButton(m_symView, (WindowStack*) m_viewStack->GetSelected());
	ch->Add(m_stackChoice, 0, wxEXPAND|wxALL, 1);

	sz->Layout();
}

void SymbolViewPlugin::Connect()
{
	m_symView->Connect(XRCID("link_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnLinkEditor), NULL, this);
	m_symView->Connect(XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnCollapseAll), NULL, this);
	m_symView->Connect(XRCID("collapse_all"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SymbolViewPlugin::OnCollapseAllUI), NULL, this);
	m_symView->Connect(XRCID("gohome"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnGoHome), NULL, this);
	m_symView->Connect(XRCID("gohome"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SymbolViewPlugin::OnGoHomeUI), NULL, this);

	m_stackChoice->Connect(wxID_ANY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SymbolViewPlugin::OnGoHomeUI), NULL, this);

	m_viewChoice->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(SymbolViewPlugin::OnViewModeMouseDown), NULL, this);
	m_viewChoice->Connect(wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnViewTypeChanged), NULL, this);

	wxEvtHandler *topwin = m_mgr->GetTheApp();
	topwin->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceLoaded), NULL, this);
	topwin->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileAdded), NULL, this);
	topwin->Connect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileRemoved), NULL, this);
	topwin->Connect(wxEVT_PROJ_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectAdded), NULL, this);
	topwin->Connect(wxEVT_PROJ_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectRemoved), NULL, this);
	topwin->Connect(wxEVT_SYNBOL_TREE_UPDATE_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsUpdated), NULL, this);
	topwin->Connect(wxEVT_SYNBOL_TREE_DELETE_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsDeleted), NULL, this);
	topwin->Connect(wxEVT_SYNBOL_TREE_ADD_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsAdded), NULL, this);
	topwin->Connect(wxEVT_FILE_RETAGGED, wxCommandEventHandler(SymbolViewPlugin::OnFileRetagged), NULL, this);
	topwin->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewPlugin::OnActiveEditorChanged), NULL, this);
	topwin->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewPlugin::OnEditorClosed), NULL, this);
	topwin->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(SymbolViewPlugin::OnAllEditorsClosed), NULL, this);
	topwin->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceClosed), NULL, this);
}


//--------------------------------------------
//Public properties
//--------------------------------------------

SymbolViewPlugin::ViewMode SymbolViewPlugin::GetViewMode() const
{
	return ViewMode(m_viewModeNames.Index(m_viewStack->GetSelectedKey()));
}


//--------------------------------------------
//Abstract methods
//--------------------------------------------

wxToolBar *SymbolViewPlugin::CreateToolBar(wxWindow *parent)
{
	return NULL;
}

void SymbolViewPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	//TODO:: create the menu for the 'Plugin' menu entry in the menu bar
}

void SymbolViewPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Append items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {
	} else if (type == MenuTypeFileView_Workspace) {
	} else if (type == MenuTypeFileView_Project) {
	} else if (type == MenuTypeFileView_Folder) {
	} else if (type == MenuTypeFileView_File) {
	}
}

void SymbolViewPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		//TODO::Unhook items for the editor context menu
	} else if (type == MenuTypeFileExplorer) {
	} else if (type == MenuTypeFileView_Workspace) {
	} else if (type == MenuTypeFileView_Project) {
	} else if (type == MenuTypeFileView_Folder) {
	} else if (type == MenuTypeFileView_File) {
	}
}

void SymbolViewPlugin::UnPlug()
{
	if (!m_symView)
		return;

	wxEvtHandler *topwin = m_mgr->GetTheApp();

	topwin->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceLoaded), NULL, this);
	topwin->Disconnect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileAdded), NULL, this);
	topwin->Disconnect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileRemoved), NULL, this);
	topwin->Disconnect(wxEVT_PROJ_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectAdded), NULL, this);
	topwin->Disconnect(wxEVT_PROJ_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectRemoved), NULL, this);
	topwin->Disconnect(wxEVT_SYNBOL_TREE_UPDATE_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsUpdated), NULL, this);
	topwin->Disconnect(wxEVT_SYNBOL_TREE_DELETE_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsDeleted), NULL, this);
	topwin->Disconnect(wxEVT_SYNBOL_TREE_ADD_ITEM, wxCommandEventHandler(SymbolViewPlugin::OnSymbolsAdded), NULL, this);
	topwin->Disconnect(wxEVT_FILE_RETAGGED, wxCommandEventHandler(SymbolViewPlugin::OnFileRetagged), NULL, this);
	topwin->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewPlugin::OnActiveEditorChanged), NULL, this);
	topwin->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewPlugin::OnEditorClosed), NULL, this);

	Notebook *notebook = m_mgr->GetWorkspacePaneNotebook();
	size_t notepos = notebook->GetPageIndex(m_symView);
	if (notepos != Notebook::npos) {
		notebook->DeletePage(notepos);
	} else {
		wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
		m_symView->GetParent()->ProcessEvent(event);
	}

	m_symView = NULL;
}


//--------------------------------------------
//Helper methods
//--------------------------------------------

/**
 * Determine a symbol tree path from the current editor, using the current view mode.
 */
wxString SymbolViewPlugin::GetSymbolsPath(IEditor* editor)
{
	switch (GetViewMode()) {
	case vmCurrentWorkspace:
		if (m_mgr->IsWorkspaceOpen()) {
			return m_mgr->GetWorkspace()->GetWorkspaceFileName().GetFullPath();
		}
		break;
	case vmCurrentProject: {
		wxString dummy;
		wxString projectName =
		    editor && !editor->GetProjectName().IsEmpty() ? editor->GetProjectName()
		    : m_mgr->GetWorkspace()->GetActiveProjectName();
		ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, dummy);
		if (project) {
			return project->GetFileName().GetFullPath();
		}
		break;
	}
	default:
		if (editor && !editor->GetProjectName().IsEmpty()) {
			return editor->GetFileName().GetFullPath();
		}
		break;
	}
	return wxEmptyString;
}

/**
 * Produce a list of files that contain tags, based on the name of the specified path.  If path is the workspace file,
 * gets all files in workspace.  If path is project file, gets all files in that project.  Else if path is a source
 * file, gets just that file and maybe a corresponding header file.
 */
void SymbolViewPlugin::GetFiles(const wxFileName &path, wxArrayString &files)
{
	if (!m_mgr->IsWorkspaceOpen())
		return;
	wxString fullPath = path.GetFullPath();
	wxString workspaceFileName = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetFullPath();
	wxArrayString projectNames;
	m_mgr->GetWorkspace()->GetProjectList(projectNames);
	for (size_t i = 0; i < projectNames.Count(); i++) {
		wxString dummy;
		ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectNames[i], dummy);
		if (!project)
			continue;
		wxString projectFileName = project->GetFileName().GetFullPath();
		std::vector<wxFileName> projectFiles;
		project->GetFiles(projectFiles, true);
		for (size_t j = 0; j < projectFiles.size(); j++) {
			wxFileName &fileName = projectFiles[j];
			wxString file = fileName.GetFullPath();
			if (fullPath == workspaceFileName || fullPath == projectFileName || fullPath == file) {
				files.Add(file);
			} else if (path.GetExt() != wxT("h") && fileName.GetExt() == wxT("h")) {
				// TODO: replace this code with a "real" solution based on actual file dependencies.
				// for now, make sure .c or .cpp file also includes corresponding .h file.
				// FIXME: this only works if the .h file is in the same directory as the .c/.cpp file.
				fileName.SetExt(path.GetExt());
				if (fullPath == fileName.GetFullPath())
					files.Add(file);
			}
		}
	}
}

/**
 * For a given set of source files, returns the set of trees (id'd by their path name) that want to show
 * tags from those source files.
 */
void SymbolViewPlugin::GetPaths(const wxArrayString &files, std::multimap<wxString, wxString> &filePaths)
{
	if (!m_mgr->IsWorkspaceOpen())
		return;

	wxString workspaceFileName = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetFullPath();

	// convert to set for faster membership testing, while adding obvious paths (own file, and entire workspace)
	std::set<wxString> fileset;
	for (size_t i = 0; i < files.Count(); i++) {
		if (fileset.insert(files[i]).second) {
			filePaths.insert(std::make_pair(files[i], files[i]));
			filePaths.insert(std::make_pair(files[i], workspaceFileName));
		}
	}

	// get projects that each file belongs to.
	wxArrayString projectNames;
	m_mgr->GetWorkspace()->GetProjectList(projectNames);
	for (size_t i = 0; i < projectNames.Count(); i++) {
		wxString dummy;
		ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectNames[i], dummy);
		if (!project)
			continue;
		wxString projectFileName = project->GetFileName().GetFullPath();
		std::vector<wxFileName> projectFiles;
		project->GetFiles(projectFiles, true);
		for (size_t j = 0; j < projectFiles.size(); j++) {
			wxFileName fileName = projectFiles[j];
			wxString filePath = fileName.GetFullPath();
			if (fileset.find(filePath) != fileset.end()) {
				filePaths.insert(std::make_pair(filePath, projectFileName));
			}
			if (fileName.GetExt() != wxT("h")) {
				// TODO: replace this code with a "real" solution based on actual file dependencies.
				// for now, make sure .h file also includes corresponding .c or .cpp file.
				// FIXME: this only works if the .h file is in the same directory as the .c/.cpp file.
				fileName.SetExt(wxT("h"));
				wxString headerPath = fileName.GetFullPath();
				if (fileset.find(headerPath) != fileset.end()) {
					filePaths.insert(std::make_pair(headerPath, filePath));
				}
			}
		}
	}
}

/**
 * Given list of files, return all tags in the workspace database that are found in those files.
 * An empty list implicitly means to get all tags in the workspace (for speed).  Also filter on
 * scope, if provided.
 */
wxSQLite3ResultSet SymbolViewPlugin::GetTags(const wxString &scope, const wxArrayString &files)
{
	wxString sql;
	sql << wxT("select * from tags");
	if (!scope.IsEmpty()) {
		sql << wxT(" where");
		sql << wxT(" scope = '") << scope << wxT("'");
	}
	if (!files.IsEmpty()) {
		sql << (scope.IsEmpty() ? wxT(" where") : wxT(" and"));
		if (files.Count() == 1) {
			sql << wxT(" file = '") << files[0] << wxT("'");
		} else {
			sql << wxT(" file in ('") << files[0] << wxT("'");
			for (size_t i = 1; i < files.Count(); i++) {
				sql << wxT(", '") << files[i] << wxT("'");
			}
			sql << wxT(")");
		}
	}
	sql << wxT(";");
	return m_mgr->GetTagsManager()->GetDatabase()->Query(sql);
}


//--------------------------------------------
//Tree-related methods
//--------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SymbolViewPlugin::SymTree, wxTreeCtrl)

/**
 * Compares two nodes.  Used by wxTreeCtrl::SortChildren()
 */
int SymbolViewPlugin::SymTree::OnCompareItems(const wxTreeItemId &id1, const wxTreeItemId &id2)
{
	int cmp = GetItemImage(id1) - GetItemImage(id2);
	return cmp ? cmp : wxTreeCtrl::OnCompareItems(id1, id2);
}


/**
 * Find and return the sym tree matching the specified path, or NULL if not found.
 */
SymbolViewPlugin::SymTree* SymbolViewPlugin::FindSymbolTree(const wxString& path)
{
	for (size_t i = 0; i < m_viewModeNames.Count(); i++) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[i]);
		if (viewStack) {
			SymTree *tree = (SymTree*) viewStack->Find(path);
			if (tree)
				return tree;
		}
	}
	return NULL;
}

/**
 * Returns the root node or correct top-level grouping node in which global tag belongs.
 */
wxTreeItemId SymbolViewPlugin::GetParentForGlobalTag(SymTree *tree, const TagEntry &tag)
{
	if (tag.GetKind() == wxT("macro"))
		return tree->m_macros;
	if (tag.GetKind() == wxT("prototype"))
		return tree->m_protos;
	if (tag.GetKind() == wxT("function") || tag.GetKind() == wxT("function") || tag.GetKind() == wxT("function"))
		return tree->m_globals;
	return tree->GetRootItem();
}

/**
 * Sets tree node appearance parameters (icon, font, etc) from tag data.
 */
void SymbolViewPlugin::SetNodeData(wxTreeCtrl *tree, wxTreeItemId id, const TagEntry &tag)
{
	// attach tag data to the tree node
	TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
	if (treetag) {
		*(TagEntry*) treetag = tag;
	} else {
		treetag = new TagTreeData(this, tree, id, tag);
	}

	// display name
	wxString displayName = treetag->GetName();
	displayName << treetag->GetSignature();
	tree->SetItemText(id, displayName);

	// icon
	wxString icon;
	icon << treetag->GetKind();
	if (!treetag->GetAccess().IsEmpty()) {
		icon << wxT('_') << treetag->GetAccess();
	}
	std::map<wxString,int>::iterator iter = m_image.find(icon);
	if (iter == m_image.end()) {
		iter = m_image.find(wxT("default"));
	}
	int index = iter->second;
	tree->SetItemImage(id, index);

	// font
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if (treetag->GetKind() == wxT("prototype")) {
		font.SetStyle(wxFONTSTYLE_ITALIC);
	}
	if (treetag->GetAccess() == wxT("public")) {
		font.SetWeight(wxFONTWEIGHT_BOLD);
	}
	tree->SetItemFont(id, font);

	// whether or not it has an expander
	tree->SetItemHasChildren(id, treetag->IsContainer());

	// add parent node to set of nodes that need their children sorted
	WindowStack *viewStack = (WindowStack*) tree->GetParent();
	id = tree->GetItemParent(id);
	treetag = (TagTreeData*) tree->GetItemData(id);
	wxString path = viewStack->Find(tree);
	wxString key = treetag ? treetag->Key() : tree->GetItemText(id);
	m_sortNodes[TagKey(path,key)] = TreeNode(tree,id);
}

/**
 * Sort children of a bunch of nodes.
 */
void SymbolViewPlugin::SortChildren()
{
	for (std::map<TagKey, TreeNode>::iterator i = m_sortNodes.begin(); i != m_sortNodes.end(); i++) {
		wxTreeCtrl *tree = i->second.first;
		wxTreeItemId id = i->second.second;
		tree->SortChildren(id);
		tree->SetItemHasChildren(id);
	}
	m_sortNodes.clear();
}

/**
 * Add new symbol to all trees that would currently be showing it.
 */
int SymbolViewPlugin::AddSymbol(const TagEntry &tag, const std::multimap<wxString, wxString> &filePaths)
{
	int count = 0;
	if (tag.GetScope() != wxT("<global>")) {
		// tag is a scoped symbol (eg "Foo::Foo") so look for matching scope nodes to add it to as a child
		for (Path2TagRange range = m_pathTags.equal_range(tag.GetScope()); range.first != range.second; range.first++) {
			wxTreeCtrl *tree = range.first->second.first;
			wxTreeItemId parent = range.first->second.second;
			TagTreeData *treetag = (TagTreeData*) tree->GetItemData(parent);
			// make sure the scope tag came from a valid file relative to the new tag's file
			// (for example if tag is Foo::Foo() from file foo.cpp, then class Foo can come from files foo.cpp or foo.h
			// -- where foo.cpp and foo.h are in the same project)
			std::pair<std::multimap<wxString,wxString>::const_iterator,
			std::multimap<wxString,wxString>::const_iterator> files = filePaths.equal_range(treetag->GetFile());
			while (files.first != files.second && files.first->second != tag.GetFile()) {
				files.first++;
			}
			if (files.first != files.second) {
				wxTreeItemId child = tree->AppendItem(parent, wxEmptyString);
				SetNodeData(tree, child, tag);
				range.second = m_pathTags.upper_bound(tag.GetScope()); // recalculate invalidated upper-bound
				count++;
			}
		}
	} else {
		// tag is a global symbol so look for all trees showing tags from tag's file (file tree, project tree, workspace tree)
		std::pair<std::multimap<wxString,wxString>::const_iterator,
		std::multimap<wxString,wxString>::const_iterator> files = filePaths.equal_range(tag.GetFile());
		for (; files.first != files.second; files.first++) {
			SymTree *tree = FindSymbolTree(files.first->second);
			if (tree) {
				wxTreeItemId parent = GetParentForGlobalTag(tree, tag);
				wxTreeItemId child = tree->AppendItem(parent, wxEmptyString);
				SetNodeData(tree, child, tag);
				count++;
			}
		}
	}
	return count;
}

/**
 * Update all nodes in all trees that represent the specified tag.
 */
int SymbolViewPlugin::UpdateSymbol(const TagEntry &tag)
{
	int count = 0;
	for (Path2TagRange range = m_pathTags.equal_range(tag.Key()); range.first != range.second; range.first++) {
		wxTreeCtrl *tree = range.first->second.first;
		wxTreeItemId id = range.first->second.second;
		TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
		// make sure the tag came from the same file as the event before updating it
		if (treetag->GetFile() == tag.GetFile()) {
			if (*treetag == tag) {
				// nothing to update (still count it though, since there's a match)
			} else if (treetag->GetDifferOnByLineNumber()) {
				// just fix the line number to reduce amount of child-sorting
				treetag->SetLine(tag.GetLine());
			} else {
				SetNodeData(tree, id, tag);
				range.second = m_pathTags.upper_bound(tag.Key()); // recalculate invalidated upper-bound
			}
			count++;
		}
	}
	return count;
}

/**
 * Delete all nodes in all trees that represent the specified tag.
 */
int SymbolViewPlugin::DeleteSymbol(const TagEntry &tag)
{
	int count = 0;
	Path2TagRange range = m_pathTags.equal_range(tag.Key());
	for (Path2TagMap::iterator next = range.first; range.first != range.second; range.first = next) {
		wxTreeCtrl *tree = range.first->second.first;
		wxTreeItemId id = range.first->second.second;
		TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
		// make sure the tag came from the same file as the event before deleting it
		if (treetag->GetFile() == tag.GetFile()) {
			tree->DeleteChildren(id);
			next++; // this has to come between deleting the children and deleting the node itself, or the iterator might get invalidated
			tree->Delete(id);
			range.second = m_pathTags.upper_bound(tag.Key()); // recalculate invalidated upper-bound
			count++;
		} else {
			next++; // skip
		}
	}
	return count;
}

/**
 * Remove from all trees any nodes whose symbols came from specified file.
 */
int SymbolViewPlugin::DeleteFileSymbols(const wxString &file)
{
	int count = 0;
	File2TagMap::iterator iter;
	while ((iter = m_fileTags.find(file)) != m_fileTags.end()) {
		wxTreeCtrl *tree = iter->second.first;
		wxTreeItemId id = iter->second.second;
		tree->Delete(id);
		count++;
	}
	return count;
}

/**
 * Dynamically add the children of the specified tree node by querying the workspace database.
 */
int SymbolViewPlugin::LoadChildren(SymTree *tree, wxTreeItemId id)
{
	int count = 0;

	// root node gets special grouping children
	if (id == tree->GetRootItem()) {
		tree->m_globals = tree->AppendItem(id, wxT("Global Functions and Variables"), m_image[wxT("globals")]);
		tree->m_protos  = tree->AppendItem(id, wxT("Functions Prototypes"),           m_image[wxT("globals")]);
		tree->m_macros  = tree->AppendItem(id, wxT("Macros"),                         m_image[wxT("globals")]);
	} else {
		tree->SetItemHasChildren(id, false);
	}

	// get scope to scan for tags
	TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
	wxString scope = treetag ? treetag->GetPath() : wxString(wxT("<global>"));

	// get files to scan for tags
	wxArrayString files;
	ViewMode viewMode = GetViewMode();
	if (viewMode != vmCurrentWorkspace) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		GetFiles(viewStack->Find(tree), files);
	}

	// query database for the tags that go under this node
	wxSQLite3ResultSet res = GetTags(scope, files);
	while (res.NextRow()) {
		TagEntry tag(res);
		wxTreeItemId parent = id != tree->GetRootItem() ? id : GetParentForGlobalTag(tree, tag);
		// create child node, add our custom tag data to it, and set its appearance accordingly
		wxTreeItemId child = tree->AppendItem(parent, wxEmptyString);
		SetNodeData(tree, child, tag);
		count++;
	}
	SortChildren();

	return count;
}

/**
 * Initialize a brand new symbol tree for the specified path.
 */
void SymbolViewPlugin::CreateSymbolTree(const wxString &path, WindowStack *parent)
{
	if (path.IsEmpty() || !parent)
		return;

	// make new empty tree
	SymTree *tree = new SymTree(parent);
	parent->Add(tree, path);
	tree->SetImageList(m_imagesList);

	// set root node text and icon
	wxFileName fn(path);
	std::map<wxString,int>::iterator i = m_image.find(fn.GetExt());
	wxTreeItemId root;
	if (i != m_image.end()) {
		root = tree->AddRoot(fn.GetName(), i->second);
	} else {
		root = tree->AddRoot(fn.GetFullName(), m_image[wxT("file")]);
	}

	// add the top level children and expand
	LoadChildren(tree, root);
	tree->Expand(root);

	// hook up event handlers
	tree->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(SymbolViewPlugin::OnNodeExpanding), NULL, this);
	tree->Connect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(SymbolViewPlugin::OnNodeActivated), NULL, this);
	tree->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SymbolViewPlugin::OnNodeDClick), NULL, this);
}

/**
 * Select the symbol tree in the specified viewStack (use current if NULL).  Create it if necessary.
 */
void SymbolViewPlugin::ShowSymbolTree()
{
	wxString path = GetSymbolsPath(m_mgr->GetActiveEditor());
	if (!path.IsEmpty()) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (viewStack->GetSelectedKey() != path) {
			m_viewStack->Freeze();
			if (!viewStack->Find(path)) {
				CreateSymbolTree(path, viewStack);
			}
			viewStack->Select(path);
			m_viewStack->Thaw();
		}
	}
}


//--------------------------------------------
//Event handlers
//--------------------------------------------

/**
 * Show list of possible view modes for user to choose.
 */
void SymbolViewPlugin::OnViewModeMouseDown(wxMouseEvent& e)
{
	m_viewChoice->Freeze();
	m_viewChoice->Clear();
	for (int i = 0; i < vmMax; i++) {
		m_viewChoice->AppendString(m_viewModeNames[i]);
	}
	m_viewChoice->SetStringSelection(m_viewStack->GetSelectedKey());
	m_viewChoice->Thaw();
	e.Skip();
}

/**
 * Change to selected view and, if no tree is selected in the new view, show current editor's tree.
 */
void SymbolViewPlugin::OnViewTypeChanged(wxCommandEvent& e)
{
	m_viewStack->Select(e.GetString());
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	m_stackChoice->SetWindowStack(viewStack);
	if (viewStack->GetSelected() == NULL || m_isLinkedToEditor) {
		ShowSymbolTree();
	}
	e.Skip();
}

void SymbolViewPlugin::OnStackChoiceUI(wxUpdateUIEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	e.Enable(!m_isLinkedToEditor && viewStack->GetSelected() != NULL);
}

/**
 * Respond to click of the "Collapse All" toolbar button by collapsing the currently shown symbol tree.
 */
void SymbolViewPlugin::OnCollapseAll(wxCommandEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	wxTreeCtrl *tree = (wxTreeCtrl*) viewStack->GetSelected();
	if (tree) {
		tree->Freeze();
		tree->CollapseAll();
		tree->Expand(tree->GetRootItem());
		tree->Thaw();
	}
	e.Skip();
}

void SymbolViewPlugin::OnCollapseAllUI(wxUpdateUIEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	e.Enable(viewStack->GetSelected() != NULL);
}

/**
 * Jump to the current editor's symbol tree.
 */
void SymbolViewPlugin::OnGoHome(wxCommandEvent& e)
{
	ShowSymbolTree();
	e.Skip();
}

void SymbolViewPlugin::OnGoHomeUI(wxUpdateUIEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	e.Enable(!m_isLinkedToEditor && viewStack->GetSelected() != NULL);
}

/**
 * Toggle the link-to-editor state.  If now linked, get rid of extraneous symbol trees.
 */
void SymbolViewPlugin::OnLinkEditor(wxCommandEvent& e)
{
	m_isLinkedToEditor = !m_isLinkedToEditor;
	if (m_isLinkedToEditor) {
		ShowSymbolTree();
	}
	e.Skip();
}

/**
 * Add children (if necessary) to the node that the user has expanded.
 */
void SymbolViewPlugin::OnNodeExpanding(wxTreeEvent& e)
{
	SymTree *tree = (SymTree*) e.GetEventObject();
	wxTreeItemId id = e.GetItem();
	if (tree->ItemHasChildren(id) && tree->GetChildrenCount(id) == 0) {
		LoadChildren(tree, id);
	}
	e.Skip();
}

/**
 * Open the chosen symbol's file, and select the text where it is defined.
 */
void SymbolViewPlugin::OnNodeActivated(wxTreeEvent& e)
{
	if(e.GetKeyCode() == WXK_RETURN){
		wxTreeCtrl *tree = (wxTreeCtrl*) e.GetEventObject();
		DoActivateSelection(tree);
	} else {
		e.Skip();
	}
}

/**
 * Display and handle right-click menu for tree item.
 */
void SymbolViewPlugin::OnNodeContextMenu(wxTreeEvent& e)
{
	// TODO: show right-click menu
	e.Skip();
}

/**
 * Respond to workspace loaded by, if in workspace view mode, showing the workspace symbol tree.
 */
void SymbolViewPlugin::OnWorkspaceLoaded(wxCommandEvent& e)
{
	if (GetViewMode() == vmCurrentWorkspace) {
		ShowSymbolTree();
	}
	e.Skip();
}

/**
 * When workspace is closed, clear everything.
 */
void SymbolViewPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
	m_viewStack->Freeze();
	for (size_t i = 0; i < m_viewModeNames.Count(); i++) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[i]);
		if (viewStack) {
			viewStack->Clear();
		}
	}
	m_viewStack->Thaw();
	e.Skip();
}

/**
 * Respond to files retagged by updating all trees: add, modify, or delete tags as necessary.
 * @note This function is also called when a new project is added to the workspace.
 */
void SymbolViewPlugin::OnFileRetagged(wxCommandEvent& e)
{
	std::vector<wxFileName> *files = (std::vector<wxFileName>*) e.GetClientData();
	if (files && !files->empty()) {
		m_viewStack->Freeze();

		// collect files that have been retagged, and all tags in current trees that came from these files
		wxArrayString paths;
		std::map<TagKey, TreeNode> tagsToDelete;
		for (size_t i = 0; i < files->size(); i++) {
			paths.Add(files->at(i).GetFullPath());
			for (File2TagRange range = m_fileTags.equal_range(paths.Last()); range.first != range.second; range.first++) {
				wxTreeCtrl *tree = range.first->second.first;
				wxTreeItemId id = range.first->second.second;
				TagTreeData *tag = (TagTreeData*) tree->GetItemData(id);
				tagsToDelete[TagKey(tag->GetFile(), tag->Key())] = TreeNode(tree,id);
			}
		}

		// query database for current tags of retagged files.
		// update or add new symbols and remove those from the to-delete list
		std::multimap<wxString,wxString> filePaths;
		GetPaths(paths, filePaths);
		wxSQLite3ResultSet res = GetTags(wxEmptyString, paths);
		while (res.NextRow()) {
			TagEntry tag(res);
			if (UpdateSymbol(tag) || AddSymbol(tag, filePaths)) {
				tagsToDelete.erase(TagKey(tag.GetFile(), tag.Key()));
			}
		}
		SortChildren();

		// remove from trees all tags no longer in the database
		for (std::map<TagKey,TreeNode>::iterator i = tagsToDelete.begin(); i != tagsToDelete.end(); i++) {
			wxTreeCtrl *tree = i->second.first;
			wxTreeItemId id = i->second.second;
			if (id.IsOk()) {
				tree->Delete(id);
			}
		}

		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Respond to files added to existing project by fixing up our maps and trees.
 */
void SymbolViewPlugin::OnProjectFileAdded(wxCommandEvent& e)
{
	wxArrayString *files = (wxArrayString*) e.GetClientData();
	if (files && !files->IsEmpty()) {
		m_viewStack->Freeze();

		std::multimap<wxString, wxString> filePaths;
		GetPaths(*files, filePaths);
		wxSQLite3ResultSet res = GetTags(wxEmptyString, *files);
		while (res.NextRow()) {
			AddSymbol(res, filePaths);
		}
		SortChildren();

		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Respond to files removed from a project by fixing up our maps and trees.
 */
void SymbolViewPlugin::OnProjectFileRemoved(wxCommandEvent& e)
{
	wxArrayString *files = (wxArrayString*) e.GetClientData();
	if (files && !files->IsEmpty()) {
		m_viewStack->Freeze();
		for (size_t i = 0; i < files->Count(); i++) {
			DeleteFileSymbols(files->Item(i));
		}
		SortChildren();
		// if the tree that was being displayed got deleted, try to show another
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (viewStack->GetSelected() == NULL) {
			ShowSymbolTree();
		}
		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Placeholder function for now, since there is nothing to do.  Adding of project's files is
 * handled by OnFileRetagged() which is called first.
 */
void SymbolViewPlugin::OnProjectAdded(wxCommandEvent& e)
{
	e.Skip();
}

/**
 * Placeholder function for now, since there is nothing to do.  Removal of project's files is
 * handled by OnProjectFileRemoved() which is called first.
 */
void SymbolViewPlugin::OnProjectRemoved(wxCommandEvent& e)
{
	e.Skip();
}

/**
 * Respond to added symbols event (from ParseThread) by fixing up our maps and trees.
 */
void SymbolViewPlugin::OnSymbolsAdded(wxCommandEvent& e)
{
	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
		m_viewStack->Freeze();
		wxArrayString files;
		files.Add(data->GetFileName());
		std::multimap<wxString, wxString> filePaths;
		GetPaths(files, filePaths);
		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
		for (size_t i = 0; i < tags.size(); i++) {
			AddSymbol(tags[i].second, filePaths);
		}
		SortChildren();
		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Respond to updated symbols event (from ParseThread) by fixing up our maps and trees.
 */
void SymbolViewPlugin::OnSymbolsUpdated(wxCommandEvent& e)
{
	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
		m_viewStack->Freeze();
		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
		for (size_t i = 0; i < tags.size(); i++) {
			UpdateSymbol(tags[i].second);
		}
		SortChildren();
		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Respond to deleted symbols event (from ParseThread) by fixing up our maps and trees.
 */
void SymbolViewPlugin::OnSymbolsDeleted(wxCommandEvent& e)
{
	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
	if (data && !data->GetItems().empty()) {
		m_viewStack->Freeze();
		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
		for (size_t i = 0; i < tags.size(); i++) {
			DeleteSymbol(tags[i].second);
		}
		// if the tree that was being displayed got deleted, try to show another
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (viewStack->GetSelected() == NULL) {
			ShowSymbolTree();
		}
		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * Respond to active editor change by, if allowed, showing the appropriate symbol tree.
 */
void SymbolViewPlugin::OnActiveEditorChanged(wxCommandEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	if (viewStack->GetSelected() == NULL || m_isLinkedToEditor) {
		ShowSymbolTree();
	}
	e.Skip();
}

/**
 * Remove the editor's symbol tree.  If no other editors viewing the same project, remove the project symbol tree.
 */
void SymbolViewPlugin::OnEditorClosed(wxCommandEvent& e)
{
	IEditor *editor = (IEditor*) e.GetClientData();
	if (editor && !editor->GetProjectName().IsEmpty() && m_isLinkedToEditor) {
		m_viewStack->Freeze();
		// delete file's symbol tree
		WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[vmCurrentFile]);
		viewStack->Delete(editor->GetFileName().GetFullPath());
		// maybe delete project symbol tree
		wxString dummy;
		ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(editor->GetProjectName(), dummy);
		if (proj && editor->GetProjectName() != m_mgr->GetWorkspace()->GetActiveProjectName()) {
			// check if any other file is open in this project before deleting its tree
			std::vector<wxFileName> files;
			proj->GetFiles(files, true);
			WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[vmCurrentFile]);
			bool deleteit = true;
			for (size_t i = 0; i < files.size() && deleteit; i++) {
				deleteit = !viewStack->Find(files[i].GetFullPath());
			}
			if (deleteit) {
				viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[vmCurrentProject]);
				viewStack->Delete(proj->GetFileName().GetFullPath());
			}
		}
		if (m_mgr->GetActiveEditor() != editor) {
			// show tree of active editor
			ShowSymbolTree();
		} else {
			// if there's tree left to show in the current view mode, show it
			viewStack = (WindowStack*) m_viewStack->GetSelected();
			std::vector<wxString> keys;
			viewStack->GetKeys(keys);
			if (!keys.empty()) {
				viewStack->Select(keys[0]);
			}
		}
		m_viewStack->Thaw();
	}
	e.Skip();
}

/**
 * When all editors are closed, remove all file trees and all project trees except for the active project.
 */
void SymbolViewPlugin::OnAllEditorsClosed(wxCommandEvent& e)
{
	if (m_isLinkedToEditor) {
		m_viewStack->Freeze();

		// remove all the file trees
		WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[vmCurrentFile]);
		viewStack->Clear();

		// remove the project trees (except active project)
		viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[vmCurrentProject]);
		wxWindow *save = NULL;
		wxString savePath, dummy;
		ProjectPtr curProj = m_mgr->GetWorkspace()->FindProjectByName(m_mgr->GetWorkspace()->GetActiveProjectName(), dummy);
		if (curProj) {
			savePath = curProj->GetFileName().GetFullPath();
			save = viewStack->Remove(savePath);
		}
		viewStack->Clear();
		if (save) {
			viewStack->Add(save, savePath);
		}

		if (GetViewMode() == vmCurrentProject) {
			ShowSymbolTree();
		}
		m_viewStack->Thaw();
	}
	e.Skip();
}

void SymbolViewPlugin::OnNodeDClick(wxMouseEvent& e)
{
	wxTreeCtrl *tree = dynamic_cast<wxTreeCtrl*>( e.GetEventObject() );
	if ( tree ) {
		if(DoActivateSelection(tree)){
			// avoid the Skip()
			return;
		}
	}
	e.Skip();
}

bool SymbolViewPlugin::DoActivateSelection(wxTreeCtrl* tree)
{
	wxTreeItemId id = tree->GetSelection();
	if (id.IsOk() && tree && tree->GetItemData(id) != NULL) {
		TagTreeData *tag = (TagTreeData*) tree->GetItemData(id);
		if (tag && !tag->GetFile().IsEmpty() && m_mgr->OpenFile(tag->GetFile())) {
			m_mgr->FindAndSelect(tag->GetPattern(), tag->GetName());
			return true;
		}
	}
	return false;
}
