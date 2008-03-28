#ifndef __wxverticalbook__
#define __wxverticalbook__

#include "wx/panel.h"
#include "wx/aui/framemanager.h"

enum {
	wxVB_LEFT 	= 0x00000001,
	wxVB_RIGHT 	= 0x00000002
};

class wxTabContainer;
class wxVerticalTab;

class wxVerticalBook : public wxPanel
{
	friend class wxTabContainer;

	wxTabContainer *m_tabs;
	long m_style;
	wxAuiManager *m_aui;
	wxString m_paneName;
	
public:
	static const size_t	npos = static_cast<size_t>(-1);

protected:
	void Initialize();
	void SetSelection(wxVerticalTab *tab);
	void OnRender(wxAuiManagerEvent &e);
	
public:
	wxVerticalBook(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);
	virtual ~wxVerticalBook();


	/**
	 * \brief return the currently selected item index
	 * \return the currently selected item, of the book is empty, return wxVerticalBook::npos
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
	 * \brief set the book orientation to the left
	 */
	void SetOrientationLeft(bool left = true);
	
	/**
	 * \brief return the page caption
	 * \param page page index
	 * \return text or wxEmptyString if page index is invalid
	 */
	wxString GetPageText(size_t page) const;
	
	/**
	 * \brief remove page from the book without destroying it
	 */
	void RemovePage(size_t page);

	/**
	 * \brief delete page from the book and destroy it as well
	 */
	void DeletePage(size_t page);
	
	/**
	 * \brief set an AUI manager for this book. This allows the book to automatically detect chanegs in 
	 * orientation so the book can adapt itself 
	 * \param manager AUI manager
	 * \param containedPaneName the name of the cotainer name that contains our book 
	 */
	void SetAuiManager(wxAuiManager *manager, const wxString &containedPaneName);
	
};

class wxVerticalBookEvent : public wxNotifyEvent
{
	size_t sel, oldsel;

public:
	/**
	 * \param commandType - event type
	 * \param winid - window ID
	 * \param nSel - current selection
	 * \param nOldSel - old selection
	 */
	wxVerticalBookEvent(wxEventType commandType = wxEVT_NULL, int winid = 0, size_t nSel = (size_t)-1, size_t nOldSel = (size_t)-1)
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

extern const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGED;
extern const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGING;
extern const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CLOSING;
extern const wxEventType wxEVT_COMMAND_VERTICALBOOK_PAGE_CLOSED;

typedef void (wxEvtHandler::*wxVerticalBookEventFunction)(wxVerticalBookEvent&);

#define wxVerticalBookEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxVerticalBookEventFunction, &func)

#define EVT_VERTICALBOOK_PAGE_CHANGED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGED, winid, wxVerticalBookEventHandler(fn))

#define EVT_VERTICALBOOK_PAGE_CHANGING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_VERTICALBOOK_PAGE_CHANGING, winid, wxVerticalBookEventHandler(fn))

#define EVT_VERTICALBOOK_PAGE_CLOSING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_VERTICALBOOK_PAGE_CLOSING, winid, wxVerticalBookEventHandler(fn))

#define EVT_VERTICALBOOK_PAGE_CLOSED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_VERTICALBOOK_PAGE_CLOSED, winid, wxVerticalBookEventHandler(fn))

#endif // __wxverticalbook__
