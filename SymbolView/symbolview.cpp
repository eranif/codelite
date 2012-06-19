//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : symbolview.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <set>
#include <wx/app.h>
#include <wx/wupdlock.h>
#include "event_notifier.h"
#include <wx/settings.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include "macros.h"
#include "detachedpanesinfo.h"
#include "workspace.h"
#include "ctags_manager.h"
#include "symbolview.h"
#include "fileextmanager.h"
#include <wx/busyinfo.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>
#include "parse_thread.h"
#include "sv_symbol_tree.h"

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
	info.SetDescription(_("Show Symbols of File, Project, or Workspace"));
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
        , m_tb(NULL)
		, m_viewChoice(NULL)
        , m_splitter(NULL)
		, m_viewStack(NULL)
        , m_choiceSizer(NULL)
		, m_imagesList(NULL)
{
	m_longName = _("Symbols View Plugin");
	m_shortName = wxT("SymbolView");

	LoadImagesAndIndexes();
	CreateGUIControls();
	Connect();
}

SymbolViewPlugin::~SymbolViewPlugin()
{
	thePlugin = NULL;
	m_imagesList->RemoveAll();
	delete m_imagesList;
}


//--------------------------------------------
//GUI setup methods
//--------------------------------------------

void SymbolViewPlugin::LoadImagesAndIndexes()
{
	m_viewModeNames.Add(wxEmptyString, vmMax);
	m_viewModeNames[vmCurrentFile]      = _("Current File");
	m_viewModeNames[vmCurrentProject]   = _("Current Project");
	m_viewModeNames[vmCurrentWorkspace] = _("Current Workspace");

	m_imagesList = new wxImageList(16, 16);

	// ATTN: the order of the images in this list determines the sorting of tree children (along with the child names).
	// That is why we sometimes reload the same image rather than just referring to the prior index.
	BitmapLoader *bmpLoader = m_mgr->GetStdIcons();

	m_image[wxT("workspace")]           = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("workspace/16/workspace")));
	m_image[wxT("project")]             = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("workspace/16/project")));

	m_image[wxT("h")]                   = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("mime/16/h")));
	m_image[wxT("hpp")]                 = m_image[wxT("h")];

	m_image[wxT("c")]                   = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("mime/16/c")));
	m_image[wxT("cpp")]                 = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));
	m_image[wxT("cxx")]                 = m_image[wxT("cpp")];
	m_image[wxT("c++")]                 = m_image[wxT("cpp")];
	m_image[wxT("cc")]                  = m_image[wxT("cpp")];

	m_image[wxT("file")]                = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("mime/16/text")));

	m_image[wxT("class_view")]          = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/globals")));
	m_image[wxT("globals")]             = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/globals")));
	m_image[wxT("namespace")]           = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/namespace")));

	m_image[wxT("macro")]               = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));
	m_image[wxT("macro_protected")]     = m_image[wxT("macro")];
	m_image[wxT("macro_public")]        = m_image[wxT("macro")];
	m_image[wxT("macro_private")]       = m_image[wxT("macro")];

	m_image[wxT("interface")]           = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));
	m_image[wxT("interface_public")]    = m_image[wxT("interface")];
	m_image[wxT("interface_protected")] = m_image[wxT("interface")];
	m_image[wxT("interface_private")]   = m_image[wxT("interface")];

	m_image[wxT("class")]               = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));
	m_image[wxT("class_public")]        = m_image[wxT("class")];
	m_image[wxT("class_protected")]     = m_image[wxT("class")];
	m_image[wxT("class_private")]       = m_image[wxT("class")];

	m_image[wxT("struct")]              = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct")));
	m_image[wxT("struct_public")]       = m_image[wxT("struct")];
	m_image[wxT("struct_protected")]    = m_image[wxT("struct")];
	m_image[wxT("struct_private")]      = m_image[wxT("struct")];

	m_image[wxT("union")]               = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/union")));
	m_image[wxT("union_public")]        = m_image[wxT("union")];
	m_image[wxT("union_protected")]     = m_image[wxT("union")];
	m_image[wxT("union_private")]       = m_image[wxT("union")];

	m_image[wxT("enum")]                = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/enum")));
	m_image[wxT("enum_public")]         = m_image[wxT("enum")];
	m_image[wxT("enum_protected")]      = m_image[wxT("enum")];
	m_image[wxT("enum_private")]        = m_image[wxT("enum")];

	m_image[wxT("typedef")]             = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));
	m_image[wxT("typedef_public")]      = m_image[wxT("typedef")];
	m_image[wxT("typedef_protected")]   = m_image[wxT("typedef")];
	m_image[wxT("typedef_private")]     = m_image[wxT("typedef")];

	m_image[wxT("prototype_public")]    = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));
	m_image[wxT("prototype_protected")] = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected")));
	m_image[wxT("prototype_private")]   = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));
	m_image[wxT("prototype")]           = m_image[wxT("prototype_public")];

	m_image[wxT("function_public")]     = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));
	m_image[wxT("function_protected")]  = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected")));
	m_image[wxT("function_private")]    = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));
	m_image[wxT("function")]            = m_image[wxT("function_public")];

	m_image[wxT("method_public")]       = m_image[wxT("function_public")];
	m_image[wxT("method_protected")]    = m_image[wxT("function_protected")];
	m_image[wxT("method_private")]      = m_image[wxT("function_private")];
	m_image[wxT("method")]              = m_image[wxT("method_public")];

	m_image[wxT("member_public")]       = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));
	m_image[wxT("member_protected")]    = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_protected")));
	m_image[wxT("member_private")]      = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_private")));
	m_image[wxT("member")]              = m_image[wxT("member_public")];
	m_image[wxT("variable")]            = m_image[wxT("member_public")];

	m_image[wxT("enumerator")]          = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/enumerator")));
	m_image[wxT("default")]             = m_imagesList->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct"))); // fallback for anything else
}

