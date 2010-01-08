//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custom_tabcontainer.h
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
#ifndef __wxtabcontainer__
#define __wxtabcontainer__

#include "wx/panel.h"
#include <wx/dynarray.h>
#include "dropbutton.h"

class CustomTab;
class wxMenu;

class wxTabContainer : public wxPanel
{
	int            m_orientation;
	wxSizer*       m_tabsSizer;
	CustomTab*     m_draggedTab;
	wxArrayPtrVoid m_history;
	wxMenu *       m_rightClickMenu;
	int            m_bmpHeight;
	size_t         m_fixedTabWidth;

	friend class CustomTab;

protected:
	void Initialize();

	//add page to history
	void PushPageHistory(CustomTab *page);

	//remove page from the history by its value
	//after the page removal, all items in the history
	//are updated if needed
	void PopPageHistory(CustomTab *page);

	bool DoRemoveTab(CustomTab *deleteTab, bool deleteIt, bool notify = false);

	void EnsureVisible(CustomTab *tab);

	bool IsVisible(CustomTab *tab, bool fullShown = true);

	/**
	 * @brief return the index of the first visible tab
	 */
	size_t GetFirstVisibleTab();

	/**
	 * @brief return the index of the last visible tab
	 */
	size_t GetLastVisibleTab();

	/**
	 * @brief show maximum tabs as possible.
	 */
	void DoShowMaxTabs();

public:
	wxTabContainer(wxWindow *win, wxWindowID id = wxID_ANY, int orientation = wxLEFT, long style = 0);
	virtual ~wxTabContainer();

	void AddTab(CustomTab *tab);
	void InsertTab(CustomTab *tab, size_t index);
	CustomTab *GetSelection();

	void SetSelection(CustomTab *tab, bool notify = false);
	CustomTab *IndexToTab(size_t page);
	size_t TabToIndex(CustomTab *tab);
	size_t GetTabsCount();

	void SetRightClickMenu(wxMenu* rightClickMenu) {
		this->m_rightClickMenu = rightClickMenu;
	}
	wxMenu* GetRightClickMenu() {
		return m_rightClickMenu;
	}

	void Resize();

	/**
	 * @brief delete tab from the tab container (also destroying it)
	 * @param deleteTab tab to delete
	 * @param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 * @return true if notification sent but vetoed by user, else false
	 */
	bool DeletePage(CustomTab *deleteTab, bool notify);

	/**
	 * @brief remove tab from the tab container (the tab will *not* be destroyed)
	 * @param removePage tab to remove
	 * @param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 * @return true if notification sent but vetoed by user, else false
	 */
	bool RemovePage(CustomTab *removePage, bool notify);

	/**
	 * @brief return previous selection
	 * @return previous selection, or NULL if history list is empty
	 */
	CustomTab* GetPreviousSelection();

	/**
	 * @brief set tab as being dragged
	 * @param tab
	 */
	void SetDraggedTab(CustomTab *tab);

	/**
	 * @brief swap tab with the dragged one
	 * @param tab swap dragged tab this one
	 */
	void SwapTabs(CustomTab *tab);

	/**
	 * @brief get tab being dragged, if any
	 * @return tab being dragged
	 */
	const CustomTab* GetDraggedTab() const {
		return m_draggedTab;
	}

	/**
	 * @brief set the fixed tab width
	 * @param size in pixels
	 */
	void SetFixedTabWidth(const size_t& fixedTabWidth);

	/**
	 * @brief return the fixed tab width in pixels
	 */
	size_t GetFixedTabWidth() const {
		return m_fixedTabWidth;
	}

	int GetBookStyle();
	void SetOrientation(const int& orientation) ;
	const int& GetOrientation() const {
		return m_orientation;
	}
	void ShowPopupMenu();
	void SetHistory(const wxArrayPtrVoid& history) {
		this->m_history = history;
	}
	const wxArrayPtrVoid& GetHistory() const {
		return m_history;
	}
	void SetBmpHeight(int height);
	int GetBmpHeight() const;
	static void DoDrawBackground(wxDC &dc, bool gradient, int orientation, const wxRect &rr);
	static void DoDrawMargin(wxDC &dc, int orientation, const wxRect &rr);

	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSizeEvent(wxSizeEvent &e);
	virtual void OnLeaveWindow(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnDeleteTab(wxCommandEvent &e);
	virtual void OnDoubleClick(wxMouseEvent &e);


};

//--------------------------------------------------------------------
// Dropbutton class used for the notebook
//--------------------------------------------------------------------

class DropButton : public DropButtonBase
{
private:
	wxTabContainer *m_tabContainer;

protected:
	size_t GetItemCount();
	wxString GetItem(size_t n);
	bool IsItemSelected(size_t n);

public:
	DropButton(wxWindow *parent, wxTabContainer *tabContainer);
	~DropButton();

	virtual void OnMenuSelection(wxCommandEvent &e);
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnLeftDown(wxMouseEvent &e);
};


#endif // __wxtabcontainer__
