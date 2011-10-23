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
#include "localworkspace.h"
#include "editor_config.h"
#include "dirsaver.h"
#include "manager.h"
#include "ctags_manager.h"
#include "wx/xrc/xmlres.h"
#include "pluginmanager.h"
#include "environmentconfig.h"
#include "globals.h"
#include "dirsaver.h"
#include "procutils.h"
#include "frame.h"
#include <wx/mimetype.h>

BEGIN_EVENT_TABLE(FileExplorerTree, wxVirtualDirTreeCtrl)
	EVT_TREE_ITEM_MENU(wxID_ANY, FileExplorerTree::OnContextMenu)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, FileExplorerTree::OnItemActivated)
END_EVENT_TABLE()

FileExplorerTree::FileExplorerTree(wxWindow *parent, wxWindowID id)
		: wxVirtualDirTreeCtrl(parent, id)
		, m_rclickMenu(NULL)
{
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("file_explorer_menu"));
	Connect(XRCID("open_file"),                     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFile), NULL, this);
	Connect(XRCID("open_file_in_text_editor"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenFileInTextEditor), NULL, this);
	Connect(XRCID("refresh_node"),                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnRefreshNode), NULL, this);
	Connect(XRCID("delete_node"),                   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnDeleteNode), NULL, this);
    Connect(XRCID("search_node"),                   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnSearchNode), NULL, this);
    Connect(XRCID("tags_add_global_include"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_global_exclude"),       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_workspace_include"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnTagNode), NULL, this);
    Connect(XRCID("tags_add_workspace_exclude"),    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnTagNode), NULL, this);
	Connect(XRCID("open_shell"),                    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenShell), NULL, this);
	Connect(XRCID("open_with_default_application"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileExplorerTree::OnOpenWidthDefaultApp), NULL, this);
	Connect(GetId(),                                wxEVT_LEFT_DCLICK, wxMouseEventHandler( FileExplorerTree::OnMouseDblClick ) );
	Connect(GetId(),                                wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(FileExplorerTree::OnKeyDown));
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
	if (item.IsOk()) {
		wxString fp = GetFullPath(item).GetFullPath();
		VdtcTreeItemBase *b = (VdtcTreeItemBase *)GetItemData(item);

		if (b && b->IsDir()) {
			//////////////////////////////////////////////////
			// Remove a folder
			//////////////////////////////////////////////////

			wxString msg;
			msg << _("'") << GetItemText(item) << _("' is a directory. Are you sure you want to remove it and its content?");
			if (wxMessageBox(msg, _("Remove Directory"), wxICON_WARNING|wxYES_NO|wxCANCEL) == wxYES) {
				if (!RemoveDirectory(fp)) {
					wxMessageBox(_("Failed to remove directory"), _("Remove Directory"), wxICON_ERROR | wxOK);

				} else {
					wxTreeItemId parent = GetItemParent(item);
					if (parent.IsOk()) {
						// Select the parent, and call refresh.
						// by making the parent the selected item,
						// we force the refresh to take place on the parent node
						SelectItem(parent);
						wxCommandEvent dummy;
						OnRefreshNode(dummy);
					}
				}
			}
		} else {

			//////////////////////////////////////////////////
			// Remove a file
			//////////////////////////////////////////////////

			if (wxRemoveFile(fp)) {
				Delete(item);
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
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(ff);
    }
    e.Skip();
}

void FileExplorerTree::OnTagNode(wxCommandEvent &e)
{
	bool retagRequires (false);
    wxTreeItemId item = GetSelection();
    if (item.IsOk()) {

		TagsOptionsData tod = clMainFrame::Get()->GetTagsOptions();
        wxString path = GetFullPath(item).GetFullPath();
		if ( path.EndsWith(wxT("\\")) || path.EndsWith(wxT("/")) )  {
			path.RemoveLast();
		}

		if ( e.GetId() == XRCID("tags_add_global_include") ) {
			// add this directory as include path
			wxArrayString arr = tod.GetParserSearchPaths();
			if ( arr.Index( path ) == wxNOT_FOUND ) {
				arr.Add( path );
				tod.SetParserSearchPaths( arr );

				clMainFrame::Get()->UpdateTagsOptions( tod );
				retagRequires = true;
			}
		} else if ( e.GetId() == XRCID("tags_add_global_exclude") ){
			wxArrayString arr = tod.GetParserExcludePaths();
			if ( arr.Index(path) == wxNOT_FOUND ) {
				arr.Add( path );
				tod.SetParserExcludePaths( arr );

				clMainFrame::Get()->UpdateTagsOptions( tod );
				retagRequires = true;
			}
		} else if ( e.GetId() == XRCID("tags_add_workspace_include") ){
			wxArrayString includePaths, excludePaths;
			LocalWorkspaceST::Get()->GetParserPaths(includePaths, excludePaths);

			if ( includePaths.Index(path) == wxNOT_FOUND ) {
				includePaths.Add( path );
				// Write down the new paths
				LocalWorkspaceST::Get()->SetParserPaths(includePaths, excludePaths);
				// Update the parser
				ManagerST::Get()->UpdateParserPaths();
				retagRequires = true;
			}
		} else if ( e.GetId() == XRCID("tags_add_workspace_exclude") ){
			wxArrayString includePaths, excludePaths;
			LocalWorkspaceST::Get()->GetParserPaths(includePaths, excludePaths);

			if ( excludePaths.Index(path) == wxNOT_FOUND ) {
				excludePaths.Add( path );
				// Write down the new paths
				LocalWorkspaceST::Get()->SetParserPaths(includePaths, excludePaths);
				// Update the parser
				ManagerST::Get()->UpdateParserPaths();
				retagRequires = true;
			}
		}

		// send notification to the main frame to perform retag
		if ( retagRequires ) {
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace") );
			clMainFrame::Get()->GetEventHandler()->AddPendingEvent( event );
		}
	}
}

void FileExplorerTree::OnContextMenu(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		SelectItem(item);
		if (m_rclickMenu) {

			// let the plugins hook their content. HookPopupMenu can work only once
			// further calls are harmless
			PluginManager::Get()->HookPopupMenu(m_rclickMenu, MenuTypeFileExplorer);

            wxMenuItem *tagItem = m_rclickMenu->FindChildItem(XRCID("tag_node"));
            if (tagItem) {
                tagItem->Enable(IsDirNode(item));
            }

			PopupMenu(m_rclickMenu);
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

			// Send event to the plugins to see if any plugin want to handle this file differently
			wxString file_path = fn.GetFullPath();
			if (SendCmdEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED, &file_path)) {
				return;
			}

			clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString);
		}
	}
}

void FileExplorerTree::DoOpenItemInTextEditor(const wxTreeItemId &item)
{
	if (item.IsOk()) {
		wxFileName fn = GetFullPath(item);
		clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString);
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

	DoReloadNode(item);

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

		// Apply the environment before launching the console
		EnvSetter env;

		if (!ProcUtils::Shell()) {
			wxMessageBox(_("Failed to load shell terminal"), _("CodeLite"), wxICON_WARNING|wxOK);
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
		
#ifdef __WXGTK__
		// All hell break loose, try xdg-open
		wxString cmd = wxString::Format(wxT("xdg-open \"%s\""), fullpath.GetFullPath().c_str());
		wxExecute(cmd);
		return;
#endif

		// fallback code: suggest to the user to open the file with CL
		if (wxMessageBox(wxString::Format(_("Could not find default application for file '%s'\nWould you like CodeLite to open it?"), fullpath.GetFullName().c_str()), _("CodeLite"),
						 wxICON_QUESTION|wxYES_NO) == wxYES) {
			DoOpenItem( item );
		}
	}
}
