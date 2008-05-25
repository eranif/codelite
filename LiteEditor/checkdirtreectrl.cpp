//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : checkdirtreectrl.cpp              
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
 #include "checkdirtreectrl.h"
#include "wx/dir.h"
#include "wx/filename.h"

BEGIN_EVENT_TABLE(CheckDirTreeCtrl, wxCheckTreeCtrl)
EVT_TREE_ITEM_EXPANDED(wxID_ANY, CheckDirTreeCtrl::OnItemExpading)
EVT_CHECKTREE_ITEM_UNSELECTED(wxID_ANY, CheckDirTreeCtrl::OnItemUnchecked)
EVT_CHECKTREE_ITEM_SELECTED(wxID_ANY, CheckDirTreeCtrl::OnItemChecked)
END_EVENT_TABLE()

CheckDirTreeCtrl::CheckDirTreeCtrl(wxWindow *parent, const wxString &rootPath, wxWindowID id)
: wxCheckTreeCtrl(parent, id)
{
	BuildTree(rootPath);
}

void CheckDirTreeCtrl::BuildTree(const wxString &rootPath)
{
	m_root = rootPath;
	DeleteAllItems();
	if(m_root.IsEmpty()){
		return;
	}

	wxFileName filename(m_root, wxEmptyString);
	wxTreeItemId item = AddRoot(filename.GetPath(), false, new DirTreeData(filename));
	AddChildren(item);
	Expand(item);
}

void CheckDirTreeCtrl::AddChildren(const wxTreeItemId &item)
{
	DirTreeData *data = dynamic_cast<DirTreeData*>(GetItemData(item));
	if(data){
		wxString path = data->GetDir().GetPath();
		wxDir dir(path);
		if(wxDir::Exists(path)){
			wxFileName filename(path, wxEmptyString);
			
			Freeze();
			GetChildren(item, dir, filename.GetFullPath());
			Thaw();
			
		}
	}
}

CheckDirTreeCtrl::~CheckDirTreeCtrl()
{
}

void CheckDirTreeCtrl::OnItemUnchecked(wxCheckTreeCtrlEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		if(ItemHasChildren(item) && IsExpanded(item)) {
			//the item has children, but it is expanded, so check only this item
			Check(event.GetItem(), true);
		}
		else if(ItemHasChildren(item) && !IsExpanded(item)){
			RecursiveCheck(event.GetItem(), false);		
		}
	}
	event.Skip();
}

void CheckDirTreeCtrl::OnItemChecked(wxCheckTreeCtrlEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		if(ItemHasChildren(item) && IsExpanded(item)) {
			//the item has children, but it is expanded, so check only this item
			Check(event.GetItem(), true);
		}
		else if(ItemHasChildren(item) && !IsExpanded(item)){
			RecursiveCheck(event.GetItem());		
		}
	}
	event.Skip();
}

void CheckDirTreeCtrl::GetChildren(const wxTreeItemId &parent, const wxDir &dir, const wxString &path)
{
	wxString filename;
	if(!dir.IsOpened()){
		return;
	}

	//enumerate all directories
	bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);

	//add siblings
	while(cont){
		wxFileName dirname(path + wxT("/") + filename, wxEmptyString);
		wxTreeItemId item = AppendItem(parent, filename, IsChecked(parent), new DirTreeData(dirname));
		
		wxDir child(dirname.GetFullPath());
		if(child.IsOpened() && child.HasSubDirs()){
			//add dummy item under this node with the parent path
//			AppendItem(item, wxT("<Dummy>"), IsChecked(item), new DirTreeData(dirname));
			GetChildren(item, child, dirname.GetFullPath());
		}
		cont = dir.GetNext(&filename);
	}
}

void CheckDirTreeCtrl::OnItemExpading(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		//get the item data
		DirTreeData *data = dynamic_cast<DirTreeData*>(GetItemData(item));
		if(data){
			wxFileName filename = data->GetDir();
			wxTreeItemIdValue cookie;
			wxTreeItemId child = GetFirstChild(item, cookie);
			if(child.IsOk()){
				wxString text = GetItemText(child);
				if(text == wxT("<Dummy>")){
					//we have a dummy node, remove it and replace it with actual nodes
					Delete(child);
					//add all children of the parent dir
					AddChildren(item);
					Expand(item);
				}
			}
		}
	}
	event.Skip();
}

void CheckDirTreeCtrl::GetUnselectedDirs(wxArrayString &arr)
{
	//go over all loaded items, and return full path of the 
	//unselected items
	std::list<wxTreeItemId> items;
	GetItemChildrenRecursive(GetRootItem(), items);

	std::list<wxTreeItemId>::iterator iter = items.begin();
	for(; iter != items.end(); iter++){
		wxTreeItemId item = *iter;
		if(!IsChecked(item)){
			//get the tree item data
			DirTreeData *data = dynamic_cast<DirTreeData*>(GetItemData(item));
			if(data){
				wxString text = GetItemText(item);
				//skip the dummy items insertd into the tree
				if(item != wxT("<Dummy>")){
					arr.Add(data->GetDir().GetPath());
				}
			}
		}
	}
}

void CheckDirTreeCtrl::GetSelectedDirs(wxArrayString &arr)
{
	//go over all loaded items, and return full path of the 
	//selected items
	std::list<wxTreeItemId> items;
	GetItemChildrenRecursive(GetRootItem(), items);

	std::list<wxTreeItemId>::iterator iter = items.begin();
	for(; iter != items.end(); iter++){
		wxTreeItemId item = *iter;
		if(IsChecked(item)){
			//get the tree item data
			DirTreeData *data = dynamic_cast<DirTreeData*>(GetItemData(item));
			if(data){
				wxString text = GetItemText(item);
				//skip the dummy items insertd into the tree
				if(item != wxT("<Dummy>")){
					arr.Add(data->GetDir().GetPath());
				}
			}
		}
	}
}

