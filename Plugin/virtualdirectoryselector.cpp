//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : virtualdirectoryselector.cpp
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

#include <wx/tokenzr.h>
#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include <deque>
#include "bitmap_loader.h"
#include "virtualdirectoryselector.h"
#include "tree_node.h"
#include <wx/imaglist.h>

VirtualDirectorySelector::VirtualDirectorySelector( wxWindow* parent, Workspace *wsp, const wxString &initialPath)
		: VirtualDirectorySelectorBase( parent )
		, m_workspace(wsp)
		, m_initialPath(initialPath)
{
	m_treeCtrl->SetFocus();
	DoBuildTree();
}

void VirtualDirectorySelector::OnItemSelected( wxTreeEvent& event )
{
	m_staticTextPreview->SetLabel(DoGetPath(m_treeCtrl, event.GetItem(), true));
}

void VirtualDirectorySelector::OnButtonOK( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void VirtualDirectorySelector::OnButtonCancel( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

wxString VirtualDirectorySelector::DoGetPath(wxTreeCtrl* tree, const wxTreeItemId& item, bool validateFolder)
{
	if (!item.IsOk()) {
		return wxEmptyString;
	}

	if(validateFolder) {
		int imgId = tree->GetItemImage(item);
		if (imgId != 1) { // not a virtual folder
			return wxEmptyString;
		}
	}

	std::deque<wxString> queue;
	wxString text = tree->GetItemText( item );
	queue.push_front( text );

	wxTreeItemId p = tree->GetItemParent( item );
	while ( p.IsOk() && p != tree->GetRootItem() ) {

		text = tree->GetItemText( p );
		queue.push_front( text );
		p = tree->GetItemParent( p );
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

void VirtualDirectorySelector::DoBuildTree()
{
	wxImageList *images = new wxImageList(16, 16);

	BitmapLoader bmpLoader;
	images->Add( bmpLoader.LoadBitmap( wxT( "workspace/16/workspace" ) ) );//0
	images->Add( bmpLoader.LoadBitmap( wxT( "workspace/16/virtual_folder" ) ) );    //1
	images->Add( bmpLoader.LoadBitmap( wxT( "workspace/16/project" ) ) );  //2
	m_treeCtrl->AssignImageList(images);

	if (m_workspace) {
		wxArrayString projects;
		m_workspace->GetProjectList(projects);

		VisualWorkspaceNode nodeData;
		nodeData.name = m_workspace->GetName();
		nodeData.type = ProjectItem::TypeWorkspace;

		TreeNode<wxString, VisualWorkspaceNode> *tree = new TreeNode<wxString, VisualWorkspaceNode>(m_workspace->GetName(), nodeData);

		for (size_t i=0; i<projects.GetCount(); i++) {
			wxString err;
			ProjectPtr p = m_workspace->FindProjectByName(projects.Item(i), err);
			if (p) {
				p->GetVirtualDirectories(tree);
			}
		}

		//create the tree
		wxTreeItemId root = m_treeCtrl->AddRoot(nodeData.name, 0, 0);
		tree->GetData().itemId = root;
		TreeWalker<wxString, VisualWorkspaceNode> walker(tree);

		for (; !walker.End(); walker++) {
			// Add the item to the tree
			TreeNode<wxString, VisualWorkspaceNode>* node = walker.GetNode();

			// Skip root node
			if (node->IsRoot())
				continue;

			wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
			if (parentHti.IsOk() == false) {
				parentHti = root;
			}

			int imgId(2); // Virtual folder
			switch (node->GetData().type) {
			case ProjectItem::TypeWorkspace:
				imgId = 0;
				break;
			case ProjectItem::TypeProject:
				imgId = 2;
				break;
			case ProjectItem::TypeVirtualDirectory:
			default:
				imgId = 1;
				break;

			}

			//add the item to the tree
			node->GetData().itemId = m_treeCtrl->AppendItem(
			                             parentHti,				// parent
			                             node->GetData().name,	// display name
			                             imgId,					// item image index
			                             imgId					// selected item image
			                         );
		}

		if (root.IsOk() && m_treeCtrl->HasChildren(root)) {
			m_treeCtrl->Expand(root);
		}
		delete tree;
	}

	// if a initialPath was provided, try to find and select it
	SelectPath(m_initialPath);
}

void VirtualDirectorySelector::OnButtonOkUI(wxUpdateUIEvent& event)
{
	wxTreeItemId id = m_treeCtrl->GetSelection();
	event.Enable(id.IsOk() && m_treeCtrl->GetItemImage(id) == 1);
}

bool VirtualDirectorySelector::SelectPath(const wxString& path)
{
	wxTreeItemId item = m_treeCtrl->GetRootItem();
	wxArrayString tokens = wxStringTokenize(path, wxT(":"), wxTOKEN_STRTOK);

	for (size_t i=0; i<tokens.GetCount(); i++) {
		if(item.IsOk() && m_treeCtrl->HasChildren(item)){

			// loop over the children of this node, and search for a match
			wxTreeItemIdValue cookie;
			wxTreeItemId child = m_treeCtrl->GetFirstChild(item, cookie);
			while(child.IsOk()){
				if(m_treeCtrl->GetItemText(child) == tokens.Item(i)){
					item = child;
					break;
				}
				child = m_treeCtrl->GetNextChild(child, cookie);
			}
		}
	}

	if(item.IsOk()){
		m_treeCtrl->EnsureVisible(item);
		m_treeCtrl->SelectItem(item);
		return true;
	}
	return false;
}