void SymbolViewPlugin::CreateGUIControls()
{
	DetachedPanesInfo dpi;
	m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
	wxArrayString detachedPanes = dpi.GetPanes();
	bool isDetached = detachedPanes.Index(wxT("Symbols")) != wxNOT_FOUND;

	Notebook *book = m_mgr->GetWorkspacePaneNotebook();
	if( isDetached ) {
		// Make the window child of the main panel (which is the grand parent of the notebook)
		DockablePane *cp = new DockablePane(book->GetParent()->GetParent(), book, wxT("Symbols"), wxNullBitmap, wxSize(200, 200));
		m_symView = new wxPanel(cp);
		cp->SetChildNoReparent(m_symView);

	} else {
		m_symView = new wxPanel(book);
		book->AddPage(m_symView, _("Symbols"), false);
	}

	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	m_symView->SetSizer(sz);

	m_tb = new wxToolBar(m_symView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER);
	m_tb->AddTool(XRCID("link_editor"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("link_editor")), _("Link Editor"), wxITEM_CHECK);
	m_tb->ToggleTool(XRCID("link_editor"), true);
	m_tb->AddTool(XRCID("collapse_all"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("collapse")), _("Collapse All"), wxITEM_NORMAL);
	m_tb->AddTool(XRCID("gohome"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("gohome")), _("Go to Active Editor Symbols"), wxITEM_NORMAL);

	// sizer for the drop button and view-mode choice box
	m_choiceSizer = new wxBoxSizer(wxHORIZONTAL);
	sz->Add(m_choiceSizer, 0, wxEXPAND|wxALL, 1);
    
#if wxCHECK_VERSION(2,9,1)
	m_viewChoice = new wxChoice(m_tb, wxID_ANY);
#else
    m_viewChoice = new wxChoice(m_symView, wxID_ANY);
#endif
	m_viewChoice->AppendString(m_viewModeNames[vmCurrentFile]);
	m_viewChoice->Select(0);
    
#if wxCHECK_VERSION(2,9,1)
	m_tb->AddStretchableSpace();
    m_tb->AddControl(m_viewChoice, wxEmptyString);
#else
    m_choiceSizer->Add(m_viewChoice, 1, wxEXPAND|wxALL, 1);
#endif

	//m_choiceSizer->Add(m_viewChoice, 1, wxEXPAND|wxALL, 1);

    m_splitter = new wxSplitterWindow(m_symView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER|wxSP_3DSASH);
    m_splitter->SetMinimumPaneSize(20);
    sz->Add(m_splitter, 1, wxEXPAND|wxALL, 1);

	m_tb->Realize();
	sz->Add(m_tb, 0, wxEXPAND);
    
	m_viewStack = new WindowStack(m_splitter);
	for (int i = 0; i < vmMax; i++) {
		m_viewStack->Add(new WindowStack(m_viewStack), m_viewModeNames[i]);
	}
	m_viewStack->Select(m_viewModeNames[vmCurrentFile]);
    m_splitter->Initialize(m_viewStack);

	m_stackChoice = new StackButton(m_symView, (WindowStack*) m_viewStack->GetSelected());
	m_choiceSizer->Add(m_stackChoice, 0, wxEXPAND|wxALL, 1);
	// by default the drop-down button is hidden
	m_choiceSizer->Hide(m_stackChoice);

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
	EventNotifier::Get()->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceLoaded), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileAdded), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileRemoved), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_PROJ_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectAdded), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_PROJ_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectRemoved), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_FILE_RETAGGED, wxCommandEventHandler(SymbolViewPlugin::OnFileRetagged), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewPlugin::OnActiveEditorChanged), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewPlugin::OnEditorClosed), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED, wxCommandEventHandler(SymbolViewPlugin::OnAllEditorsClosed), NULL, this);
	EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceClosed), NULL, this);
    topwin->Connect(XRCID("show_tag_in_symview"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnShowTagInSymView), NULL, this);
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

