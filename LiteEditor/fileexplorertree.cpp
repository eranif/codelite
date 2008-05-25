//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : fileexplorertree.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #include "fileexplorertree.h"
#include "manager.h"
#include "wx/xrc/xmlres.h"
#include "pluginmanager.h"
#include "globals.h"
#include "dirsaver.h"
#include "procutils.h"

BEGIN_EVENT_TABLE(FileExplorerTree, wxVirtualDirTreeCtrl)
	EVT_TREE_ITEM_MENU(wxID_ANY, FileExplorerTree::OnContextMenu)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, FileExplorerTree::OnItemActivated)
END_EVENT_TABLE()

FileExplorerTree::FileExplorerTree(wxWindow *parent, wxWindowID id)
		: wxVirtualDirTreeCtrl(parent, id)
		, m_rclickMenu(NULL)
{
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("file_explorer_menu"));
	Connect(XRCID("open_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFile), NULL, this);
	Connect(XRCID("open_file_in_text_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFileInTextEditor), NULL, this);
	Connect(XRCID("refresh_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnRefreshNode), NULL, this);
	Connect(XRCID("delete_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnDeleteNode), NULL, this);
	Connect(XRCID("open_shell"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenShell), NULL, this);
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( FileExplorerTree::OnMouseDblClick ) );
	Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileExplorerTree::OnKeyDown));
}

FileExplorerTree::~FileExplorerTree()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}
}

void FileExplorerTree::OnKeyDown(wxTreeEvent &e)
{
	if(e.GetKeyCode() == WXK_RETURN || e.GetKeyCode() == WXK_NUMPAD_ENTER){
		wxTreeItemId item = GetSelection();
		DoItemActivated(item);
	}else if(e.GetKeyCode() == WXK_DELETE || e.GetKeyCode() == WXK_NUMPAD_DELETE){
		wxCommandEvent dummy;
		OnDeleteNode(dummy);
	}else{
		e.Skip();
	}
}

void FileExplorerTree::OnDeleteNode(wxCommandEvent &e)
{
	wxTreeItemId item = GetSelection();
	bool needRefresh = false;

	if (item.IsOk()) {
		wxString fp = GetFullPath(item).GetFullPath();
		VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(item);
		if (b && b->IsDir()) {
			wxString msg;
			msg << wxT("'") << GetItemText(item) << wxT("' is a directory. Are you sure you want to remove it and its content?");
			if (wxMessageBox(msg, wxT("Remove Directory"), wxICON_WARNING|wxYES_NO|wxCANCEL) == wxYES) {
				if (!RemoveDirectory(fp)) {
					wxMessageBox(wxT("Failed to remove directory"), wxT("Remove Directory"), wxICON_ERROR | wxOK);
				}else{
					needRefresh = true;
				}
			}
		} else {
			if (wxRemoveFile(fp)) {
				needRefresh = true;
			}
		}
		if (needRefresh) {
			wxTreeItemId parent = GetItemParent(item);
			if(parent.IsOk()){
				//select the parent, and call refresh.
				//by making the parent the selected item, 
				//we force the refresh to take place on the parent node
				SelectItem(parent);
				wxCommandEvent dummy;
				OnRefreshNode(dummy);
			}
		}
	}

	e.Skip();
}

void FileExplorerTree::OnContextMenu(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		SelectItem(item);
		if (m_rclickMenu) {
			//let the plugins hook their content
			PluginManager::Get()->HookPopupMenu(m_rclickMenu, MenuTypeFileExplorer);
			PopupMenu(m_rclickMenu);
			//let the plugins remove their hooked content
			PluginManager::Get()->UnHookPopupMenu(m_rclickMenu, MenuTypeFileExplorer);
		}
	}
}

void FileExplorerTree::DoOpenItem(const wxTreeItemId &item)
{
	if (item.IsOk()) {
		wxFileName fn = GetFullPath(item);
		if (fn.GetExt() == wxT("workspace")) {
			//open workspace
			ManagerST::Get()->OpenWorkspace(fn.GetFullPath());
		} else {
			ManagerST::Get()->OpenFile(fn.GetFullPath(), wxEmptyString);
		}
	}
}

void FileExplorerTree::DoOpenItemInTextEditor(const wxTreeItemId &item)
{
	if (item.IsOk()) {
		wxFileName fn = GetFullPath(item);
		ManagerST::Get()->OpenFile(fn.GetFullPath(), wxEmptyString);
	}
}

void FileExplorerTree::OnMouseDblClick( wxMouseEvent &event ) {
	wxTreeItemId item = GetSelection();
	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	
	if (HitTest( event.GetPosition(), flags ) == item) {
		DoItemActivated( item );
		return;
	}
	event.Skip();
}

void FileExplorerTree::DoItemActivated(const wxTreeItemId &item)
{
	if (item.IsOk()) {
		VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(item);
		if (b && b->IsDir()) {
			Freeze();
			if(IsExpanded(item)){
				Collapse(item);
			}else{
				Expand(item);
			}
			Thaw();
		} else {
			DoOpenItem(item);
		}
	}
}

void FileExplorerTree::OnItemActivated(wxTreeEvent &event)
{
	DoItemActivated(event.GetItem());
	event.Skip();
}

void FileExplorerTree::OnOpenFile(wxCommandEvent &e)
{
	//Get the selected item
	wxUnusedVar(e);
	wxTreeItemId item = GetSelection();
	DoOpenItem(item);
}

void FileExplorerTree::OnOpenFileInTextEditor(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxTreeItemId item = GetSelection();
	DoOpenItemInTextEditor(item);
}

TreeItemInfo FileExplorerTree::GetSelectedItemInfo()
{
	wxTreeItemId item = GetSelection();
	TreeItemInfo info;
	info.m_item = item;
	if ( item.IsOk() ) {
		//set the text of the item
		info.m_text = GetItemText( item );
		info.m_fileName  = GetFullPath(item);
	}
	return info;
}

void FileExplorerTree::OnRefreshNode(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTreeItemId item = GetSelection();
	Freeze();
	DoReloadNode(item);
	Thaw();

/*	wxCommandEvent e(wxEVT_FILE_EXP_REFRESHED, GetId());
	e.SetEventObject(this);
	GetEventHandler()->ProcessEvent(e);*/
}

void FileExplorerTree::OnOpenShell(wxCommandEvent &event)
{
	DirSaver ds;
	wxTreeItemId item = GetSelection();
	if(item.IsOk()){
		wxFileName fullpath = GetFullPath(item);
		wxSetWorkingDirectory(fullpath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
		if(!ProcUtils::Shell()){
			wxMessageBox(wxT("Failed to load shell terminal"), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}
	}
}

