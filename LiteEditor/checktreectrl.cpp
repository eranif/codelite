//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : checktreectrl.cpp              
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
 #include "checktreectrl.h"
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/imaglist.h>
#include <wx/settings.h>

// Images used by the checktreectrl

/* XPM */
static const char *Checkbox_off_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 11 1",
"$ c #F7F7F7",
"% c #FFFFFF",
". c #E7E7DE",
"  c #185284",
"X c #DEDEDE",
"o c #E7E7DE",
"@ c #E7E7E7",
"O c #EFEFE7",
"+ c #EFEFEF",
"# c #F7F7EF",
"& c None",
/* pixels */
"&&&&&&&&&&&&&&&&",
"                ",
" ......XooOO+.. ",
" ....XXo@O++#.. ",
" ...XXo@O+++#.. ",
" ..Xoo@@++#$$.. ",
" ..Xo@O++#+$$.. ",
" .oo@@+++$$$$.. ",
" .o@O++#+$$%%.. ",
" .oO+++$$$$%%.. ",
" .O++#$#$$%%%.. ",
" .O++$$$$%%%%.. ",
" .+#$#$$%%%%%.. ",
" .............. ",
"                ",
"&&&&&&&&&&&&&&&&"
};


/* XPM */
static const char *Checkbox_on_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 10 1",
"# c #F7F7F7",
"$ c #FFFFFF",
"@ c #21A521",
"  c #185284",
". c #E7E7DE",
"X c #E7E7E7",
"o c #EFEFE7",
"O c #EFEFEF",
"+ c #F7F7EF",
"& c None",
/* pixels */
"&&&&&&&&&&&&&&&&",
"                ",
" .......XooO+.. ",
" .....XXooO+O.. ",
" ....XXXOO@+#.. ",
" ...XXXoO@@##.. ",
" ...@XOO@@@##.. ",
" ..X@@O@@@##$.. ",
" .Xo@@@@@##$#.. ",
" .ooO@@@##$$$.. ",
" .oO+O@##$#$$.. ",
" .O+O###$$$$$.. ",
" .++###$$#$$$.. ",
" .............. ",
"                ",
"&&&&&&&&&&&&&&&&"
};


//-----------------------------------------------------------
// wxCheckTreeCtrl
//-----------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_CKTR_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_CKTR_ITEM_UNSELECTED)

BEGIN_EVENT_TABLE(wxCheckTreeCtrl, wxTreeCtrl)
EVT_LEFT_DOWN(wxCheckTreeCtrl::OnLeftDown)
EVT_LEFT_DCLICK(wxCheckTreeCtrl::OnLeftDown)
END_EVENT_TABLE()

wxCheckTreeCtrl::wxCheckTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
: m_checkedBmp(16, 16)
, m_uncheckedBmp(16, 16)
{
	Create(parent, id, pos, size, style | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT, validator, name);
	//create images
	m_checkedBmp = wxBitmap(Checkbox_on_xpm);
	m_uncheckedBmp = wxBitmap(Checkbox_off_xpm);

	//create an image list and assign it
	wxImageList *il = new wxImageList(16, 16, true);
	il->Add(m_checkedBmp);		//0
	il->Add(m_uncheckedBmp);	//1

	//will be owned by the control
	AssignImageList(il);
}

wxCheckTreeCtrl::~wxCheckTreeCtrl()
{
}

wxTreeItemId wxCheckTreeCtrl::AddRoot(const wxString& text, bool checked, wxTreeItemData* data)
{
	return wxTreeCtrl::AddRoot(text, checked ? 0 : 1, checked ? 0 : 1, data);
}

wxTreeItemId wxCheckTreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data)
{
	return wxTreeCtrl::AppendItem(parent, text, checked ? 0 : 1, checked ? 0 : 1, data);
}

void wxCheckTreeCtrl::OnLeftDown(wxMouseEvent &event)
{
	int flags;
	wxTreeItemId item = wxTreeCtrl::HitTest(event.GetPosition(), flags);
	if(item.IsOk() && flags & wxTREE_HITTEST_ONITEMICON){
		if(IsChecked(item)){
			//fire unselect event
			wxCheckTreeCtrlEvent e(wxEVT_CKTR_ITEM_UNSELECTED, GetId());
			e.SetItem(item);
			e.SetEventObject(this);
			GetEventHandler()->ProcessEvent(e);
		
			//Veto?
			if(!e.IsAllowed()){
				return;
			}
			Check(item, false);

		}else{
			//fire select event
			wxCheckTreeCtrlEvent e(wxEVT_CKTR_ITEM_SELECTED, GetId());
			e.SetItem(item);
			e.SetEventObject(this);
			GetEventHandler()->ProcessEvent(e);
		
			//Veto?
			if(!e.IsAllowed()){
				return;
			}
			Check(item, true);
		}
		return;
	}
	event.Skip();
}

bool wxCheckTreeCtrl::IsChecked(const wxTreeItemId &item) const
{
	int imgId = GetItemImage(item);
	return imgId == 0;
}

void wxCheckTreeCtrl::Check(const wxTreeItemId &item, bool check)
{
	if(check){
		SetItemImage(item, 0);
		SetItemImage(item, 0, wxTreeItemIcon_Selected);
	}else{
		SetItemImage(item, 1);
		SetItemImage(item, 1, wxTreeItemIcon_Selected);
	}
}

void wxCheckTreeCtrl::GetItemChildrenRecursive(const wxTreeItemId &parent, std::list<wxTreeItemId> &children)
{
	//delete the item's children
	wxTreeItemIdValue cookie;
	children.push_back(parent);
	wxTreeItemId child = GetFirstChild(parent, cookie);
	while(child.IsOk())
	{
		if(ItemHasChildren(child)){
			GetItemChildrenRecursive(child, children);
		}else{
			children.push_back(child);
		}
		child = GetNextChild(parent, cookie);
	}
}

void wxCheckTreeCtrl::RecursiveCheck(const wxTreeItemId &item, bool check)
{
	std::list<wxTreeItemId> children;
	GetItemChildrenRecursive(item, children);

	std::list<wxTreeItemId>::iterator iter = children.begin();
	for(; iter != children.end(); iter++)
	{
		Check((*iter), check);
	}
}