clToolBar *SymbolViewPlugin::CreateToolBar(wxWindow *parent)
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

	EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SymbolViewPlugin::OnWorkspaceLoaded), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileAdded), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectFileRemoved), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_PROJ_REMOVED, wxCommandEventHandler(SymbolViewPlugin::OnProjectAdded), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_PROJ_ADDED, wxCommandEventHandler(SymbolViewPlugin::OnProjectRemoved), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_FILE_RETAGGED, wxCommandEventHandler(SymbolViewPlugin::OnFileRetagged), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(SymbolViewPlugin::OnActiveEditorChanged), NULL, this);
	EventNotifier::Get()->Disconnect(wxEVT_EDITOR_CLOSING, wxCommandEventHandler(SymbolViewPlugin::OnEditorClosed), NULL, this);
    topwin->Disconnect(XRCID("show_tag_in_symview"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SymbolViewPlugin::OnShowTagInSymView), NULL, this);

	Notebook *notebook = m_mgr->GetWorkspacePaneNotebook();
	size_t notepos = notebook->GetPageIndex(m_symView);
	if (notepos != Notebook::npos) {
		notebook->RemovePage(notepos, false);
	}

	m_symView->Destroy();
	m_symView = NULL;
}

//--------------------------------------------
//Helper methods
//--------------------------------------------

/**
 * Determine a symbol tree path from a source file name and project, using the current view mode.
 */
wxString SymbolViewPlugin::GetSymbolsPath(const wxString &fileName, const wxString &projname)
{
    wxString projectName = projname;
    if (projectName.IsEmpty()) {
        projectName = m_mgr->GetProjectNameByFile(fileName);
    }
	switch (GetViewMode()) {
	case vmCurrentWorkspace:
		if (m_mgr->IsWorkspaceOpen()) { // ignore input, return workspace file path
			return m_mgr->GetWorkspace()->GetWorkspaceFileName().GetFullPath();
		}
		break;
	case vmCurrentProject: {
        if (projectName.IsEmpty()) {
            projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
        }
		wxString dummy;
		ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectName, dummy);
		if (project) {
			return project->GetFileName().GetFullPath();
		}
		break;
	}
	default:
		if (!projectName.IsEmpty()) { // file must be part of a project
			return fileName;
		}
		break;
	}
	return wxEmptyString;

}

/**
 * Determine a symbol tree path from the current editor, using the current view mode.
 */
wxString SymbolViewPlugin::GetSymbolsPath(IEditor* editor)
{
    wxString fname;
    wxString project;
    if (editor) {
        fname = editor->GetFileName().GetFullPath();
        project = editor->GetProjectName();
    }
    return GetSymbolsPath(fname, project);
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

	if(GetViewMode() == vmCurrentWorkspace) {
		wxArrayString projectNames;
		wxString      dummy;
		m_mgr->GetWorkspace()->GetProjectList(projectNames);
		for (size_t i = 0; i < projectNames.Count(); i++) {
			ProjectPtr project = m_mgr->GetWorkspace()->FindProjectByName(projectNames.Item(i), dummy);
			if (!project)
				continue;

			std::vector<wxFileName> projectFiles;
			project->GetFiles(projectFiles, true);
			for (size_t j = 0; j < projectFiles.size(); j++) {
				files.Add(projectFiles.at(j).GetFullPath());
			}
		}
	} else {
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
				
				FileExtManager::FileType pathType = FileExtManager::GetType(path.GetFullName());
				FileExtManager::FileType fileType = FileExtManager::GetType(fileName.GetFullName());
				
				if (fullPath == workspaceFileName || fullPath == projectFileName || fullPath == file) {
					files.Add(file);

				} else if ((pathType == FileExtManager::TypeSourceC || pathType == FileExtManager::TypeSourceCpp) && fileType == FileExtManager::TypeHeader) {
					// TODO: replace this code with a "real" solution based on actual file dependencies.
					// for now, make sure .c or .cpp file also includes corresponding .h file.
					wxString otherFile;
					if(FindSwappedFile(fileName, otherFile, projectFiles)) {
						if (fullPath.CmpNoCase(otherFile) == 0) {
							files.Add(file);
						}
					}
				}
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
			
			FileExtManager::FileType fileNameType = FileExtManager::GetType(fileName.GetFullName());
			if (fileNameType == FileExtManager::TypeSourceC || fileNameType == FileExtManager::TypeSourceCpp) {
				wxString headerFile;
				if(FindSwappedFile(fileName, headerFile, projectFiles)) {
					if (fileset.find(headerFile) == fileset.end()) {
						filePaths.insert(std::make_pair(headerFile, filePath));
					}
				}
			}
		}
	}
}

//--------------------------------------------
//Tree-related methods
//--------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(SymbolViewPlugin::SymTree, wxTreeCtrl)

int SymbolViewPlugin::SymTree::IsCtorOrDtor(const wxTreeItemId &id)
{
	if (!id.IsOk() || GetRootItem() == id)
		return 0;
	TagTreeData *childtag = (TagTreeData*) GetItemData(id);
	if (!childtag || ((childtag->GetKind() != wxT("function")) && (childtag->GetKind() != wxT("prototype"))))
		return 0;

	wxTreeItemId parent = GetItemParent(id);
	if (!parent.IsOk())
		return 0;
	TagTreeData *parenttag = (TagTreeData*) GetItemData(parent);
	if (!parenttag || ((parenttag->GetKind() != wxT("class")) && (parenttag->GetKind() != wxT("struct"))))
		return 0;

	wxString name = childtag->GetName();
	name.StartsWith(wxT("~"), &name);

	return name == parenttag->GetName() ? 1 : 0;
}

/**
 * Compares two nodes.  Used by wxTreeCtrl::SortChildren()
 */
