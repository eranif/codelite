#include "precompiled_header.h" 

#include "cpp_symbol_tree.h"
#include "manager.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>

IMPLEMENT_DYNAMIC_CLASS(CppSymbolTree, SymbolTree)

//----------------------------------------------------------------
// accessory function
//----------------------------------------------------------------
wxImageList* CreateSymbolTreeImages()
{
	wxImageList *images = new wxImageList(16, 16, true);

	images->Add(wxXmlResource::Get()->LoadBitmap(_T("project")));		   // 0
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("namespace")));	   // 1
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("globals")));		   // 2
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("class")));		   // 3
    images->Add(wxXmlResource::Get()->LoadBitmap(_T("struct")));		   // 4
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_public")));	   // 5
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_protected")));  // 6
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("func_private")));	   // 7
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_public")));   // 8
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_protected")));// 9
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("member_private")));  // 10
	
	wxBitmap bmp;

	// typedef
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));				   // 11
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	// macro (same icon as typedef)
	bmp = wxXmlResource::Get()->LoadBitmap(_T("typedef"));				   // 12
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(_T("enum"));				   // 13
	bmp.SetMask(new wxMask(bmp, wxColor(0, 128, 128)));
	images->Add(bmp);
	
	bmp = wxXmlResource::Get()->LoadBitmap(wxT("enumerator"));			//14
	images->Add(bmp);

	bmp = wxXmlResource::Get()->LoadBitmap(wxT("class_view"));			//15
	images->Add(bmp);
	return images;
}

CppSymbolTree::CppSymbolTree()
{
}

CppSymbolTree::CppSymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
: SymbolTree(parent, id, pos, size, style)
{
	Connect(GetId(), wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(CppSymbolTree::OnMouseRightUp));
	Connect(GetId(), wxEVT_LEFT_DCLICK, wxMouseEventHandler(CppSymbolTree::OnMouseDblClick));
	Connect(GetId(), wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(CppSymbolTree::OnItemActivated));
}

void CppSymbolTree::OnMouseRightUp(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();
	if(item.IsOk()){
		SelectItem(item, true);
	}
}

void CppSymbolTree::OnMouseDblClick(wxMouseEvent& event)
{
	//-----------------------------------------------------
	// We override the doubleclick on item event
	// to demonstrate how to access the tag information 
	// stored in the symbol tree
	//-----------------------------------------------------

	wxTreeItemId treeItem = GetSelection();
	if(!treeItem)
	{
		event.Skip();
		return;
	}

	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	if(HitTest(event.GetPosition(), flags) != treeItem)
	{
		event.Skip();
		return;
	}
	
	DoItemActivated(treeItem, event);
}

bool CppSymbolTree::ActivateSelectedItem()
{
	wxTreeItemId item = GetSelection();
	wxTreeEvent dummy;
	return DoItemActivated(item, dummy);
}

bool CppSymbolTree::DoItemActivated(wxTreeItemId item, wxEvent &event)
{
	//-----------------------------------------------------
	// Each tree items, keeps a private user data that 
	// holds the key for searching the its corresponding
	// node in the m_tree data structure
	//-----------------------------------------------------
	if(item.IsOk() == false)
		return false;

	MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(item));
	if( !itemData ){
		event.Skip();
		return false;
	}

	wxString filename = itemData->GetFileName();
	wxString project = ManagerST::Get()->GetProjectNameByFile(filename);
	int lineno = itemData->GetLineno()-1;
	
	// Open the file and set the cursor to line number
	ManagerST::Get()->OpenFile(filename, project, lineno);
	return true;
}

void CppSymbolTree::OnItemActivated(wxTreeEvent &event)
{
	if(event.GetKeyCode() == WXK_RETURN){
		wxTreeItemId item = GetSelection();
		DoItemActivated(item, event);
	}else{
		event.Skip();
	}
}

void CppSymbolTree::AdvanceSelection(bool forward){
	wxTreeItemId item = GetSelection();
	if (!item.IsOk()){
		return;
	}

	wxTreeItemId nextItem;
	if(forward){
		//Item is visible, scroll to it to make sure GetNextVisible() wont 
		//fail
		ScrollTo(item);
		nextItem = GetNextVisible(item);
	}else{
		nextItem = TryGetPrevItem(item);
	}

	if(nextItem.IsOk()){
		SelectItem(nextItem);
	}
}

wxTreeItemId CppSymbolTree::TryGetPrevItem(wxTreeItemId item)
{
	wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

	// find out the starting point
	wxTreeItemId prevItem = GetPrevSibling(item);
	if ( !prevItem.IsOk() )
	{
		prevItem = GetItemParent(item);
	}

	// from there we must be able to navigate until this item
	while ( prevItem.IsOk() )
	{
		ScrollTo(prevItem);
		const wxTreeItemId nextItem = GetNextVisible(prevItem);
		if ( !nextItem.IsOk() || nextItem == item )
			return prevItem;

		prevItem = nextItem;
	}

	return wxTreeItemId();
}
