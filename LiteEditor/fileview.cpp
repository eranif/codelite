//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileview.cpp
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
#include "fileview.h"
#include "frame.h"
#include "nameanddescdlg.h"
#include "globals.h"
#include "importfilesdlg.h"
#include "manager.h"
#include "tree.h"
#include <wx/xrc/xmlres.h>
#include "wx/imaglist.h"
#include <wx/textdlg.h>
#include <deque>
#include "new_item_dlg.h"
#include "project_settings_dlg.h"
#include "buildmanager.h"
#include "macros.h"
#include "pluginmanager.h"
#include "dirtraverser.h"
#include "ctags_manager.h"
#include <wx/progdlg.h>

IMPLEMENT_DYNAMIC_CLASS(FileViewTree, wxTreeCtrl)

BEGIN_EVENT_TABLE( FileViewTree, wxTreeCtrl )
	EVT_TREE_BEGIN_DRAG( wxID_ANY, FileViewTree::OnItemBeginDrag )
	EVT_TREE_END_DRAG( wxID_ANY, FileViewTree::OnItemEndDrag )
END_EVENT_TABLE()
FileViewTree::FileViewTree()
{
}

void FileViewTree::ConnectEvents()
{
	Connect( XRCID( "remove_project" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRemoveProject ), NULL, this );
	Connect( XRCID( "set_as_active" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnSetActive ), NULL, this );
	Connect( XRCID( "new_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnNewItem ), NULL, this );
	Connect( XRCID( "add_existing_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnAddExistingItem ), NULL, this );
	Connect( XRCID( "new_virtual_folder" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnNewVirtualFolder ), NULL, this );
	Connect( XRCID( "remove_virtual_folder" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRemoveVirtualFolder ), NULL, this );
	Connect( XRCID( "project_properties" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnProjectProperties ), NULL, this );
	Connect( XRCID( "sort_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnSortItem ), NULL, this );
	Connect( XRCID( "remove_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRemoveItem ), NULL, this );
	Connect( XRCID( "export_makefile" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnExportMakefile ), NULL, this );
	Connect( XRCID( "save_as_template" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnSaveAsTemplate ), NULL, this );
	Connect( XRCID( "build_order" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnBuildOrder ), NULL, this );
	Connect( XRCID( "clean_project" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnClean ), NULL, this );
	Connect( XRCID( "build_project" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnBuild ), NULL, this );
	Connect( XRCID( "rebuild_project" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnReBuild ), NULL, this );
	Connect( XRCID( "generate_makefile" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRunPremakeStep ), NULL, this );
	Connect( XRCID( "stop_build" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnStopBuild ), NULL, this );
	Connect( XRCID( "retag_project" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRetagProject ), NULL, this );
	Connect( XRCID( "retag_workspace" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRetagWorkspace ), NULL, this );
	Connect( XRCID( "build_project_only" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnBuildProjectOnly ), NULL, this );
	Connect( XRCID( "clean_project_only" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnCleanProjectOnly ), NULL, this );
	Connect( XRCID( "import_directory" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnImportDirectory ), NULL, this );
	Connect( XRCID( "compile_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnCompileItem ), NULL, this );
	Connect( XRCID( "rename_item" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRenameItem ), NULL, this );
	Connect( XRCID( "rename_virtual_folder" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FileViewTree::OnRenameVirtualFolder ), NULL, this );

	Connect( XRCID( "remove_project" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "set_as_active" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "new_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "add_existing_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "new_virtual_folder" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "remove_virtual_folder" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "project_properties" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "sort_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "remove_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "export_makefile" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "save_as_template" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "build_order" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "clean_project" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "build_project" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "rebuild_project" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "retag_project" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "retag_workspace" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "build_project_only" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "clean_project_only" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "import_directory" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "compile_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "rename_item" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
	Connect( XRCID( "generate_makefile" ), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FileViewTree::OnBuildInProgress ), NULL, this );
}

void FileViewTree::OnBuildInProgress( wxUpdateUIEvent &event )
{
	event.Enable( !ManagerST::Get()->IsBuildInProgress() );
}

FileViewTree::FileViewTree( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create( parent, id, pos, size, style );


//	SetBackgroundColour(*wxBLACK);
//	SetForegroundColour(wxT("GREY"));

	// Initialise images map
	wxImageList *images = new wxImageList( 16, 16, true );
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "project" ) ) );				//0
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "folder" ) ) );				//1
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "page_white_c" ) ) );			//2
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "page_white_cplusplus" ) ) );	//3
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "page_white_h" ) ) );			//4
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "page_white_text" ) ) );		//5
	images->Add( wxXmlResource::Get()->LoadBitmap( wxT( "workspace" ) ) );			//6
	AssignImageList( images );

	Connect( GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( FileViewTree::OnPopupMenu ) );
	Connect( GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( FileViewTree::OnMouseDblClick ) );
	Connect( GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( FileViewTree::OnItemActivated ) );
}

FileViewTree::~FileViewTree()
{
	delete m_folderMenu;
	delete m_projectMenu;
	delete m_fileMenu;
	delete m_workspaceMenu;
}

