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
#include "fileexplorertree.h"
#include "dirsaver.h"
#include "manager.h"
#include "ctags_manager.h"
#include "wx/xrc/xmlres.h"
#include "pluginmanager.h"
#include "globals.h"
#include "dirsaver.h"
#include "procutils.h"
#include "frame.h"
#include <wx/mimetype.h>

BEGIN_EVENT_TABLE(FileExplorerTree, wxVirtualDirTreeCtrl)
	EVT_TREE_ITEM_MENU(wxID_ANY, FileExplorerTree::OnContextMenu)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, FileExplorerTree::OnItemActivated)
	EVT_TREE_ITEM_EXPANDED(wxID_ANY, FileExplorerTree::OnExpanded)
END_EVENT_TABLE()

FileExplorerTree::FileExplorerTree(wxWindow *parent, wxWindowID id)
		: wxVirtualDirTreeCtrl(parent, id)
		, m_rclickMenu(NULL)
		, m_itemsAdded(false)
{
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("file_explorer_menu"));
	Connect(XRCID("open_file"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFile), NULL, this);
	Connect(XRCID("open_file_in_text_editor"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFileInTextEditor), NULL, this);
	Connect(XRCID("refresh_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnRefreshNode), NULL, this);
	Connect(XRCID("delete_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnDeleteNode), NULL, this);
    Connect(XRCID("search_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnSearchNode), NULL, this);
    Connect(XRCID("tag_node"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnTagNode), NULL, this);
	Connect(XRCID("open_shell"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenShell), NULL, this);
	Connect(XRCID("open_with_default_application"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenWidthDefaultApp), NULL, this);
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler( FileExplorerTree::OnMouseDblClick ) );
	Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileExplorerTree::OnKeyDown));
	TagsManagerST::Get()->Connect(wxEVT_UPDATE_FILETREE_EVENT, wxCommandEventHandler(FileExplorerTree::OnTagsUpdated), NULL, this);
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
	if (e.GetKeyCode() == WXK_RETURN || e.GetKeyCode() == WXK_NUMPAD_ENTER) {
		wxTreeItemId item = GetSelection();
		DoItemActivated(item);
	} else if (e.GetKeyCode() == WXK_DELETE || e.GetKeyCode() == WXK_NUMPAD_DELETE) {
		wxCommandEvent dummy;
		OnDeleteNode(dummy);
	} else {
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
					wxMessageBox(_("Failed to remove directory"), wxT("Remove Directory"), wxICON_ERROR | wxOK);
				} else {
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
			if (parent.IsOk()) {
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

void FileExplorerTree::OnSearchNode(wxCommandEvent &e)
{
    wxTreeItemId item = GetSelection();
    if (item.IsOk()) {
        wxCommandEvent ff(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_in_files"));
        ff.SetString(GetFullPath(item).GetFullPath());
        Frame::Get()->AddPendingEvent(ff);
    }
    e.Skip();
}

void FileExplorerTree::OnTagNode(wxCommandEvent &e)
{
    wxTreeItemId item = GetSelection();
    if (item.IsOk()) {
        wxString path = GetFullPath(item).GetFullPath();
        wxCommandEvent td(wxEVT_COMMAND_MENU_SELECTED, XRCID("create_ext_database"));
        td.SetString(path);
        Frame::Get()->AddPendingEvent(td);
    }
}

void FileExplorerTree::OnContextMenu(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		SelectItem(item);
		if (m_rclickMenu) {
            wxMenuItem *tagItem = m_rclickMenu->FindChildItem(XRCID("tag_node"));
            if (tagItem) {
                tagItem->Enable(IsDirNode(item));
            }
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

void FileExplorerTree::OnMouseDblClick( wxMouseEvent &event )
{
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
			if (IsExpanded(item)) {
				Collapse(item);
			} else {
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
	wxFileName   path = GetFullPath(item); // save here because item gets clobbered by DoReloadNode()
    
	Freeze();
    
	m_itemsAdded = false;
	DoReloadNode(item);
	if (m_itemsAdded) { // check if new items need to be bolded
		if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD) {
			std::vector<wxFileName> files;
			TagsManagerST::Get()->GetFiles(GetFullPath(item).GetFullPath(), files);
			DoTagsUpdated(files, true);
		}
		m_itemsAdded = false;
	}
    
	Thaw();
}

void FileExplorerTree::OnOpenShell(wxCommandEvent &event)
{
	DirSaver ds;
	wxTreeItemId item = GetSelection();
	if (item.IsOk()) {
		wxFileName fullpath = GetFullPath(item);
		
		DirSaver ds;
		wxSetWorkingDirectory(fullpath.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));
		
		if (!ProcUtils::Shell()) {
			wxMessageBox(_("Failed to load shell terminal"), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}
	}
}

void FileExplorerTree::OnOpenWidthDefaultApp(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxTreeItemId item = GetSelection();
	if (item.IsOk()) {
		wxFileName fullpath = GetFullPath(item);
		wxMimeTypesManager *mgr = wxTheMimeTypesManager;
		wxFileType *type = mgr->GetFileTypeFromExtension(fullpath.GetExt());
		if ( type ) {
			wxString cmd = type->GetOpenCommand(fullpath.GetFullPath());
			delete type;

			if ( cmd.IsEmpty() == false ) {
				wxExecute(cmd);
				return;
			}
		}
		
		// fallback code: suggest to the user to open the file with CL
		if (wxMessageBox(wxString::Format(wxT("Could not find default application for file '%s'\nWould you like CodeLite to open it?"), fullpath.GetFullName().c_str()), wxT("CodeLite"),
						 wxICON_QUESTION|wxYES_NO) == wxYES) {
			DoOpenItem( item );
		}
	}
}

void FileExplorerTree::OnExpanded(wxTreeEvent &event)
{
    wxTreeItemId item = event.GetItem();
    if (item.IsOk() && m_itemsAdded) { // check if new items need to be bolded
		if( TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD ){
			std::vector<wxFileName> files;
			TagsManagerST::Get()->GetFiles(GetFullPath(item).GetFullPath(), files);
			DoTagsUpdated(files, true);
		}
        m_itemsAdded = false;
    }
}

void FileExplorerTree::OnAddedItems(const wxTreeItemId &parent)
{
    m_itemsAdded = true;
}

void FileExplorerTree::OnTagsUpdated(wxCommandEvent &e)
{
	std::vector<wxFileName> *files = (std::vector<wxFileName>*)e.GetClientData();
	if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_MARK_TAGS_FILES_IN_BOLD){
		DoTagsUpdated(*files, e.GetInt() ? true : false);
	}
}

// Mark files (and their directories) bold in file explorer
// Intended to highlight files that contribute tags.
void FileExplorerTree::DoTagsUpdated(const std::vector<wxFileName>& files, bool bold)
{
    // we use this map as a queue of nodes to check, where nodes are guaranteed to be sorted
    // by full pathname, and no duplicates will occur
    std::map<wxString, std::pair<wxTreeItemId, bool> > nodes;
	
	// insert the list of files in the map
    for (unsigned i = 0; i < files.size(); i++) {
        wxTreeItemId id = GetItemByFullPath(files[i], false);
        if (id.IsOk())
            // note: use GetFullPath(id) to get the path, rather than files[i], in case 
            // the returned node is an ancestor dir of the file (this happens when the file 
            // explorer tree is not fully expanded, which is most of the time).
            nodes[GetFullPath(id).GetFullPath()] = std::make_pair(id, bold);
    }
    if (nodes.empty())
        return;
        
    Freeze();
    // now we process the queue.  first, note we traverse the map in reverse.  this way we 
    // always process all children of a node before we process the node itself.  the 'bold'
    // flag of a given directory is just the logical-or of all the bold flags of its children
    // but we want to compute this lazily.
    for (std::map<wxString, std::pair<wxTreeItemId, bool> >::reverse_iterator n = nodes.rbegin();
             n != nodes.rend(); n++) {
        wxTreeItemId id = n->second.first;
        bold = n->second.second;
        if (!bold && IsDirNode(id)) {
            // need to make sure no other child of directory is still bold before we can clear it
            wxTreeItemIdValue cookie = 0;
            for (wxTreeItemId ch = GetFirstChild(id, cookie); ch.IsOk(); ch = GetNextChild(id, cookie))
                if (IsBold(ch)) {
                    bold = true;
                    break;
                }
        }
        if (IsBold(id) == bold) {
            // this node's bold flag is still the same, so we can short-circuit the upward propagation
            // from this node.
            continue;
        }
        // update the node's flag, and propagate upwards.
        SetItemBold(id, bold);
        id = GetItemParent(id);
        if (id.IsOk()) {
            wxString path = GetFullPath(id).GetFullPath();
            // here's the logical-or part, at least for the paths that we are explicitly processing:
            nodes.insert(std::make_pair(path, std::make_pair(id, false))).first->second.second |= bold;
        }
    }
    Thaw();	
}
