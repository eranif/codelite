//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : checktreectrl.h
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
 #ifndef CHECKTREECTRL_H
#define CHECKTREECTRL_H

#include <wx/treectrl.h>
#include <wx/bitmap.h>
#include "list"

class wxCheckTreeCtrl : public wxTreeCtrl
{
	wxBitmap m_checkedBmp;
	wxBitmap m_uncheckedBmp;

protected:
	void GetItemChildrenRecursive(const wxTreeItemId &parent, std::list<wxTreeItemId> &children);

public:
	wxCheckTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("treeCtrl"));
	virtual ~wxCheckTreeCtrl();

	//override item managements functions
	virtual wxTreeItemId AddRoot(const wxString& text, bool checked, wxTreeItemData* data = NULL);
	virtual wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data = NULL);

	bool IsChecked(const wxTreeItemId& item) const;
	virtual void Check(const wxTreeItemId& item, bool check = true);
	void RecursiveCheck(const wxTreeItemId &item, bool check = true);

	DECLARE_EVENT_TABLE();
	virtual void OnLeftDown(wxMouseEvent &event);
};

/**
 * \brief Holds information about events associated with wxCheckTreeCtrl objects
 * This event is fired when an item is (un)selected. use the:
 * \code
 * EVT_CHECKTREE_ITEM_SELECTED() //wxEVT_CKTR_ITEM_SELECTED
 * EVT_CHECKTREE_ITEM_UNSELECTED() //wxEVT_CKTR_ITEM_UNSELECTED
 * \code
 * macros to handle them.
 */
class wxCheckTreeCtrlEvent : public wxNotifyEvent
{
	wxTreeItemId m_item;

public:
	/**
	 * \param commandType - event type
	 * \param winid - window ID
	 */
	wxCheckTreeCtrlEvent(wxEventType commandType = wxEVT_NULL, int winid = 0) : wxNotifyEvent(commandType, winid){}
	virtual ~wxCheckTreeCtrlEvent(){}

	void SetItem(const wxTreeItemId &item) { m_item = item; }
	const wxTreeItemId& GetItem() const{ return m_item; }
};

extern const wxEventType wxEVT_CKTR_ITEM_UNSELECTED;
extern const wxEventType wxEVT_CKTR_ITEM_SELECTED;

typedef void (wxEvtHandler::*wxCheckTreeCtrlEventFunction)(wxCheckTreeCtrlEvent&);

#define wxCheckTreeCtrlEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCheckTreeCtrlEventFunction, &func)

#define EVT_CHECKTREE_ITEM_SELECTED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_CKTR_ITEM_SELECTED, winid, wxCheckTreeCtrlEventHandler(fn))

#define EVT_CHECKTREE_ITEM_UNSELECTED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_CKTR_ITEM_UNSELECTED, winid, wxCheckTreeCtrlEventHandler(fn))

#endif //CHECKTREECTRL_H