void FileViewTree::Create( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
#ifndef __WXGTK__
	style |= ( wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT );
#else
	style |= ( wxTR_HAS_BUTTONS );
#endif
	wxTreeCtrl::Create( parent, id, pos, size, style );

	BuildTree();

	// Load the popup menu
	m_folderMenu = wxXmlResource::Get()->LoadMenu( wxT( "file_tree_folder" ) );
	m_projectMenu = wxXmlResource::Get()->LoadMenu( wxT( "file_tree_project" ) );
	m_fileMenu = wxXmlResource::Get()->LoadMenu( wxT( "file_tree_file" ) );
	m_workspaceMenu = wxXmlResource::Get()->LoadMenu( wxT( "workspace_popup_menu" ) );
	ConnectEvents();
}

void FileViewTree::BuildTree()
{
	DeleteAllItems();
	m_itemsToSort.clear();

	if ( ManagerST::Get()->IsWorkspaceOpen() ) {
		// Add an invisible tree root
		ProjectItem data;
		data.m_displayName = WorkspaceST::Get()->GetName();
		data.m_kind = ProjectItem::TypeWorkspace;

		wxTreeItemId root = AddRoot( data.m_displayName, 6, -1, new FilewViewTreeItemData( data ) );
		m_itemsToSort[root.m_pItem] = true;

		wxArrayString list;
		ManagerST::Get()->GetProjectList( list );

		Freeze();
		for ( size_t n=0; n<list.GetCount(); n++ ) {
			BuildProjectNode( list.Item( n ) );
		}

		SortTree();
		Thaw();

		//set selection to first item
		SelectItem( root );
	}
}

void FileViewTree::SortItem(wxTreeItemId &item)
{
	if (item.IsOk() && ItemHasChildren(item)) {
		SortChildren(item);
	}
}

void FileViewTree::SortTree()
{
	//sort the tree
	std::map<void*, bool>::const_iterator iter = m_itemsToSort.begin();
	for (; iter != m_itemsToSort.end(); iter ++) {
		wxTreeItemId item = iter->first;
		SortItem(item);
	}
	m_itemsToSort.clear();
}

wxTreeItemId FileViewTree::GetSingleSelection()
{
	/*
	wxArrayTreeItemIds items;
	size_t num = GetSelections(items);
	if(num >= 1){
		return items.Item(0);
	}
	return wxTreeItemId();
	*/
	return GetSelection();
}

int FileViewTree::GetIconIndex( const ProjectItem &item )
{
	int icondIndex( 5 );
	switch ( item.GetKind() ) {
	case ProjectItem::TypeProject:
		icondIndex = 0;
		break;
	case ProjectItem::TypeVirtualDirectory:
		icondIndex = 1;	// Folder
		break;
	case ProjectItem::TypeFile: {
		wxFileName filename( item.GetFile() );
		if ( filename.GetExt().CmpNoCase( wxT( "cpp" ) ) == 0 ) {
			icondIndex = 3;
		} else if ( filename.GetExt().CmpNoCase( wxT( "cxx" ) ) == 0 ) {
			icondIndex = 3;
		} else if ( filename.GetExt().CmpNoCase( wxT( "c++" ) ) == 0 ) {
			icondIndex = 3;
		} else if ( filename.GetExt().CmpNoCase( wxT( "cc" ) ) == 0 ) {
			icondIndex = 3;
		} else if ( filename.GetExt().CmpNoCase( wxT( "c" ) ) == 0 ) {
			icondIndex = 2;
		} else if ( filename.GetExt().CmpNoCase( wxT( "h" ) ) == 0 ) {
			icondIndex = 4;
		} else if ( filename.GetExt().CmpNoCase( wxT( "hpp" ) ) == 0 ) {
			icondIndex = 4;
		} else {
			icondIndex = 5;
		}
		break;
	}
	default:
		icondIndex = 5;	// Text file
		break;
	}
	return icondIndex;
}

void FileViewTree::BuildProjectNode( const wxString &projectName )
{
	ProjectTreePtr tree = ManagerST::Get()->GetProjectFileViewTree( projectName );
	TreeWalker<wxString, ProjectItem> walker( tree->GetRoot() );

	std::map<wxString, wxTreeItemId> items;
	for ( ; !walker.End(); walker++ ) {
		// Add the item to the tree
		ProjectTreeNode* node = walker.GetNode();
		wxTreeItemId parentHti;
		if ( node->IsRoot() ) {
			parentHti = GetRootItem();
		} else {
			wxString parentKey = node->GetParent()->GetKey();
			if ( items.find( parentKey ) == items.end() )
				continue;
			parentHti = items.find( parentKey )->second;
		}

		wxTreeItemId hti = AppendItem(	parentHti,							// parent
		                               node->GetData().GetDisplayName(),	// display name
		                               GetIconIndex( node->GetData() ),		// item image index
		                               GetIconIndex( node->GetData() ),		// selected item image
		                               new FilewViewTreeItemData( node->GetData() ) );
		m_itemsToSort[parentHti.m_pItem] = true;

		// Set active project with bold
		wxString activeProjectName = ManagerST::Get()->GetActiveProjectName();
		wxString displayName = node->GetData().GetDisplayName();

		if ( parentHti == GetRootItem() && displayName == activeProjectName) {
			SetItemBold( hti );
		}

		items[node->GetKey()] = hti;
	}
}

