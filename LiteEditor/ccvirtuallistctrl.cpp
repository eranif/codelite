#include "ccvirtuallistctrl.h"
CCVirtualListCtrl::CCVirtualListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxListCtrl(parent, id, pos, size, style)
{
}

CCVirtualListCtrl::~CCVirtualListCtrl()
{
}

wxListItemAttr* CCVirtualListCtrl::OnGetItemAttr(long item) const
{
	return NULL;
}

int CCVirtualListCtrl::OnGetItemColumnImage(long item, long column) const
{
	wxUnusedVar(column);
	return OnGetItemImage(item);
}

int CCVirtualListCtrl::OnGetItemImage(long item) const
{
	CCItemInfo info;
	if(item >= (long)m_tags.size()) {
		return wxNOT_FOUND;
	}
	info = m_tags.at(item);
	return info.imgId;
}

wxString CCVirtualListCtrl::OnGetItemText(long item, long column) const
{
	CCItemInfo info;
	if(item >= (long)m_tags.size()) {
		return wxEmptyString;
	}
	info = m_tags.at(item);
	return info.displayName;
}
