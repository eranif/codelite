//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : custom_notebook.h
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
#ifndef __Notebook__
#define __Notebook__

#include <wx/wx.h>
#include <wx/aui/auibook.h>

enum {
	wxVB_LEFT                   = wxAUI_NB_LEFT,
	wxVB_RIGHT                  = wxAUI_NB_RIGHT,
	wxVB_TOP                    = wxAUI_NB_TOP,
	wxVB_BOTTOM                 = wxAUI_NB_BOTTOM,
	wxVB_FIXED_WIDTH            = wxAUI_NB_TAB_FIXED_WIDTH,
	wxVB_HAS_X                  = wxAUI_NB_CLOSE_ON_ACTIVE_TAB,
	wxVB_MOUSE_MIDDLE_CLOSE_TAB = 0x00020000,
	wxVB_NODND                  = 0x00040000,
	wxVB_NO_TABS                = 0x00100000,
	wxVB_PASS_FOCUS             = 0x00400000
};

class NotebookNavDialog;
class wxMenu;

class Notebook : public wxAuiNotebook
{
	NotebookNavDialog *m_popupWin;
	wxMenu*            m_contextMenu;
	wxArrayPtrVoid     m_history;
	long               m_style;
	size_t             m_leftDownTabIdx;
	bool               m_notify;
	wxPoint            m_leftDownPos;

public:
	static const size_t npos = static_cast<size_t>(-1);

protected:
	void      Initialize();
	void      PushPageHistory(wxWindow *page);
	void      PopPageHistory(wxWindow *page);
	wxWindow* GetPreviousSelection();
	bool      HasCloseButton() {return m_style & wxVB_HAS_X;}
	bool      HasCloseMiddle() {return m_style & wxVB_MOUSE_MIDDLE_CLOSE_TAB;}

public:
	Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);
	virtual ~Notebook();

	/**
	 * \brief return the currently selected item index
	 * \return the currently selected item, of the book is empty, return Notebook::npos
	 */
	size_t GetSelection();

	/**
	 * \brief set page at given index to be the selected page. this function does not trigger an event
	 * \param page
	 */
	void SetSelection(size_t page, bool notify = false);

	/**
	 * \brief add page to the book
	 * \param win window to add, the window will be reparented to the book
	 * \param text page's caption
	 * \param selected set the page as the selected page
	 */
	bool AddPage(wxWindow *win, const wxString &text, bool selected = false, const wxBitmap &bmp = wxNullBitmap);
	bool InsertPage(size_t index, wxWindow *win, const wxString &text, bool selected = false, const wxBitmap &bmp = wxNullBitmap);

	/**
	 * \brief return page at give position
	 * \param page page's index
	 * \return the page or NULL if index is out of bounds
	 */
	wxWindow *GetPage(size_t page);

	/**
	 * \brief return the page caption
	 * \param page page index
	 * \return text or wxEmptyString if page index is invalid
	 */
	wxString GetPageText(size_t page) const;

	/**
	 * \brief remove page from the book without destroying it
	 * \param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 */
	bool RemovePage(size_t page, bool notify = true);

	/**
	 * \brief delete page from the book and destroy it as well
	 * \param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 */
	bool DeletePage(size_t page, bool notify = true);

	/**
	 * \brief delete all the pages in the notebook
	 */
	bool DeleteAllPages(bool notify = false);

	/**
	 * \brief return the tabbing history for this notebook
	 */
	const wxArrayPtrVoid& GetHistory() const;

	/**
	 *\param menu - right click menu object
	 */
	void SetRightClickMenu(wxMenu* menu);

	/**
	 * \brief return the active page
	 * \return active page or NULL if there are no pages in the book
	 */
	wxWindow *GetCurrentPage();

	/**
	 * \brief return page index by window pointer
	 * \param page page to search
	 * \return page index, or Notebook::npos if page does not exist in the notebook
	 */
	size_t GetPageIndex(wxWindow *page);

	/**
	 * \brief return page index by page text
	 * \param text text to search for
	 * \return page index, or Notebook::npos if page does not exist in the notebook
	 */
	size_t GetPageIndex(const wxString &text);

	/**
	 * \brief set the text for page at a given index
	 * \param index page's index
	 * \param text new text
	 */
	bool SetPageText(size_t index, const wxString &text);

protected:
	// Event handlers
	void OnNavigationKey      (wxNavigationKeyEvent &e);
	void OnKeyDown            (wxKeyEvent           &e);
	void OnTabRightDown       (wxAuiNotebookEvent   &e);
	void OnTabMiddle          (wxAuiNotebookEvent   &e);

	// wxAuiNotebook events
	void OnInternalPageChanged (wxAuiNotebookEvent &e);
	void OnInternalPageChanging(wxAuiNotebookEvent &e);
	void OnInternalPageClosing (wxAuiNotebookEvent &e);
	void OnInternalPageClosed  (wxAuiNotebookEvent &e);
	void OnBgDclick            (wxAuiNotebookEvent &e);

	void OnInternalMenu        (wxCommandEvent  &e);

	// wxChoicebook events
	void OnFocus                (wxFocusEvent      &e);

protected:
	void DoPageChangedEvent   (wxAuiNotebookEvent &e);
	void DoPageChangingEvent  (wxAuiNotebookEvent &e);
	bool DoNavigate();

};

class NotebookEvent : public wxNotifyEvent
{
	size_t sel, oldsel;

public:
	/**
	 * \param commandType - event type
	 * \param winid - window ID
	 * \param nSel - current selection
	 * \param nOldSel - old selection
	 */
	NotebookEvent(wxEventType commandType = wxEVT_NULL, int winid = 0, size_t nSel = (size_t)-1, size_t nOldSel = (size_t)-1)
			: wxNotifyEvent(commandType, winid), sel(nSel), oldsel(nOldSel) {}

	/**
	 * \param s - index of currently selected page
	 */
	void SetSelection(size_t s) {
		sel = s;
	}

	/**
	 * \param s - index of previously selected page
	 */
	void SetOldSelection(size_t s) {
		oldsel = s;
	}

	/// Returns the index of currently selected page
	size_t GetSelection() {
		return sel;
	}

	/// Returns the index of previously selected page
	size_t GetOldSelection() {
		return oldsel;
	}

	virtual wxEvent *Clone() const { return new NotebookEvent(*this); }
};

extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGED;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGING;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSING;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSED;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_X_CLICKED;
extern const wxEventType wxEVT_COMMAND_BOOK_BG_DCLICK;

typedef void (wxEvtHandler::*NotebookEventFunction)(NotebookEvent&);

#define NotebookEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(NotebookEventFunction, &func)

#define EVT_BOOK_PAGE_CHANGED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_CHANGED, winid, NotebookEventHandler(fn))

#define EVT_BOOK_PAGE_CHANGING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_CHANGING, winid, NotebookEventHandler(fn))

#define EVT_BOOK_PAGE_CLOSING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_CLOSING, winid, NotebookEventHandler(fn))

#define EVT_BOOK_PAGE_CLOSED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_CLOSED, winid, NotebookEventHandler(fn))

#define EVT_BOOK_PAGE_MIDDLE_CLICKED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED, winid, NotebookEventHandler(fn))

#define EVT_BOOK_PAGE_X_CLICKED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_PAGE_X_CLICKED, winid, NotebookEventHandler(fn))

#define EVT_BOOK_SWAP_PAGES(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_SWAP_PAGES, winid, NotebookEventHandler(fn))
	
#define EVT_BOOK_BG_DCLICK(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_BOOK_BG_DCLICK, winid, NotebookEventHandler(fn))
#endif // __Notebook__