//-----------------------------------------------
// Event handlers
//-----------------------------------------------

void FileViewTree::PopupContextMenu( wxMenu *menu, MenuType type, const wxString &projectName )
{
	wxMenuItem *itemSep(NULL);
	wxMenuItem *item(NULL);
	if ( type == MenuTypeFileView_Project ) {

		BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(projectName, wxEmptyString);
		if (bldConf && bldConf->IsCustomBuild()) {
			wxString toolName = bldConf->GetToolName();
			if (toolName != wxT("None")) {
				//add the custom execution command
				itemSep = new wxMenuItem(menu, wxID_SEPARATOR);
				menu->Prepend(itemSep);

				wxString menu_text(wxT("Run ") + toolName);

				item = new wxMenuItem(menu, XRCID("generate_makefile"), menu_text, wxEmptyString, wxITEM_NORMAL);
				menu->Prepend(item);
			}
		}
	}

	if (ManagerST::Get()->IsBuildInProgress() == false) {
		PluginManager::Get()->HookPopupMenu( menu, type );
	}
	PopupMenu( menu );
	if (ManagerST::Get()->IsBuildInProgress() == false) {
		//let the plugins remove their hooked content
		PluginManager::Get()->UnHookPopupMenu( menu, type );
	}

	//remove the custom makefile hooked menu items
	if (item) {
		menu->Destroy(item);
	}
	if (itemSep) {
		menu->Destroy(itemSep);
	}
}

void FileViewTree::OnPopupMenu( wxTreeEvent &event )
{
	if ( GetSingleSelection().IsOk() ) {
		wxTreeItemId item = event.GetItem();
		if ( item.IsOk() ) {
			SelectItem( item, true );

			FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
			switch ( data->GetData().GetKind() ) {
			case ProjectItem::TypeProject:
				PopupContextMenu( m_projectMenu, MenuTypeFileView_Project, data->GetData().GetDisplayName() );
				break;
			case ProjectItem::TypeVirtualDirectory:
				PopupContextMenu( m_folderMenu, MenuTypeFileView_Folder );
				break;
			case ProjectItem::TypeFile:
				PopupContextMenu( m_fileMenu, MenuTypeFileView_File );
				break;
			case ProjectItem::TypeWorkspace:
				PopupContextMenu( m_workspaceMenu, MenuTypeFileView_Workspace );
				break;
			default:
				break;
			}
		}
	}
}

TreeItemInfo FileViewTree::GetSelectedItemInfo()
{
	wxTreeItemId item = GetSingleSelection();
	TreeItemInfo info;
	info.m_item = item;
	if ( item.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );

		//set the text of the item
		info.m_text = GetItemText( item );
		info.m_itemType = data->GetData().GetKind();
		info.m_fileName  = data->GetData().GetFile();
		if (info.m_itemType == ProjectItem::TypeVirtualDirectory) {
			//incase of virtual directories, set the file name to be the directory of
			//the project
			wxString path = GetItemPath(item);
			wxString project = path.BeforeFirst(wxT(':'));
			info.m_fileName = wxFileName(ManagerST::Get()->GetProjectCwd(project), wxEmptyString);
		}
	}
	return info;
}

void FileViewTree::OnMouseDblClick( wxMouseEvent &event )
{
	wxArrayTreeItemIds items;
	size_t num = GetMultiSelection( items );
	if ( num <= 0 ) {
		event.Skip();
		return;
	}

	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	for ( size_t i=0; i<num; i++ ) {
		if ( HitTest( event.GetPosition(), flags ) == items.Item( i ) ) {
			wxTreeItemId item = items.Item( i );
			DoItemActivated( item, event );
			return;
		}
	}
	event.Skip();
}

void FileViewTree::DoItemActivated( wxTreeItemId &item, wxEvent &event )
{
	//-----------------------------------------------------
	// Each tree items, keeps a private user data that
	// holds the key for searching the its corresponding
	// node in the m_tree data structure
	//-----------------------------------------------------
	if ( item.IsOk() == false )
		return;
	FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
	if ( !itemData ) {
		event.Skip();
		return;
	}

	//if file item was hit, open it
	if ( itemData->GetData().GetKind() == ProjectItem::TypeFile ) {
		wxString filename = itemData->GetData().GetFile();
		wxString project  = itemData->GetData().Key().BeforeFirst( wxT( ':' ) );

		// Convert the file name to be in absolute path
		wxFileName fn( filename );
		fn.MakeAbsolute( ManagerST::Get()->GetProjectCwd( project ) );
		ManagerST::Get()->OpenFile( fn.GetFullPath(), project, -1 );
		return;
	} else { // if(itemData->GetData().GetKind() == ProjectItem::TypeVirtualDirectory)
		event.Skip();
	}
}

void FileViewTree::OnExportMakefile( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName, errMsg;
		//TODO:: make the builder name configurable
		BuilderPtr builder = BuildManagerST::Get()->GetBuilder( wxT( "GNU makefile for g++/gcc" ) );
		projectName = GetItemText( item );
		if ( !builder->Export( projectName, wxEmptyString, false, true, errMsg ) ) {
			wxMessageBox( errMsg, wxT( "CodeLite" ), wxICON_HAND );
			return;
		}
	}
}

