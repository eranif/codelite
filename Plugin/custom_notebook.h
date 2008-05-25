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

#include "wx/panel.h"
#include "wx/aui/framemanager.h"

enum {
	wxVB_LEFT 					= 0x00000001,
	wxVB_RIGHT 					= 0x00000002,
	wxVB_TOP 					= 0x00000004,
	wxVB_BOTTOM					= 0x00000008,
	wxVB_HAS_X					= 0x00000010,
	wxVB_MOUSE_MIDDLE_CLOSE_TAB = 0x00000020,
	wxVB_TAB_DECORATION			= 0x00000040,
	wxVB_BG_GRADIENT			= 0x00000080,
	wxVB_BORDER					= 0x00000100,
	wxVB_NODND					= 0x00000200
};

class wxTabContainer;
class CustomTab;
class NotebookNavDialog;
class wxMenu;

class Notebook : public wxPanel
{
	friend class wxTabContainer;

	wxTabContainer *m_tabs;
	long m_style;
	wxAuiManager *m_aui;
	wxString m_paneName;
	NotebookNavDialog *m_popupWin;
	
public:
	static const size_t	npos = static_cast<size_t>(-1);

protected:
	void Initialize();
	
	void OnRender(wxAuiManagerEvent &e);
	
public:
	Notebook(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);
	virtual ~Notebook();

	/**
	 * \brief select given tab by the actual tab object
	 */
	void SetSelection(CustomTab *tab);
	
	/**
	 * \brief return the currently selected item index
	 * \return the currently selected item, of the book is empty, return Notebook::npos
	 */
	size_t GetSelection();

	/**
	 * \brief set page at given index to be the selected page. this function does not trigger an event
	 * \param page
	 */
	void SetSelection(size_t page);

	/**
	 * \brief add page to the book
	 * \param win window to add, the window will be reparented to the book
	 * \param text page's caption
	 * \param bmp image
	 * \param selected set the page as the selected page
	 */
	void AddPage(wxWindow *win, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false);

	/**
	 * \brief add page to the book
	 * \param tab to be added 
	 */
	void AddPage(CustomTab *tab);

	/**
	 * \brief return page at give position
	 * \param page page's index
	 * \return the page or NULL if index is out of bounds
	 */
	wxWindow *GetPage(size_t page);

	/**
	 * \brief return the number of pages are stored in the book
	 */
	size_t GetPageCount() const;
	
	/**
	 * \brief set the book orientation
	 */
	void SetOrientation(int orientation);
	
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
	void RemovePage(size_t page, bool notify = true);

	/**
	 * \brief delete page from the book and destroy it as well
	 * \param notify set this to true if you wish to receive wxEVT_COMMAND_BOOK_PAGE_CLOSING & wxEVT_COMMAND_BOOK_PAGE_CLOSED
	 */
	void DeletePage(size_t page, bool notify = true);
	
	/**
	 * \brief delete all the pages in the notebook, this function does not fire any events
	 */
	void DeleteAllPages();
	
	/**
	 * \brief set an AUI manager for this book. This allows the book to automatically detect chanegs in 
	 * orientation so the book can adapt itself 
	 * \param manager AUI manager
	 * \param containedPaneName the name of the cotainer name that contains our book 
	 */
	void SetAuiManager(wxAuiManager *manager, const wxString &containedPaneName);
	
	/**
	 * \brief return the tabbing history for this notebook
	 */
	const wxArrayPtrVoid& GetHistory() const;
	
	/**
	 * \brief return the notebook style
	 */
	long GetBookStyle() const {return m_style;}
	
	/**
	 * \brief return the tabs container control
	 */
	wxTabContainer *GetTabContainer() {return m_tabs;}
	
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
	 * \brief set the text for page at a given index
	 * \param index page's index
	 * \param text new text
	 */
	void SetPageText(size_t index, const wxString &text);
	
	DECLARE_EVENT_TABLE()
	virtual void OnNavigationKey(wxNavigationKeyEvent &e);
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
};

extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGED;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CHANGING;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSING;
extern const wxEventType wxEVT_COMMAND_BOOK_PAGE_CLOSED;

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

#endif // __Notebook__