int SymbolViewPlugin::SymTree::OnCompareItems(const wxTreeItemId &id1, const wxTreeItemId &id2)
{
	int cmp = 0;
	cmp = IsCtorOrDtor(id2) - IsCtorOrDtor(id1);
	if (cmp)
		return cmp;
	cmp = GetItemImage(id1) - GetItemImage(id2);
	if (cmp)
		return cmp;
	return wxTreeCtrl::OnCompareItems(id1, id2);
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
	tree->SetItemText(id, treetag->GetDisplayName());

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
	tree->SetItemHasChildren(id, treetag->IsContainer() || treetag->GetKind() == wxT("enum"));

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
 * Dynamically add the children of the specified tree node by querying the workspace database.
 */
int SymbolViewPlugin::LoadChildren(SymTree *tree, wxTreeItemId id)
{
	int count = 0;

	wxString activeFileName;
	if(m_mgr->GetActiveEditor()) {
		activeFileName = m_mgr->GetActiveEditor()->GetFileName().GetFullPath();
	}

	// root node gets special grouping children
	if (id == tree->GetRootItem()) {
		tree->m_globals = tree->AppendItem(id, _("Global Functions and Variables"), m_image[wxT("globals")]);
		tree->m_protos  = tree->AppendItem(id, _("Functions Prototypes"),           m_image[wxT("globals")]);
		tree->m_macros  = tree->AppendItem(id, _("Macros"),                         m_image[wxT("globals")]);
	} else {
		tree->SetItemHasChildren(id, false);
	}

	// get files to scan for tags
	wxArrayString files;

	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	GetFiles(viewStack->Find(tree), files);


	// get scope and kind to scan for tags (also: enumerators indicate their parent enum via "typeref" in the db)
	TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
	ITagsStoragePtr db = m_mgr->GetTagsManager()->GetDatabase();
	std::vector<TagEntryPtr> tags;
	if (!treetag) {
		db->GetTagsByFilesAndScope(files, wxT("<global>"), tags);

	} else if (treetag->GetKind() != wxT("enum")) {
		wxArrayString kinds;
		kinds.Add(wxT("member"));
		kinds.Add(wxT("function"));
		kinds.Add(wxT("prototype"));
		kinds.Add(wxT("typedef"));
		kinds.Add(wxT("enumerator"));
		kinds.Add(wxT("class"));
		kinds.Add(wxT("struct"));
		kinds.Add(wxT("union"));
		kinds.Add(wxT("namespace"));
		db->GetTagsByFilesKindAndScope(files, kinds, treetag->GetPath(), tags);
//		sqlopts.insert(std::make_pair(wxString(wxT("scope")), treetag->GetPath()));
//		sqlopts.insert(std::make_pair(wxString(wxT("!kind")), wxString(wxT("macro"))));
//		sqlopts.insert(std::make_pair(wxString(wxT("!kind")), wxString(wxT("variable"))));
	} else {
		wxArrayString kinds;
		kinds.Add(wxT("enumerator"));
		db->GetTagsByFilesScopeTyperefAndKind(files, kinds, treetag->GetScope(), treetag->GetPath(), tags);
//		sqlopts.insert(std::make_pair(wxString(wxT("scope")), treetag->GetScope()));
//		sqlopts.insert(std::make_pair(wxString(wxT("typeref")), treetag->GetPath()));
//		sqlopts.insert(std::make_pair(wxString(wxT("kind")), wxString(wxT("enumerator"))));
	}

	// query database for the tags that go under this node
	for(size_t i=0; i<tags.size(); i++ ) {
		TagEntryPtr tag = tags.at(i);
		if (tag->GetKind() == wxT("enumerator") && !tag->GetTyperef().IsEmpty() && (!treetag || treetag->GetKind() != wxT("enum")))
			// typed enumerators go under their enum instead of here
			continue;
		wxTreeItemId parent = id != tree->GetRootItem() ? id : GetParentForGlobalTag(tree, *tag);
		// create child node, add our custom tag data to it, and set its appearance accordingly

		// If the view mode is "Current File" but the tagEntry is NOT from
		// the current tree, AND the current item is not a "parent" node
		// dont show it
		bool disableItem = (GetViewMode() == vmCurrentFile   &&
							activeFileName != tag->GetFile() &&
							!(tag->IsContainer() || tag->GetKind() == wxT("enum")) );
		if(!disableItem) {

			wxTreeItemId child = tree->AppendItem(parent, wxEmptyString);
			SetNodeData(tree, child, *tag);
			count++;

		}
	}

	SortChildren();
	return count;
}

/**
 * Add new symbol to all trees that would currently be showing it.
 */
int SymbolViewPlugin::AddSymbol(const TagEntry &tag, const std::multimap<wxString, wxString> &filePaths)
{
	int count = 0;
	wxString tagScope = tag.GetScope();
	if (tag.GetKind() == wxT("enumerator") && !tag.GetTyperef().IsEmpty()) {
		tagScope = tag.GetTyperef();
	}
	if (tagScope != wxT("<global>")) {
		// tag is a scoped symbol (eg "Foo::Foo") so look for matching scope nodes to add it to as a child
		int oldcount = count;
		for (Path2TagRange range = m_pathTags.equal_range(tagScope); range.first != range.second; range.first++) {
			wxTreeCtrl *tree = range.first->second.first;
			wxTreeItemId parent = range.first->second.second;
			if (!tree->IsExpanded(parent) && tree->GetChildrenCount(parent) == 0)
				continue; // don't add symbols to unexpanded node or it will fool LoadChildren()
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
				range.second = m_pathTags.upper_bound(tagScope); // recalculate invalidated upper-bound
				count++;
			}
		}
		if (oldcount == count) {
			// no scope in which to place this tag!  probably the parent hasn't been added yet (there's no
			// guarantee on the order tags come back from the db). queue the tag to be handled later.
			m_deferredTags.push(tag);
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
 * Try to add symbols that were deferred because their parent (scope) symbols hadn't been read yet.
 * Keeps trying to add symbols till no more can be added.
 */
void SymbolViewPlugin::AddDeferredSymbols(const std::multimap<wxString, wxString> &filePaths)
{
	size_t oldsize = size_t(-1);
	while (m_deferredTags.size() < oldsize) {
		oldsize = m_deferredTags.size();
		for (size_t n = oldsize; n > 0; n--) {
			TagEntry tag = m_deferredTags.front();
			m_deferredTags.pop();
			AddSymbol(tag, filePaths);
		}
	}
	while (!m_deferredTags.empty()) {
		m_deferredTags.pop();
	}
}

/**
 * Given a set of files that have been recently (re)tagged, query the workspace db for symbols in those files,
 * and update all symbol trees to reflect changes.
 */
void SymbolViewPlugin::UpdateTrees(const wxArrayString &files, bool removeOld)
{
	m_mgr->SetStatusMessage(_("Updating SymbolView tree..."), 0);

	std::multimap<wxString,wxString> sqlopts;
	std::map<TagKey, TreeNode> tagsToDelete;

	// collect files that have been retagged, and all tags in current trees that came from these files (if removing old)
	wxArrayString filesArr;
	for (size_t i = 0; i < files.Count(); i++) {
		filesArr.Add(files.Item(i));

		if (removeOld) {
			for (File2TagRange range = m_fileTags.equal_range(files[i]); range.first != range.second; range.first++) {
				wxTreeCtrl *tree = range.first->second.first;
				wxTreeItemId id = range.first->second.second;
				TagTreeData *tag = (TagTreeData*) tree->GetItemData(id);
				tagsToDelete[TagKey(tag->GetFile(), tag->Key())] = TreeNode(tree,id);
			}
		}
	}

	// for the set of files retagged, find paths of all symbol trees showing tags from those files
	std::multimap<wxString,wxString> treePaths;
	GetPaths(files, treePaths);
	std::vector<TagEntryPtr> tags;
	m_mgr->GetTagsManager()->GetDatabase()->GetTagsByFiles(files, tags);

	// query database for current tags of retagged files.
	// update or add new symbols and remove these from the to-delete list
	for(size_t i=0; i<tags.size(); i++) {

		if (removeOld && UpdateSymbol(*tags.at(i))) {
			tagsToDelete.erase(TagKey(tags.at(i)->GetFile(), tags.at(i)->Key()));
		} else {
			AddSymbol(*tags.at(i), treePaths);
		}
	}

	AddDeferredSymbols(treePaths);
	SortChildren();

	if (removeOld) {
		// remove from trees all tags no longer in the database
		for (std::map<TagKey,TreeNode>::iterator i = tagsToDelete.begin(); i != tagsToDelete.end(); i++) {
			wxTreeCtrl *tree = i->second.first;
			wxTreeItemId id = i->second.second;
			if (id.IsOk()) {
				tree->Delete(id);
			}
		}
	}
}

/**
 * Initialize a brand new symbol tree for the specified path.
 */
void SymbolViewPlugin::CreateSymbolTree(const wxString &path, WindowStack *parent)
{
	if (path.IsEmpty() || !parent)
		return;

	m_mgr->SetStatusMessage(_("Building SymbolView tree..."), 0);
    
    if (!path.EndsWith(wxT(".project")) && !path.EndsWith(wxT(".workspace")))
    {
        //build the outline view
        svSymbolTree* m_treeOutline = new svSymbolTree( parent, m_mgr, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxNO_BORDER);
        m_treeOutline->SetSymbolsImages(svSymbolTree::CreateSymbolTreeImages());
        m_treeOutline->BuildTree(path);
        m_treeOutline->ExpandAll();
        parent->Add(m_treeOutline, path);
    }
    else
    {
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
        tree->Connect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(SymbolViewPlugin::OnNodeKeyDown), NULL, this);
        tree->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(SymbolViewPlugin::OnNodeSelected), NULL, this);
        tree->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(SymbolViewPlugin::OnNodeDClick), NULL, this);
    }
    m_mgr->SetStatusMessage(wxEmptyString, 0);
}