void FileViewTree::OnRemoveProject( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
		if ( data->GetData().GetKind() == ProjectItem::TypeProject ) {
			DoRemoveProject( data->GetData().GetDisplayName() );
		}
	}
}

void FileViewTree::OnSortItem( wxCommandEvent &WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	SortItem(item);
}

bool FileViewTree::AddFilesToVirtualFodler(wxTreeItemId &item, wxArrayString &paths)
{
	if (item.IsOk() == false)
		return false;

	FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
	switch ( data->GetData().GetKind() ) {
	case ProjectItem::TypeVirtualDirectory:
		//OK
		break;
	default:
		return false;
	}

	wxArrayString actualAdded;
	wxString vdPath = GetItemPath( item );
	wxString project;
	project = vdPath.BeforeFirst( wxT( ':' ) );
	ManagerST::Get()->AddFilesToProject( paths, vdPath, actualAdded );
	for ( size_t i=0; i<actualAdded.Count(); i++ ) {

		// Add the tree node
		wxFileName fnFileName( actualAdded.Item( i ) );
		wxString path( vdPath );
		path += wxT( ":" );
		path += fnFileName.GetFullName();
		ProjectItem projItem( path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile );

		wxTreeItemId hti = AppendItem(	item,						// parent
		                               projItem.GetDisplayName(),	// display name
		                               GetIconIndex( projItem ),		// item image index
		                               GetIconIndex( projItem ),		// selected item image
		                               new FilewViewTreeItemData( projItem ) );
		wxUnusedVar( hti );
	}

	SortItem(item);
	Expand( item );
	return true;
}

void FileViewTree::OnAddExistingItem( wxCommandEvent & WXUNUSED( event ) )
{
	static wxString start_path(wxEmptyString);
	
	wxTreeItemId item = GetSingleSelection();
	if ( !item.IsOk() ) {
		return;
	}
	const wxString ALL(	wxT("All Files (*)|*|")
	                    wxT( "C/C++ Source Files (*.c;*.cpp;*.cxx;*.cc)|*.c;*.cpp;*.cxx;*.cc|" )
	                    wxT( "C/C++ Header Files (*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc)|*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc" ) );
	wxString vdPath = GetItemPath( item );
	wxString project;
	project = vdPath.BeforeFirst( wxT( ':' ) );

	wxArrayString paths;
	ProjectPtr proj = ManagerST::Get()->GetProject( project );
	if(start_path.IsEmpty()) {
		start_path = proj->GetFileName().GetPath();
	}
	
	wxFileDialog *dlg = new wxFileDialog( this, wxT( "Add Existing Item" ), start_path, wxEmptyString, ALL, wxFD_MULTIPLE | wxOPEN | wxFILE_MUST_EXIST , wxDefaultPosition );
	if ( dlg->ShowModal() == wxID_OK ) {
		dlg->GetPaths( paths );
		
		if(paths.IsEmpty() == false){
			// keep the last used path
			wxFileName fn(paths.Item(0));
			start_path = fn.GetPath();
		}
		AddFilesToVirtualFodler(item, paths);
	}
	dlg->Destroy();
	SendCmdEvent(wxEVT_PROJ_FILE_ADDED, (void*)&paths);
}

void FileViewTree::OnNewItem( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	if ( !item.IsOk() ) {
		return;
	}

	wxString path = GetItemPath( item );

	// Project name
	wxString projName = path.BeforeFirst( wxT( ':' ) );
	wxString projCwd = ManagerST::Get()->GetProjectCwd( projName );

	NewItemDlg *dlg = new NewItemDlg( this, projCwd, wxID_ANY, wxT( "New Item" ) );

	if ( dlg->ShowModal() == wxID_OK ) {
		wxString filename = dlg->GetFileName().GetFullPath();
		ManagerST::Get()->AddNewFileToProject( filename, path );

		// Add the tree node
		wxFileName fnFileName( filename );
		path += wxT( ":" );
		path += fnFileName.GetFullName();
		ProjectItem projItem( path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile );

		wxTreeItemId hti = AppendItem(	item,						// parent
		                               projItem.GetDisplayName(),	// display name
		                               GetIconIndex( projItem ),		// item image index
		                               GetIconIndex( projItem ),		// selected item image
		                               new FilewViewTreeItemData( projItem ) );
		wxUnusedVar( hti );
		SortItem(item);
		Expand( item );

		wxArrayString arrStr;
		arrStr.Add(filename);
		SendCmdEvent(wxEVT_PROJ_FILE_ADDED, (void*)&arrStr);
	}

	dlg->Destroy();
}

void FileViewTree::OnSetActive( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	DoSetProjectActive( item );
}

void FileViewTree::DoSetProjectActive( wxTreeItemId &item )
{
	if ( item.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
		if ( data->GetData().GetKind() == ProjectItem::TypeProject ) {

			wxString curActiveProj = ManagerST::Get()->GetActiveProjectName();

			// find previous active project and remove its bold style
			wxTreeItemIdValue cookie;
			wxTreeItemId child = GetFirstChild( GetRootItem(), cookie );
			while ( child.IsOk() ) {
				FilewViewTreeItemData *childData = static_cast<FilewViewTreeItemData*>( GetItemData( child ) );
				if ( childData->GetData().GetDisplayName() == curActiveProj ) {
					SetItemBold( child, false );
					break;
				}
				child = GetNextChild( GetRootItem(), cookie );
			}

			ManagerST::Get()->SetActiveProject( data->GetData().GetDisplayName() );
			SetItemBold( item );
		}
	}

}

