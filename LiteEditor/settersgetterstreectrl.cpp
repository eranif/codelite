#include "settersgetterstreectrl.h"
#include <wx/xrc/xmlres.h>
#include <wx/imaglist.h>

// -------------------------------------------------------
// Images
/* XPM */
static const char *Checkbox_off_XPM[] = {
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
static const char *Checkbox_on_XPM[] = {
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

/* XPM */
static const char * check_box_disabled_xpm[] = {
"16 16 10 1",
" 	c None",
".	c #185284",
"+	c #E7E7DE",
"@	c #E7E7E7",
"#	c #EFEFE7",
"$	c #EFEFEF",
"%	c #F7F7EF",
"&	c #C3C3C3",
"*	c #F7F7F7",
"=	c #FFFFFF",
"                ",
"................",
".+++++++@##$%++.",
".+++++@@##$%$++.",
".++&&&&&&&&&&++.",
".++&&&&&&&&&&++.",
".++&&&&&&&&&&++.",
".++&&&&&&&&&&++.",
".+@&&&&&&&&&&++.",
".+#&&&&&&&&&&++.",
".+#&&&&&&&&&&++.",
".+$&&&&&&&&&&++.",
".+%%***==*===++.",
".++++++++++++++.",
"................",
"                "};
//----------------------------------------------

BEGIN_EVENT_TABLE(SettersGettersTreeCtrl, wxCheckTreeCtrl)
	EVT_CHECKTREE_ITEM_UNSELECTED (wxID_ANY, SettersGettersTreeCtrl::OnItemUnchecked)
	EVT_CHECKTREE_ITEM_SELECTED   (wxID_ANY, SettersGettersTreeCtrl::OnItemChecked  )
END_EVENT_TABLE()

SettersGettersTreeCtrl::SettersGettersTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
		: wxCheckTreeCtrl(parent, id, pos, size, style, validator, name)
{
	// Replace the default list image

	//create an image list and assign it
	wxImageList *il = new wxImageList(16, 16, true);
	il->Add(wxBitmap(Checkbox_on_XPM));                              // 0
	il->Add(wxBitmap(Checkbox_off_XPM));                             // 1
	il->Add(wxXmlResource::Get()->LoadBitmap(wxT("member_public"))); // 2
	il->Add(wxBitmap(check_box_disabled_xpm));                       // 3
	//will be owned by the control
	AssignImageList(il);
}

SettersGettersTreeCtrl::~SettersGettersTreeCtrl()
{
}

wxTreeItemId SettersGettersTreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data)
{
	if ( data ) {
		SettersGettersTreeData* d = dynamic_cast<SettersGettersTreeData*>( data );

		if ( d ) {
			if ( d->m_kind == SettersGettersTreeData::Kind_Parent ) {
				return wxTreeCtrl::AppendItem(parent, text, 2, 2, data);
			} else if ( d->m_disabled ) {
				return wxTreeCtrl::AppendItem(parent, text, 3, 3, data);
			}
		}
	}
	return wxCheckTreeCtrl::AppendItem(parent, text, checked, data);
}

wxTreeItemId SettersGettersTreeCtrl::AddRoot(const wxString& text, bool checked, wxTreeItemData* data)
{
	wxUnusedVar(checked);
	return wxTreeCtrl::AddRoot(text, wxNOT_FOUND, wxNOT_FOUND, data);
}

void SettersGettersTreeCtrl::OnItemUnchecked(wxCheckTreeCtrlEvent& e)
{
	wxTreeItemId item = e.GetItem();
	if (item.IsOk()) {
		wxTreeItemData *data = this->GetItemData(item);
		if ( data ) {
			SettersGettersTreeData* d = dynamic_cast<SettersGettersTreeData*>( data );
			if ( d ) {
				if ( d->m_disabled ) {
					e.Veto();
				} else {
					switch (d->m_kind) {
					case SettersGettersTreeData::Kind_Root:
					case SettersGettersTreeData::Kind_Parent:
						e.Veto();
						break;
					}
				}
			}
		}
		e.Skip();
	}
}

void SettersGettersTreeCtrl::OnItemChecked(wxCheckTreeCtrlEvent& e)
{
	wxTreeItemId item = e.GetItem();
	if (item.IsOk()) {
		wxTreeItemData *data = this->GetItemData(item);
		if ( data ) {
			SettersGettersTreeData* d = dynamic_cast<SettersGettersTreeData*>( data );
			if ( d ) {
				if ( d->m_disabled ) {
					e.Veto();
				} else {
					switch (d->m_kind) {
					case SettersGettersTreeData::Kind_Root:
					case SettersGettersTreeData::Kind_Parent:
						e.Veto();
						break;
					}
				}
			}
		}
		e.Skip();
	}
}