/**
 * Select the symbol tree in the specified viewStack (use current if NULL).  Create it if necessary.
 */
void SymbolViewPlugin::ShowSymbolTree(const wxString &symtreepath)
{
	wxString path = !symtreepath.IsEmpty() ? symtreepath : GetSymbolsPath(m_mgr->GetActiveEditor());
	if (path.IsEmpty())
        return;
    WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
    if (viewStack->GetSelectedKey() != path) {
        m_viewStack->Freeze();
        if (!viewStack->Find(path)) {
            CreateSymbolTree(path, viewStack);
        }
        viewStack->Select(path);
        m_viewStack->Thaw();
    }
    if (m_tb->GetToolState(XRCID("link_editor")) && viewStack->GetSelectedKey() != GetSymbolsPath(m_mgr->GetActiveEditor())) {
        // turn off link-editor, or the view will switch back as soon as the editor regains focus
        m_tb->ToggleTool(XRCID("link_editor"), false);
        wxCommandEvent dummy;
        OnLinkEditor(dummy);
    }
}

bool SymbolViewPlugin::DoActivateSelection(wxTreeCtrl* tree)
{
	if (!tree)
		return false;

	wxTreeItemId id = tree->GetSelection();
	if (!id.IsOk())
		return false;

	TagTreeData *tag = (TagTreeData*) tree->GetItemData(id);
	if (!tag)
		return false;

	bool     linkEditor    = m_tb->GetToolState(XRCID("link_editor"));
	IEditor *currentEditor = m_mgr->GetActiveEditor();

	if(linkEditor) {
		// When link editor is enabled, dont allow opening items from the SymbolView tree which dont belong
		// to the current file
		if(GetViewMode() == vmCurrentFile && currentEditor && tag->GetFile() != currentEditor->GetFileName().GetFullPath())
			return false;
	}

	if (tag->GetFile().IsEmpty() || !m_mgr->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine()-1))
		return false;

	m_mgr->FindAndSelect(tag->GetPattern(), tag->GetName());
	return true;
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
	if (viewStack->GetSelected() == NULL || m_tb->GetToolState(XRCID("link_editor"))) {
		ShowSymbolTree();
	}
	e.Skip();
}