void FileViewTree::OnRemoveVirtualFolder( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	DoRemoveVirtualFolder( item );
}

void FileViewTree::OnRemoveItem( wxCommandEvent &WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	DoRemoveItem( item );
}

void FileViewTree::DoRemoveItem( wxTreeItemId &item )
{
	wxString name = GetItemText( item );
	FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
	switch (data->GetData().GetKind()) {
	case ProjectItem::TypeFile: {
		wxString message;
		message << wxT( "Are you sure you want remove '" ) << name << wxT( "' ?" );
		if ( wxMessageBox( message, wxT( "CodeLite" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
			wxTreeItemId parent = GetItemParent( item );
			if ( parent.IsOk() ) {
				wxString path = GetItemPath( parent );
				ManagerST::Get()->RemoveFile( data->GetData().GetFile(), path );

				wxString file_name(data->GetData().GetFile());
				Delete( item );

				SendCmdEvent(wxEVT_PROJ_FILE_REMOVED, (void*)&file_name);
			}
		}
	}
	break;
	case ProjectItem::TypeVirtualDirectory:
		DoRemoveVirtualFolder(item);
		break;
	case ProjectItem::TypeProject:
		DoRemoveProject(name);
		break;
	default:
		break;
	}
}

void FileViewTree::DoRemoveVirtualFolder( wxTreeItemId &item )
{
	wxString name = GetItemText( item );
	wxString message( wxT( "'" ) + name + wxT( "'" ) );
	message << wxT( " and all its content will be removed from the project." );

	if ( wxMessageBox( message, wxT( "CodeLite" ), wxYES_NO|wxICON_WARNING ) == wxYES ) {
		wxString path = GetItemPath( item );
		DeleteChildren( item );
		Delete( item );

		ManagerST::Get()->RemoveVirtualDirectory( path );
	}
}

void FileViewTree::OnNewVirtualFolder( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	DoAddVirtualFolder( item );
}

void FileViewTree::DoAddVirtualFolder( wxTreeItemId &parent )
{
	static int count = 0;
	wxString defaultName( wxT( "NewDirectory" ) );
	defaultName << count++;

	wxTextEntryDialog *dlg = new wxTextEntryDialog( NULL, wxT( "Virtual Directory Name:" ), wxT( "New Virtual Directory" ), defaultName );
	dlg->Centre();
	if ( dlg->ShowModal() == wxID_OK ) {
		wxString path = GetItemPath( parent );
		if ( dlg->GetValue().Trim().IsEmpty() )
			return;

		path += wxT( ":" );
		path += dlg->GetValue();

		ProjectItem itemData( path, dlg->GetValue(), wxEmptyString, ProjectItem::TypeVirtualDirectory );
		AppendItem(	parent,								// parent
		            itemData.GetDisplayName(),	// display name
		            GetIconIndex( itemData ),		// item image index
		            GetIconIndex( itemData ),		// selected item image
		            new FilewViewTreeItemData( itemData ) );

		SortItem( parent );
		Expand( parent );

		ManagerST::Get()->AddVirtualDirectory( path );
	}
	dlg->Destroy();
}

wxString FileViewTree::GetItemPath( wxTreeItemId &item )
{
	std::deque<wxString> queue;
	wxString text = GetItemText( item );
	queue.push_front( text );

	wxTreeItemId p = GetItemParent( item );
	while ( p.IsOk() && p != GetRootItem() ) {

		text = GetItemText( p );
		queue.push_front( text );
		p = GetItemParent( p );
	}

	wxString path;
	size_t count = queue.size();
	for ( size_t i=0; i<count; i++ ) {
		path += queue.front();
		path += wxT( ":" );
		queue.pop_front();
	}

	if ( !queue.empty() ) {
		path += queue.front();
	} else {
		path = path.BeforeLast( wxT( ':' ) );
	}

	return path;
}

void FileViewTree::OnProjectProperties( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();

	wxString projectName( GetItemText( item ) );
	wxString title( projectName );
	title << wxT( " Project Settings" );

	//open the project properties dialog
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	//find the project configuration name that matches the workspace selected configuration
	ProjectSettingsDlg *dlg = new ProjectSettingsDlg( Frame::Get(), matrix->GetProjectSelectedConf( matrix->GetSelectedConfigurationName(), projectName ),
	        projectName,
	        title );
	dlg->ShowModal();

	//mark this project as modified
	ProjectPtr proj = ManagerST::Get()->GetProject(projectName);
	if (proj) {
		proj->SetModified(true);
	}

	dlg->Destroy();
}

void FileViewTree::DoRemoveProject( const wxString &name )
{
	wxString message ( wxT( "You are about to remove project '" ) );
	message << name << wxT( "' " );
	message << wxT( " from the workspace, click 'Yes' to proceed or 'No' to abort." );
	if ( wxMessageBox ( message, wxT( "Confirm" ), wxYES_NO ) == wxYES ) {
		ManagerST::Get()->RemoveProject( name );
	}
}

int FileViewTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
	// used for SortChildren, reroute to our sort routine
	FilewViewTreeItemData *a = (FilewViewTreeItemData *)GetItemData(item1),
	                           *b = (FilewViewTreeItemData *)GetItemData(item2);
	if (a && b)
		return OnCompareItems(a,b);

	return 0;
}

int FileViewTree::OnCompareItems(const FilewViewTreeItemData *a, const FilewViewTreeItemData *b)
{
	// if dir and other is not, dir has preference
	if (a->GetData().GetKind() == ProjectItem::TypeVirtualDirectory &&
	        b->GetData().GetKind() == ProjectItem::TypeFile)
		return -1;
	else if (b->GetData().GetKind() == ProjectItem::TypeVirtualDirectory &&
	         a->GetData().GetKind() == ProjectItem::TypeFile)
		return 1;

	// else let ascii fight it out
	return a->GetData().GetDisplayName().CmpNoCase(b->GetData().GetDisplayName());
}

void FileViewTree::OnSaveAsTemplate( wxCommandEvent & WXUNUSED( event ) )
{
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString name = GetItemText( item );
		ProjectPtr proj = ManagerST::Get()->GetProject( name );
		if ( proj ) {
			NameAndDescDlg *dlg = new NameAndDescDlg(NULL, name);
			if ( dlg->ShowModal() == wxID_OK ) {
				wxString newName = dlg->GetName();
				wxString desc  	 = dlg->GetDescription();

				newName = newName.Trim().Trim(false);
				desc = desc.Trim().Trim(false);

				if ( newName.IsEmpty() == false ) {
					ManagerST::Get()->SaveProjectTemplate( proj, newName, desc );
				}
			}
			dlg->Destroy();
		}
	}
}

