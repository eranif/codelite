//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cpp_symbol_tree.cpp
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
#include "precompiled_header.h"
#include "globals.h"
#include "stringsearcher.h"
#include "stringsearcher.h"
//#include "cl_editor.h"
//#include "pluginmanager.h"

#include "sv_symbol_tree.h"
//#include "manager.h"
//#include "frame.h"
#include "bitmap_loader.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>

IMPLEMENT_DYNAMIC_CLASS(svSymbolTree, SymbolTree)

const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED = wxNewEventType();

//----------------------------------------------------------------
// accessory function
//----------------------------------------------------------------
wxImageList* svSymbolTree::CreateSymbolTreeImages()
{
	wxImageList *images = new wxImageList(16, 16, true);

	BitmapLoader *bmpLoader = new BitmapLoader();
	images->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));               // 0
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/namespace")));           // 1
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/globals")));             // 2
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/class")));               // 3
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/struct")));              // 4
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_public")));     // 5
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_protected")));  // 6
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/function_private")));    // 7
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_public")));       // 8
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_protected")));    // 9
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/member_private")));      // 10
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/typedef")));             // 11
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/macro")));               // 12
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/enum")));                // 13
	images->Add(bmpLoader->LoadBitmap(wxT("cc/16/enumerator")));          // 14
	images->Add(bmpLoader->LoadBitmap(wxT("mime/16/cpp")));               // 15
	return images;
}

svSymbolTree::svSymbolTree()
{
}

svSymbolTree::svSymbolTree(wxWindow *parent, IManager* manager, const wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
		: SymbolTree(parent, id, pos, size, style)
{
    m_manager = manager;
	Connect(GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(svSymbolTree::OnMouseRightUp));
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(svSymbolTree::OnMouseDblClick));
	Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(svSymbolTree::OnItemActivated));
	MSWSetNativeTheme(this);
}

void svSymbolTree::OnMouseRightUp(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if (item.IsOk()) {
		SelectItem(item, true);
	}
}

void svSymbolTree::OnMouseDblClick(wxMouseEvent& event)
{
	//-----------------------------------------------------
	// We override the doubleclick on item event
	// to demonstrate how to access the tag information
	// stored in the symbol tree
	//-----------------------------------------------------

	wxTreeItemId treeItem = GetSelection();
	if (!treeItem) {
		event.Skip();
		return;
	}

	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	if (HitTest(event.GetPosition(), flags) != treeItem) {
		event.Skip();
		return;
	}

	DoItemActivated(treeItem, event, true);
}

bool svSymbolTree::ActivateSelectedItem()
{
	wxTreeItemId item = GetSelection();
	wxTreeEvent dummy;
	return DoItemActivated(item, dummy, true);
}

bool svSymbolTree::DoItemActivated(wxTreeItemId item, wxEvent &event, bool notify)
{
	//-----------------------------------------------------
	// Each tree items, keeps a private user data that
	// holds the key for searching the its corresponding
	// node in the m_tree data structure
	//-----------------------------------------------------
	if (item.IsOk() == false)
		return false;

	MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(item));
	if ( !itemData ) {
		event.Skip();
		return false;
	}

	wxString filename = itemData->GetFileName();
	wxString project = m_manager->GetWorkspace()->GetActiveProjectName();
	wxString pattern = itemData->GetPattern();
    int      lineno  = itemData->GetLine();

/*
	// Open the file and set the cursor to line number
	if(clMainFrame::Get()->GetMainBook()->OpenFile(filename, project, lineno-1)) {
		// get the editor, and search for the pattern in the file
		LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
		if (editor) {
			FindAndSelect(editor, pattern, GetItemText(item));
		}
	}
*/
    FindAndSelect(m_manager->GetActiveEditor(), pattern, GetItemText(item));
    
	// post an event that an item was activated
	if ( notify ) {
		wxCommandEvent e(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED);
		e.SetEventObject(this);
		wxPostEvent(GetEventHandler(), e);
	}
	return true;
}

void svSymbolTree::OnItemActivated(wxTreeEvent &event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		wxTreeItemId item = GetSelection();
		DoItemActivated(item, event, true);
	} else {
		event.Skip();
	}
}

void svSymbolTree::AdvanceSelection(bool forward)
{
	wxTreeItemId item = GetSelection();
	if (!item.IsOk()) {
		return;
	}

	wxTreeItemId nextItem;
	if (forward) {
		//Item is visible, scroll to it to make sure GetNextVisible() wont
		//fail
		ScrollTo(item);
		nextItem = GetNextVisible(item);
	} else {
		nextItem = TryGetPrevItem(item);
	}

	if (nextItem.IsOk()) {
		SelectItem(nextItem);
	}
}

wxTreeItemId svSymbolTree::TryGetPrevItem(wxTreeItemId item)
{
	wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

	// find out the starting point
	wxTreeItemId prevItem = GetPrevSibling(item);
	if ( !prevItem.IsOk() ) {
		prevItem = GetItemParent(item);
	}

	// from there we must be able to navigate until this item
	while ( prevItem.IsOk() ) {
		ScrollTo(prevItem);
		const wxTreeItemId nextItem = GetNextVisible(prevItem);
		if ( !nextItem.IsOk() || nextItem == item )
			return prevItem;

		prevItem = nextItem;
	}

	return wxTreeItemId();
}

void svSymbolTree::FindAndSelect(IEditor* editor, wxString& pattern, const wxString& name)
{
	editor->FindAndSelect(pattern, name, 0 /* from pos */, m_manager->GetNavigationMgr());
}