void SymbolViewPlugin::OnStackChoiceUI(wxUpdateUIEvent& e)
{
	WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
	e.Enable(!m_tb->GetToolState(XRCID("link_editor")) && viewStack->GetSelected() != NULL);
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
	e.Enable(!m_tb->GetToolState(XRCID("link_editor")) && viewStack->GetSelected() != NULL);
}

/**
 * Toggle the link-to-editor state.  If now linked, get rid of extraneous symbol trees.
 */
void SymbolViewPlugin::OnLinkEditor(wxCommandEvent& e)
{
	if (m_tb->GetToolState(XRCID("link_editor"))) {
		// hide the dropbutton
		m_choiceSizer->Hide(m_stackChoice);
		m_choiceSizer->Layout();
		ShowSymbolTree();
	} else {
		m_choiceSizer->Show(m_stackChoice);
		m_choiceSizer->Layout();
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
void SymbolViewPlugin::OnNodeKeyDown(wxTreeEvent& e)
{
	wxTreeCtrl *tree = dynamic_cast<wxTreeCtrl*>(e.GetEventObject());
	switch (e.GetKeyCode()) {
	case WXK_RETURN:
		if (DoActivateSelection(tree))
			return; // no e.Skip() or the node will expand too
		break;
	}
	e.Skip();
}

void SymbolViewPlugin::OnNodeSelected(wxTreeEvent &e)
{
    e.Skip();
}

/**
 * Open the chosen symbol's file, and select the text where it is defined.
 */
void SymbolViewPlugin::OnNodeDClick(wxMouseEvent& e)
{
	wxTreeCtrl *tree = dynamic_cast<wxTreeCtrl*>( e.GetEventObject() );
	if (!DoActivateSelection(tree)) {
		e.Skip(); // only if activation fails, or node will expand too
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
	if(m_mgr->IsWorkspaceOpen()) {
		if (GetViewMode() == vmCurrentWorkspace) {
			ShowSymbolTree();
		}
	}
	e.Skip();
}

/**
 * When workspace is closed, clear everything.
 */
void SymbolViewPlugin::OnWorkspaceClosed(wxCommandEvent& e)
{
	DoClearSymbolView();
	e.Skip();
}

void SymbolViewPlugin::DoClearSymbolView()
{
	for (size_t i = 0; i < m_viewModeNames.Count(); i++) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->Find(m_viewModeNames[i]);
		if (viewStack) {
			viewStack->Clear();
		}
	}

	// set the view mode to current file, this to avoid long waiting on opening next workspace
	m_viewStack->Select(m_viewModeNames[vmCurrentFile]);
	m_viewChoice->SetStringSelection(m_viewModeNames[vmCurrentFile]);
}

/**
 * Respond to files retagged by updating all trees: add, modify, or delete tags as necessary.
 * @note This function is also called when a new project is added to the workspace.
 */
void SymbolViewPlugin::OnFileRetagged(wxCommandEvent& e)
{
	// Clear the plugin content

	std::vector<wxFileName> *files = (std::vector<wxFileName>*) e.GetClientData();
	if (files && !files->empty()) {

	/*The commented-out code below is because the original caused a crash in the 'else' section:
	see https://sourceforge.net/tracker/index.php?func=detail&aid=3535567&group_id=202033&atid=979960
	As I don't understand this under-commented code, I've just hacked it away. This solves the crash,
	without any obvious side-effect (perhaps the 'else' was a premature optimisation...) DH */
//		if(files->size() > 1) {
			DoClearSymbolView();

			// Tag only visible files
			if(m_mgr->GetActiveEditor()) {
				wxArrayString filePaths;
				filePaths.Add(m_mgr->GetActiveEditor()->GetFileName().GetFullPath());

				clWindowUpdateLocker locker(m_viewStack);
				UpdateTrees(filePaths, false);
			}
			
			ShowSymbolTree();	// Make the tree visible again: this didn't happen previously...
/*		} else {

			wxArrayString filePaths;
			for (size_t i = 0; i < files->size(); i++) {
				filePaths.Add(files->at(i).GetFullPath());
			}
			clWindowUpdateLocker locker(m_viewStack);
			UpdateTrees(filePaths, true);

		}*/
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
		clWindowUpdateLocker locker(m_viewStack);
		UpdateTrees(*files, false);
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
		wxWindowDisabler disableAll;
		for (size_t i = 0; i < files->Count(); i++) {
			DeleteFileSymbols(files->Item(i));
		}
		SortChildren();
		// if the tree that was being displayed got deleted, try to show another
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (viewStack->GetSelected() == NULL) {
			ShowSymbolTree();
		}
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
//void SymbolViewPlugin::OnSymbolsAdded(wxCommandEvent& e)
//{
//	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
//	if (data && !data->GetItems().empty()) {
//		m_viewStack->Freeze();
//
//		wxArrayString files;
//		std::multimap<wxString, wxString> filePaths;
//
//		files.Add(data->GetFileName());
//		GetPaths(files, filePaths);
//
//		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
//		for (size_t i = 0; i < tags.size(); i++) {
//			AddSymbol(tags[i].second, filePaths);
//		}
//		AddDeferredSymbols(filePaths);
//		SortChildren();
//
//		m_viewStack->Thaw();
//	}
//	e.Skip();
//}
//
///**
// * Respond to updated symbols event (from ParseThread) by fixing up our maps and trees.
// */
//void SymbolViewPlugin::OnSymbolsUpdated(wxCommandEvent& e)
//{
//	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
//	if (data && !data->GetItems().empty()) {
//		m_viewStack->Freeze();
//
//		wxArrayString files;
//		std::multimap<wxString, wxString> filePaths;
//
//		files.Add(data->GetFileName());
//		GetPaths(files, filePaths);
//
//		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
//		for (size_t i = 0; i < tags.size(); i++) {
//			if (tags[i].second.GetKind() != wxT("enumerator")) {
//				UpdateSymbol(tags[i].second);
//			} else {
//				// must remove and re-add enumerators in case tree location changed
//				// due to possible change in typeref
//				DeleteSymbol(tags[i].second);
//				AddSymbol(tags[i].second, filePaths);
//			}
//		}
//		AddDeferredSymbols(filePaths);
//		SortChildren();
//
//		m_viewStack->Thaw();
//	}
//	e.Skip();
//}
//
///**
// * Respond to deleted symbols event (from ParseThread) by fixing up our maps and trees.
// */
//void SymbolViewPlugin::OnSymbolsDeleted(wxCommandEvent& e)
//{
//	ParseThreadEventData *data = (ParseThreadEventData*) e.GetClientData();
//	if (data && !data->GetItems().empty()) {
//		m_viewStack->Freeze();
//		const std::vector<std::pair<wxString,TagEntry> > &tags = data->GetItems();
//		for (size_t i = 0; i < tags.size(); i++) {
//			DeleteSymbol(tags[i].second);
//		}
//		// if the tree that was being displayed got deleted, try to show another
//		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
//		if (viewStack->GetSelected() == NULL) {
//			ShowSymbolTree();
//		}
//		m_viewStack->Thaw();
//	}
//	e.Skip();
//}

/**
 * Respond to active editor change by, if allowed, showing the appropriate symbol tree.
 */
void SymbolViewPlugin::OnActiveEditorChanged(wxCommandEvent& e)
{
	if (m_mgr->IsWorkspaceOpen()) {
		WindowStack *viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (viewStack->GetSelected() == NULL || m_tb->GetToolState(XRCID("link_editor"))) {
			ShowSymbolTree();
		}
	}
	e.Skip();
}

/**
 * Remove the editor's symbol tree.  If no other editors viewing the same project, remove the project symbol tree.
 */
void SymbolViewPlugin::OnEditorClosed(wxCommandEvent& e)
{
	IEditor *editor = (IEditor*) e.GetClientData();
	if (editor && !editor->GetProjectName().IsEmpty() && m_tb->GetToolState(XRCID("link_editor")) && m_mgr->IsWorkspaceOpen()) {
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
		viewStack = (WindowStack*) m_viewStack->GetSelected();
		if (m_mgr->GetActiveEditor() != editor) {
			// show tree of active editor
			ShowSymbolTree();
		} else if (!viewStack->GetSelected() && viewStack->Count() > 0) {
			// if there's tree left to show in the current view mode, show it
			std::vector<wxString> keys;
			viewStack->GetKeys(keys);
			viewStack->Select(keys[0]);
		}
	}
	e.Skip();
}

/**
 * When all editors are closed, remove all file trees and all project trees except for the active project.
 */
void SymbolViewPlugin::OnAllEditorsClosed(wxCommandEvent& e)
{
	if (m_tb->GetToolState(XRCID("link_editor")) && m_mgr->IsWorkspaceOpen()) {

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
	}
	e.Skip();
}

/**
 * Find the tag under the caret in active editor, show the tree that tag belongs in, and highlight the tag.
 * Some trickery is required to make sure the tag is loaded in the tree since trees are normally built on
 * demand (when the user expands each node).
 */
void SymbolViewPlugin::OnShowTagInSymView(wxCommandEvent& e)
{
    wxUnusedVar(e);

    int pos = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_symView);
    if (pos != wxNOT_FOUND) {
        m_mgr->GetWorkspacePaneNotebook()->SetSelection(pos);
    }

    // first get the right symbol tree to show
    TagEntryPtr tag = m_mgr->GetTagAtCaret(true, false);
    if (!tag)
        return;
    wxString path = GetSymbolsPath(tag->GetFile());
    if (path.IsEmpty())
        return;
    ShowSymbolTree(path);

    // now expand the path to the tag in the tree
    SymTree *tree = FindSymbolTree(path);
	wxString tagScope = tag->GetScope();
	if (tag->GetKind() == wxT("enumerator") && !tag->GetTyperef().IsEmpty()) {
		tagScope = tag->GetTyperef();
	}
	if (tagScope != wxT("<global>")) {
        // filePaths will be used to make sure we expand the correct scope tags
		wxArrayString files;
		std::multimap<wxString, wxString> filePaths;
		files.Add(tag->GetFile());
		GetPaths(files, filePaths);
        // we don't know much about the scope other than the name and file it should be in, so we have
        // to brute-force this a little: expand any node that *could* be a parent of our tag. chances
        // are pretty good there won't be much redundancy
        wxArrayString scopes = wxStringTokenize(tagScope, wxT(":"), wxTOKEN_STRTOK);
        wxString curScope;
        for (size_t i = 0; i < scopes.Count(); i++) {
            curScope << (i > 0 ? wxT("::") : wxT("")) << scopes[i];
            for (Path2TagRange range = m_pathTags.equal_range(curScope); range.first != range.second; range.first++) {
                if (tree != range.first->second.first)
                    continue;
                wxTreeItemId id = range.first->second.second;
                TagTreeData *treetag = (TagTreeData*) tree->GetItemData(id);
                // make sure the alleged parent is from the correct file
                std::pair<std::multimap<wxString,wxString>::const_iterator,
                          std::multimap<wxString,wxString>::const_iterator> files = filePaths.equal_range(treetag->GetFile());
                while (files.first != files.second && files.first->second != tag->GetFile()) {
                    files.first++;
                }
                if (files.first == files.second)
                    continue;
                if (tree->ItemHasChildren(id) && tree->GetChildrenCount(id) == 0) {
                    LoadChildren(tree, id);
                    range.second = m_pathTags.upper_bound(curScope); // recalculate invalidated upper bound
                }
            }
        }
    }
    // all parents loaded and expanded, so find the tag itself
    for (Path2TagRange range = m_pathTags.equal_range(tag->Key()); range.first != range.second; range.first++) {
        if (range.first->second.first != tree)
            continue;
        TagTreeData *treetag = (TagTreeData*) tree->GetItemData(range.first->second.second);
        if (!treetag)
            continue;
        if (*treetag == *tag || treetag->GetDifferOnByLineNumber()) {
            wxTreeItemId id = range.first->second.second;
            tree->SelectItem(id);
            tree->EnsureVisible(id);
            if (tree->ItemHasChildren(id)) {
                tree->Expand(id);
            }
            break;
        }
    }
}

bool SymbolViewPlugin::FindSwappedFile(const wxFileName& rhs, wxString& lhs, const std::vector<wxFileName> &workspaceFiles)
{
	wxFileName otherFile(rhs);
	wxString ext = rhs.GetExt();
	wxArrayString exts;

	//replace the file extension
	int fileType = FileExtManager::GetType(rhs.GetFullName());
	switch(fileType) {
	case FileExtManager::TypeSourceC:
	case FileExtManager::TypeSourceCpp:
		//try to find a header file
		exts.Add(wxT("h"));
		exts.Add(wxT("hpp"));
		exts.Add(wxT("hxx"));
		break;

	case FileExtManager::TypeHeader:
		exts.Add(wxT("cpp"));
		exts.Add(wxT("cxx"));
		exts.Add(wxT("cc"));
		exts.Add(wxT("c"));
		break;
	default:
		return false;
	}

	wxArrayString           projects;
	wxString                errMsg;

	for (size_t j=0; j<exts.GetCount(); j++) {
		otherFile.SetExt(exts.Item(j));
		if (otherFile.FileExists()) {
			//we got a match
			lhs = otherFile.GetFullPath();
			return true;
		}

		for (size_t i=0; i<workspaceFiles.size(); i++) {
			if (workspaceFiles.at(i).GetFullName() == otherFile.GetFullName()) {
				lhs = workspaceFiles.at(i).GetFullPath();
				return true;
			}
		}
	}
	return false;
}