void FileViewTree::OnBuildOrder( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		ManagerST::Get()->PopupProjectDependsDlg( projectName );
	}
}

void FileViewTree::OnClean( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		
		QueueCommand buildInfo(projectName, wxEmptyString, false, QueueCommand::Clean);
		ManagerST::Get()->CleanProject( buildInfo );
	}
}

void FileViewTree::OnBuild( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		QueueCommand buildInfo(projectName, wxEmptyString, false, QueueCommand::Build);
		ManagerST::Get()->BuildProject( buildInfo );
	}
}

void FileViewTree::OnCompileItem(wxCommandEvent &e)
{
	wxUnusedVar( e );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
		if (data->GetData().GetKind() == ProjectItem::TypeFile) {
			Manager *mgr = ManagerST::Get();
			wxTreeItemId parent = GetItemParent( item );
			if ( parent.IsOk() ) {
				wxString logmsg;
				wxString path = GetItemPath( parent );
				wxString proj = path.BeforeFirst(wxT(':'));
				logmsg << wxT("Compiling file: ") << data->GetData().GetFile() << wxT(" of project ") << proj << wxT("\n");
				mgr->CompileFile(proj, data->GetData().GetFile());
			}
		}
	}
}


void FileViewTree::OnStopBuild( wxCommandEvent &event )
{
	wxUnusedVar( event );
	ManagerST::Get()->StopBuild();
}

void FileViewTree::OnItemActivated( wxTreeEvent &event )
{
	if ( event.GetKeyCode() == WXK_RETURN ) {
		wxArrayTreeItemIds items;
		size_t num = GetMultiSelection( items );
		if ( num > 0 ) {
			for ( size_t i=0; i<num; i++ ) {
				wxTreeItemId item = items.Item( i );
				DoItemActivated( item, event );
			}
		}
	} else if (event.GetKeyCode() == WXK_DELETE ||  event.GetKeyCode() == WXK_NUMPAD_DELETE ) {
		wxArrayTreeItemIds items;
		size_t num = GetMultiSelection( items );
		if ( num > 0 ) {
			for ( size_t i=0; i<num; i++ ) {
				wxTreeItemId item = items.Item( i );
				DoRemoveItem( item );
			}
		}
	} else {
		event.Skip();
	}
}

size_t FileViewTree::GetMultiSelection( wxArrayTreeItemIds &arr )
{
	if ( GetWindowStyleFlag() & wxTR_MULTIPLE ) {
		return GetSelections( arr );
	} else {
		arr.Add( GetSelection() );
		return 1;
	}
}

void FileViewTree::OnRetagProject( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		ManagerST::Get()->RetagProject( projectName );
	}
}

void FileViewTree::OnRetagWorkspace( wxCommandEvent &event )
{
	wxUnusedVar( event );
	ManagerST::Get()->RetagWorkspace();
}

void FileViewTree::OnItemBeginDrag( wxTreeEvent &event )
{
	if ( event.GetItem() != GetRootItem() ) {
		m_draggedItem = event.GetItem();
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( m_draggedItem ) );
		if ( data->GetData().GetKind() == ProjectItem::TypeFile ) {
			event.Allow();
			return;
		}
	}
}

void FileViewTree::OnItemEndDrag( wxTreeEvent &event )
{
	wxTreeItemId itemSrc = m_draggedItem,
	                       itemDst = event.GetItem();

	wxString targetVD, fromVD;
	if ( itemDst.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( itemDst ) );
		if ( data->GetData().GetKind() == ProjectItem::TypeVirtualDirectory ) {
			//we are only allowed items between virtual folders
			wxTreeItemId target  = itemDst;
			if ( target.IsOk() ) {
				targetVD = GetItemPath( target );
			} else {
				return;
			}

			wxTreeItemId fromItem  = GetItemParent( itemSrc );
			if ( fromItem.IsOk() ) {
				fromVD = GetItemPath( fromItem );
			} else {
				return;
			}

			//the file name to remove
			FilewViewTreeItemData *srcData = static_cast<FilewViewTreeItemData*>( GetItemData( itemSrc ) );
			wxString filename = srcData->GetData().GetFile();

			ProjectItem itemData = srcData->GetData();

			//call the manager to remove them in the underlying project
			if ( ManagerST::Get()->MoveFileToVD( filename, fromVD, targetVD ) ) {
				//remove the item from its current node, and place it under the
				//new parent node
				AppendItem(	target,						// parent
				            itemData.GetDisplayName(),	// display name
				            GetIconIndex( itemData ),		// item image index
				            GetIconIndex( itemData ),		// selected item image
				            new FilewViewTreeItemData( itemData ) );
				Delete( itemSrc );
				Expand( target );
			}
		}
	}
}

void FileViewTree::OnBuildProjectOnly( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		QueueCommand info(projectName, wxEmptyString, true, QueueCommand::Build);
		ManagerST::Get()->BuildProject( info );
	}
}

void FileViewTree::OnCleanProjectOnly( wxCommandEvent &event )
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		QueueCommand info(projectName, wxEmptyString, true, QueueCommand::Clean);
		ManagerST::Get()->CleanProject( info );
	}
}

void FileViewTree::ExpandToPath(const wxString &project, const wxFileName &fileName)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(GetRootItem(), cookie);
	while (child.IsOk()) {
		FilewViewTreeItemData *childData = static_cast<FilewViewTreeItemData*>( GetItemData( child ) );
		if (childData->GetData().GetDisplayName() == project) {
			wxTreeItemId fileItem = FindItemByPath(child, ManagerST::Get()->GetProjectCwd( project ), fileName.GetFullPath());
			if (fileItem.IsOk()) {
				SelectItem(fileItem);
			} else {
				wxString message;
				message << wxT("Failed to find file: ") << fileName.GetFullPath() << wxT(" in FileView.");
				wxLogMessage(message);
			}
			break;
		}
		child = GetNextChild(GetRootItem(), cookie);
	}
}

wxTreeItemId FileViewTree::FindItemByPath(wxTreeItemId &parent, const wxString &projectPath, const wxString &fileName)
{
	if (!parent.IsOk())
		return wxTreeItemId();

	if (!ItemHasChildren(parent))
		return wxTreeItemId();

	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while (child.IsOk()) {
		FilewViewTreeItemData *childData = static_cast<FilewViewTreeItemData*>( GetItemData( child ) );
		wxFileName fn(childData->GetData().GetFile());
		fn.MakeAbsolute( projectPath );
		if (fn.GetFullPath() == fileName) {
			return child;
		}

		if (ItemHasChildren(child)) {
			wxTreeItemId res = FindItemByPath(child, projectPath, fileName);
			if (res.IsOk()) {
				return res;
			}
		}
		child = GetNextChild(parent, cookie);
	}
	return wxTreeItemId();
}

void FileViewTree::OnImportDirectory(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxTreeItemId item = GetSingleSelection();
	if ( !item.IsOk() ) {
		return;
	}

	wxString message;
	message << wxT("Select Directory to import:");


	wxString vdPath = GetItemPath( item );
	wxString project;
	project = vdPath.BeforeFirst(wxT(':'));

	ProjectPtr proj = ManagerST::Get()->GetProject( project );

	ImportFilesDlg *dlg = new ImportFilesDlg(NULL, proj->GetFileName().GetPath());
	if (dlg->ShowModal() != wxID_OK) {
		dlg->Destroy();
		return;
	}

	wxString path = dlg->GetBaseDir();
	bool noExtFiles = dlg->GetIncludeFilesWoExt();
	wxString mask = dlg->GetFileMask();
	dlg->Destroy();

	wxFileName rootPath(path);

	//Collect all candidates files
	wxArrayString files;
	DirTraverser trv(mask, noExtFiles);

	//ignore .svn & .cvs files
	wxArrayString excludeDirs;
	excludeDirs.Add(wxT(".svn"));
	excludeDirs.Add(wxT(".cvs"));

	trv.SetExcludeDirs(excludeDirs);
	wxDir dir(path);

	dir.Traverse(trv);
	files = trv.GetFiles();

	//loop over the files and construct for each file a record with
	//the following information:
	//-virtual directory (full path, starting from project level)
	//-display name
	//-full path of the file
	proj->BeginTranscation();
	{
		// Create a progress dialog
		wxProgressDialog *prgDlg = new wxProgressDialog (wxT("Importing files ..."), wxT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"), (int)files.GetCount(), NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE);
		prgDlg->GetSizer()->Fit(prgDlg);
		prgDlg->Layout();
		prgDlg->Centre();

		for (size_t i=0; i<files.GetCount(); i++) {
			wxFileName fn(files.Item(i));
			wxString filename = files.Item(i);
			FileViewItem fvitem;
			fvitem.fullpath = fn.GetFullPath();
			fvitem.displayName = fn.GetFullName();

			fn.MakeRelativeTo(path);

			wxString relativePath = fn.GetPath();
			relativePath.Replace(wxT("/"), wxT(":"));
			relativePath.Replace(wxT("\\"), wxT(":"));

			if (relativePath.IsEmpty()) {
				//the file is probably under the root, add it under
				//a virtual directory with the name of the target
				//root folder
				relativePath = rootPath.GetName();
			}
			relativePath.Append(wxT(":"));

			fvitem.virtualDir = relativePath;
			DoAddItem(proj, fvitem);

			wxString msg;
			msg << wxT("Adding file: ") << fn.GetFullPath();
			prgDlg->Update((int)i, msg);
		}
		m_itemsToSort.clear();
		prgDlg->Destroy();
	}

	//save the project file to disk
	proj->CommitTranscation();

	//reload the project
	ManagerST::Get()->RemoveProject( proj->GetName() );
	ManagerST::Get()->AddProject(proj->GetFileName().GetFullPath());
}

void FileViewTree::DoAddItem(ProjectPtr proj, const FileViewItem &item)
{
	if (!proj) {
		return;
	}

	Manager *mgr = ManagerST::Get();
	//make sure that this file does not exist
	wxString projName = mgr->GetProjectNameByFile(item.fullpath);
	if (projName.IsEmpty()) {
		//first add the virtual directory, if it already exist,
		//this function does nothing
		proj->CreateVirtualDir(item.virtualDir, true);

		//add the file.
		//For performance reasons, we dont go through the Workspace API
		//but directly through the project API
		proj->AddFile(item.fullpath, item.virtualDir);
	}
}

void FileViewTree::OnRunPremakeStep(wxCommandEvent &event)
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		ManagerST::Get()->RunCustomPreMakeCommand( projectName );
	}
}

void FileViewTree::OnRenameItem(wxCommandEvent& e)
{
	wxUnusedVar( e );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		FilewViewTreeItemData *data = static_cast<FilewViewTreeItemData*>( GetItemData( item ) );
		if (data->GetData().GetKind() == ProjectItem::TypeFile) {
			wxTreeItemId parent = GetItemParent( item );
			if ( parent.IsOk() ) {

				wxString path = GetItemPath( parent );
				wxString proj = path.BeforeFirst(wxT(':'));

				ProjectPtr p = ManagerST::Get()->GetProject(proj);
				if ( p ) {
					// prompt user for new name
					wxString newName = wxGetTextFromUser(wxT("New file name:"), wxT("Rename file:"), GetItemText(item));
					if ( newName.IsEmpty() == false ) {

						// remove the file from the workspace (this will erase it from the symbol database and will
						// also close the editor that it is currently opened in (if any)
						if (ManagerST::Get()->RemoveFile(GetItemText(item), path)) {

							// rename the file
							wxFileName tmp(data->GetData().GetFile());
							tmp.SetFullName(newName);
							wxRenameFile(data->GetData().GetFile(), tmp.GetFullPath());

							// add the new file to the project
							FileViewItem new_item;
							new_item.fullpath = tmp.GetFullPath();
							new_item.displayName = tmp.GetFullName();
							new_item.virtualDir = path.AfterFirst(wxT(':'));
							DoAddItem(p, new_item);

							// update the item's info
							data->SetDisplayName(new_item.displayName);
							data->SetFile(new_item.fullpath);

							// rename the tree item
							SetItemText(item, new_item.displayName);

							ManagerST::Get()->RetagFile(new_item.fullpath);

						}
					}
				} // p
			}// parent.IsOk()
		}//TypeFile
	}//item.IsOk()
}

void FileViewTree::OnRenameVirtualFolder(wxCommandEvent& e)
{
	wxUnusedVar( e );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		// got the item, prompt user for new name
		wxString newName = wxGetTextFromUser(wxT("New virtual folder name:"), wxT("Rename virtual folder:"), GetItemText(item));
		if (newName.IsEmpty() || newName == GetItemText(item)) {
			// user clicked cancel
			return;
		}

		// locate the project
		wxString path = GetItemPath( item );
		wxString proj = path.BeforeFirst(wxT(':'));

		path = path.AfterFirst(wxT(':'));
		ProjectPtr p = ManagerST::Get()->GetProject(proj);
		if (!p) {
			wxLogMessage(wxT("failed to rename virtual folder: ") + path + wxT(", reason: could not locate project ") + proj);
			return;
		}
		
		if (!p->RenameVirtualDirectory(path, newName)) {
			wxLogMessage(wxT("failed to rename virtual folder: ") + path);
			return;
		}
		SetItemText(item, newName);
	}
}

void FileViewTree::OnReBuild(wxCommandEvent& event)
{
	wxUnusedVar( event );
	wxTreeItemId item = GetSingleSelection();
	if ( item.IsOk() ) {
		wxString projectName = GetItemText( item );
		Frame::Get()->RebuildProject( projectName );
	}
}
