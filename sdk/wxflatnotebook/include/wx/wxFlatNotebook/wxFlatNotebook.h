///////////////////////////////////////////////////////////////////////////////
// Name:		wxFlatNotebook.cpp 
// Purpose:     generic implementation of flat style notebook class.
// Author:      Eran Ifrah <eranif@bezeqint.net>
// Modified by: Priyank Bolia <soft@priyank.in>
// Created:     30/12/2005
// Modified:    01/01/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license <http://www.wxwidgets.org/licence3.txt>
///////////////////////////////////////////////////////////////////////////////

#ifndef WXFLATNOTEBOOK_H
#define WXFLATNOTEBOOK_H

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/dynarray.h>

#ifdef __WXMSW__
#ifdef _DEBUG
//#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#endif // __WXMSW__

#ifdef WXMAKINGDLL_FNB
#    define WXDLLIMPEXP_FNB WXEXPORT
#elif defined(WXUSINGDLL_FNB)
#    define WXDLLIMPEXP_FNB WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_FNB
#endif // WXMAKINGDLL_FNB

#include <wx/dcbuffer.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>

#include <wx/wxFlatNotebook/wxFNBDropTarget.h>

class wxPageContainer;

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

// forward declerations
class wxFNBRenderer;
class wxFNBRendererDefault;
class wxFNBRendererFirefox2;
class wxFNBRendererVC8;
class wxTabNavigatorWindow;
class wxMenu;

// Since some compiler complains about std::min, we define our own macro
#define FNB_MIN(a, b) ((a > b) ? b : a)

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxBitmap, wxFlatNotebookImageList, WXDLLIMPEXP_FNB);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxWindow*, wxWindowPtrArray, WXDLLIMPEXP_FNB);

///  wxFlatNotebook styles
#define wxFNB_DEFAULT_STYLE				wxFNB_MOUSE_MIDDLE_CLOSES_TABS
#define wxFNB_FF2						0x00010000		// Firefox 2 tabs style
#define wxFNB_VC71						wxFNB_FF2		// support old styles
#define wxFNB_FANCY_TABS				wxFNB_FF2		// support old styles
#define wxFNB_TABS_BORDER_SIMPLE		0x00000004
#define wxFNB_NO_X_BUTTON				0x00000008
#define wxFNB_NO_NAV_BUTTONS			0x00000010
#define wxFNB_MOUSE_MIDDLE_CLOSES_TABS	0x00000020
#define wxFNB_BOTTOM					0x00000040
#define wxFNB_NODRAG					0x00000080
#define wxFNB_VC8						0x00000100
#define wxFNB_X_ON_TAB					0x00000200
#define wxFNB_BACKGROUND_GRADIENT		0x00000400
#define wxFNB_COLORFUL_TABS				0x00000800
#define wxFNB_DCLICK_CLOSES_TABS		0x00001000
#define wxFNB_SMART_TABS				0x00002000
#define wxFNB_DROPDOWN_TABS_LIST		0x00004000
#define wxFNB_ALLOW_FOREIGN_DND			0x00008000
#define wxFNB_LEFT						0x00010000		// Left tabs
#define wxFNB_CUSTOM_DLG				0x00020000		// Popup customize dialog using right click


/// General macros
#define VERTICAL_BORDER_PADDING			4
#define MASK_COLOR wxColor(0, 128, 128)

enum wxCustomizeDlgOptions {
	wxFNB_CUSTOM_TAB_LOOK		= 0x00000001,	///< Allow customizing the tab appearance
	wxFNB_CUSTOM_ORIENTATION	= 0x00000002,	///< Allow customizing the tab orientation (upper | bottom)
	wxFNB_CUSTOM_FOREIGN_DRAG	= 0x00000004,	///< Allow accept foreign tabs 
	wxFNB_CUSTOM_LOCAL_DRAG		= 0x00000008,	///< Allow local drag and drop
	wxFNB_CUSTOM_CLOSE_BUTTON	= 0x00000010,	///< Allow customizing close button
	wxFNB_CUSTOM_ALL			= wxFNB_CUSTOM_TAB_LOOK | 
								  wxFNB_CUSTOM_ORIENTATION |
								  wxFNB_CUSTOM_FOREIGN_DRAG |
								  wxFNB_CUSTOM_LOCAL_DRAG |
								  wxFNB_CUSTOM_CLOSE_BUTTON 
};

/**
* \brief Nice cross-platform flat notebook with X-button, navigation arrows and much more
*/

class WXDLLIMPEXP_FNB wxFlatNotebook : public wxPanel
{
public:

	///Default constructor
	wxFlatNotebook() 
	{ Init(); }

	/// Parametrized constructor
	/**
	\param parent - parent window
	\param id - window ID
	\param pos - window position
	\param size - window size
	\param style - window style
	\param name - window class name
	*/
	wxFlatNotebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("Flat Notebook"));

	/** 
	 * See wxFlatNotebook constructor
	 */
	bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("Flat Notebook"));

	/// Destructor
	virtual ~wxFlatNotebook(void);

	/**
     * Cleans up all the optimization structures held globally
     *
	 */
    static void CleanUp ();

	/**
	 * Advance the current selection
	 *\param bForward - if set to true then selection should be advanced forward otherwise - backward
	 */
	void AdvanceSelection(bool bForward);

	/// Apends new notebook page
	/**
	\param windows - window to be appended
	\param caption - tab caption
	\param selected - determines if new page should be selected automatically
	\param imgindex - page image index
	*/
	bool AddPage(wxWindow* windows, const wxString& caption, const bool selected = false, const int imgindex = -1);

	/// Inserts new notebook page
	/**
	\param index - page index
	\param page - window to be appended
	\param text - tab caption
	\param select - determines if new page should be selected automatically
	\param imgindex - page image index
	*/
	bool InsertPage(size_t index, wxWindow* page, const wxString& text, bool select = false, const int imgindex = -1);
	/// Changes the selection from currently visible/selected page to the page given by index.
	/**
	 * \param page - index of page to be selected
	 **/ 
	void SetSelection(size_t page);

	
	/// Removes the window from the notebook, and destroys the window associated with that notebook page.
	/**
	 * \param page - index of page to be deleted
	 * \param notify - by default wxFlatNotebook fires two events:
	 * - wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED 
	 * - wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING 
	 * to disable this functionality set notify to false
	 */
	void DeletePage(size_t page, bool notify = true);

	/// Deletes all notebook pages and destroys all windows associated with pages
	bool DeleteAllPages();

	/// Returns the total number of pages in the notebook.
	int  GetPageCount() const;

	/// Returns the window object associated with selected notebook page.
	wxWindow * GetCurrentPage() const;

	/// Returns the window object associated with a notebook page.
	/**
	\param page - page index
	*/
	wxWindow * GetPage(size_t page) const;
	/// Returns the page index of the window object.
	/**
	\param win - window object
	*/
	int GetPageIndex(wxWindow* win) const;

	/// Returns the currently visible/selected notebook page 0 based index.
	int GetSelection() const;

	/**
	* Return the previous selection, useful when implementing smart tabulation
	* \return previous selection, or wxNOT_FOUND 
	*/
	int GetPreviousSelection() const;

	const wxArrayInt &GetBrowseHistory() const;

	/// Returns the best size for a page
	wxSize GetPageBestSize();

	/// Sets the caption/text of the notebook page
	/**
	\param page - page index
	\param text - new value of tab caption
	*/
	bool SetPageText(size_t page, const wxString& text);

	/**
	 * Removes the window from the notebook, and destroys the window associated with that notebook page.
	 * \param page - index of page to be deleted
	 * \param notify - by default wxFlatNotebook fires two events:
	 * - wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED 
	 * - wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING 
	 * to disable this functionality set notify to false
	 */
	bool RemovePage(size_t page, bool notify = true);

	/// Sets the amount of space around each page's icon and label, in pixels.
	/**
	NB: The vertical padding cannot be changed in for wxFlatNotebook.
	\param padding - new amount of space around each page's icon and label
	*/
	void SetPadding(const wxSize& padding);
	/// Alters the notebook style
	/**
	\param style - new value of notebook style
	*/
	virtual void SetWindowStyleFlag(long style);

	/// Sets a right click menu to the notebook
	/**
	\param menu - right click menu object
	*/
	void SetRightClickMenu(wxMenu* menu);
	/// Returns the page text
	/**
	\param page - page index
	*/
	wxString GetPageText(size_t page);
	/// Sets an image index of specified page
	/**
	\param page - page index
	\param imgindex - new image index
	*/
	void SetPageImageIndex(size_t page, int imgindex);
	/// Returns an image index of specified page
	/**
	\param page - page index
	*/
	int GetPageImageIndex(size_t page);
	/// Sets gradient colors (only applicable when using the wxFNB_FANCY_TABS)
	/**
	\param from - first gradient colour
	\param to - second gradient colour
	\param border - page border colour
	*/
	void SetGradientColors(const wxColour& from, const wxColour& to, const wxColour& border);
	/// Sets first gradient colour
	/**
	\param from - new value of first gradient colour
	*/
	void SetGradientColorFrom(const wxColour& from);

	/// Sets second gradient colour
	/**
	\param to - new value of second gradient colour
	*/
	void SetGradientColorTo(const wxColour& to);
	/// Sets the colour of page border
	/**
	\param border - new value of the colour of page border
	*/
	void SetGradientColorBorder(const wxColour& border);
	/// Sets an image list associated with notebook pages
	/**
	\param imglist - image list object.
	Image list assigned with this method will not be deleted by wxFlatNotebook's destructor, you must delete it yourself.
	*/
	void SetImageList(wxFlatNotebookImageList * imglist);

	/// Returns an image list object associated with wxFlatNotebook
	wxFlatNotebookImageList * GetImageList();

	/**
	* \brief Drop event handler, to be passed as function pointer to CTextDropTarget class.
	* \param x X coordinate where the drop take place
	* \param y Y coordinate where the drop take place
	* \param nTabPage page index
	* \param wnd_oldContainer pointer to wxPageContainer object that contained dragged page
	* \return Drag operation identifier
	*/
	wxDragResult OnDropTarget(wxCoord x, wxCoord y, int nTabPage, wxWindow * wnd_oldContainer);

	/// Enable / Disable page
	/**
	\param page - page to enable/diable
	\param enabled - set to true to enable the tab, false otherwise
	*/
	void Enable(size_t page, bool enabled);

	/// Return Returns true if if the page is enabled
	/**
	\param page - page index
	*/
	bool GetEnabled(size_t page);

	/// Set the active tab text
	/**
	\param textColour - the active tab text colour
	*/
	void SetActiveTabTextColour(const wxColour& textColour);

	/// Gets first gradient colour
	const wxColour& GetGradientColorFrom();

	/// Gets second gradient colour
	const wxColour& GetGradientColorTo();

	/// Gets the tab border colour
	const wxColour& SetGradientColorBorder();

	/// Get the active tab text
	const wxColour& GetActiveTabTextColour();

	/// Get the non-active tab text color
	const wxColour& GetNonActiveTabTextColour();

	/// Set the non-active tab text color
	void SetNonActiveTabTextColour(const wxColour& color);

	/// Return the tab area panel
	wxPanel* GetTabArea() { return (wxPanel*)m_pages; }

	/// Get the tab area background colour
	const wxColour& GetTabAreaColour();

	/// Set the tab area background colour
	void SetTabAreaColour(const wxColour& color);

	/// Get the active tab color
	const wxColour& GetActiveTabColour();

	/// Set the active tab color
	void SetActiveTabColour(const wxColour& color);

	/**
	* Return the padding used between the text and icons, text and borders, etc.
	* \return padding in pixels
	*/
	int GetPadding() { return m_nPadding; }

	/**
	 * Set the customization options available for this notebook, can be one of the wxFNB_CUSTOM_* values
	 * this values is by default set to wxFNB_CUSTOM_ALL
	 */
	void SetCustomizeOptions(long options);
	
	/**
	 * Get the customization options available for this notebook
	 */
	long GetCustomizeOptions() const;
	
	/**
	 * \brief set a fixed tab width, when set to -1 (the default), wxFNB will calculate each tab lenght according to 
	 * image, text and x button, otherwise, the value provided here will be used. A tab can not have width lower than 
	 * 30 pixels. If the tabs' text is wider than the fixed tab width, it will be truncated 
	 * \param width in pixels, in range between 30 - 300
	 */
	void SetFixedTabWidth(int width);
	
	// Setters / Getters
	void SetForceSelection(bool force) { m_bForceSelection = force; }
	bool GetForceSelection() { return m_bForceSelection; }
	wxWindowPtrArray& GetWindows() { return m_windows; }
	wxPageContainer *GetPages() { return m_pages; }
	wxBoxSizer* GetMainSizer() { return m_mainSizer; }

	/**
	 * \param page - index of page to be selected
	 **/ 
	void DoSetSelection(size_t page);

protected:
	/// Initialization function, called internally
	virtual void Init();
	wxPageContainer *m_pages;

private:
	/// Internal flag to force selection of page,
	/// even if this page is disabled.
	/// used incase such that the book itself need to update its selection.
	/// e.g. after DeletePage()
	bool m_bForceSelection;

	wxBoxSizer* m_mainSizer;

	/// vector of all the windows associated with the notebook pages.
	wxWindowPtrArray m_windows;
	wxFNBDropTarget<wxFlatNotebook> *m_pDropTarget;
	int m_nFrom;
	int m_nPadding;
	wxTabNavigatorWindow *m_popupWin;
	bool m_sendPageChangeEvent; ///< Ugly but needed to allow SetSelection to send / dont send event
	int m_fixedTabWidth;
	
	DECLARE_EVENT_TABLE()
	void OnNavigationKey(wxNavigationKeyEvent& event);
};

/**
* \brief Contains parameters of notebook page
*/
class WXDLLIMPEXP_FNB wxPageInfo
{
private:
	// Members
	/// Page caption
	wxString m_strCaption;

	/// Page position
	wxPoint m_pos;

	/// Page size
	wxSize  m_size;

	/// Page region
	wxRegion m_region;

	/// Page image index
	int m_ImageIndex;

	/// Page enable/disabled flag
	bool m_bEnabled;

	/// Tab 'x' button rectangle
	wxRect m_xRect;

	/// Tab color
	wxColor m_color;

public:

	/// Default constructor
	wxPageInfo(): m_strCaption(wxEmptyString), m_ImageIndex(-1), m_bEnabled(true){};
	/// Parametrized constructor
	/**
	\param caption - page caption
	\param imgindex - image index
	*/
	wxPageInfo(const wxString& caption, int imgindex) :
	m_strCaption(caption), m_pos(-1, -1), m_size(-1, -1), m_ImageIndex(imgindex), m_bEnabled(true){}
	/// Destructor
	~wxPageInfo(){};

	/// Sets page caption
	/**
	\param value - new page caption
	*/
	void SetCaption(wxString value) {m_strCaption = value;}

	///Returns page caption
	wxString GetCaption() {return m_strCaption;}

	/// Sets page position
	/**
	\param value - new page position
	*/
	void SetPosition(wxPoint value) {m_pos = value;}

	///Returns page position
	const wxPoint & GetPosition() {return m_pos;}

	/// Sets page size
	/**
	\param value - new page size
	*/
	void SetSize(wxSize value) {m_size = value;}

	///Returns page size
	const wxSize & GetSize() {return m_size;}

	/// Sets page image index
	/**
	\param value - new image index
	*/
	void SetImageIndex(int value) {m_ImageIndex = value;}

	/// Returns an image index
	int GetImageIndex() {return m_ImageIndex;}

	/// Return true if the page is enabled
	bool GetEnabled() { return m_bEnabled; }

	/// Set the page enable/disable flag
	/**
	\param enabled - new page enable status
	*/
	void Enable(bool enabled) { m_bEnabled = enabled; }

	/// Set the page region
	/**
	\param n - number of points
	\param points - array of points that construct the region
	*/
	void SetRegion(const size_t n, const wxPoint points[]) { m_region = wxRegion(n, points); }

	/// Get the page region
	wxRegion& GetRegion() { return m_region ; }

	/// Set the 'x' button rectangle area
	/**
	\param xrect - the 'x' button rectangle
	*/
	void SetXRect(const wxRect& xrect) { m_xRect = xrect; }

	/// Get the 'x' button rectangle
	wxRect& GetXRect() { return m_xRect; }

	/**
	*
	* \return The tab color
	*/
	wxColor GetColor() { return m_color; }

	/**
	*
	* \param color Tab face color
	*/
	void SetColor(wxColor& color) { m_color = color; }

};

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxPageInfo, wxPageInfoArray, WXDLLIMPEXP_FNB);

/// Button status
enum
{
	wxFNB_BTN_PRESSED,
	wxFNB_BTN_HOVER,
	wxFNB_BTN_NONE
};

/// Hit Test results
enum
{
	wxFNB_TAB,				///< On a tab
	wxFNB_X,				///< On the X button
	wxFNB_TAB_X,			///< On the 'X' button (tab's X button)
	wxFNB_LEFT_ARROW,		///< On the rotate left arrow button
	wxFNB_RIGHT_ARROW,		///< On the rotate right arrow button
	wxFNB_DROP_DOWN_ARROW,	///< On the drop down arrow button
	wxFNB_NOWHERE			///< Anywhere else
};

/**
* \brief Notebook page
*/
class WXDLLIMPEXP_FNB wxPageContainer : public wxPanel
{
protected:

	friend class wxFlatNotebook;
	friend class wxFNBRenderer;
	friend class wxFNBRendererDefault;
	friend class wxFNBRendererVC71;
	friend class wxFNBRendererVC8;

	wxFlatNotebookImageList * m_ImageList;

public:
	/// Parametrized constructor
	/**
	\param parent - parent window
	\param id - window ID
	\param pos - window position
	\param size - window size
	\param style - window style
	*/
	wxPageContainer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	/// Destructor
	virtual ~wxPageContainer(void);

	/// Sets an image list associated with notebook pages
	/**
	\param imglist - image list object.
	Image list assigned with this method will not be deleted by wxFlatNotebook's destructor, you must delete it yourself.
	*/
	virtual void SetImageList(wxFlatNotebookImageList * imglist) {m_ImageList = imglist;}

	/// Returns an image list object associated with wxFlatNotebook
	virtual wxFlatNotebookImageList * GetImageList() {return m_ImageList;}

	/// Apends new notebook page
	/**
	\param caption - tab caption
	\param selected - determines if new page should be selected automatically
	\param imgindex - page image index
	*/
	virtual bool AddPage(const wxString& caption, const bool selected = false, const int imgindex = -1);

	/// Inserts new notebook page
	/**
	\param index - page index
	\param page - window to be appended
	\param text - tab caption
	\param select - determines if new page should be selected automatically
	\param imgindex - page image index
	*/
	virtual bool InsertPage(size_t index, wxWindow* page, const wxString& text, bool select = false, const int imgindex = -1);

	/// Changes the selection from currently visible/selected page to the page given by index.
	/**
	\param page - index of page to be selected
	*/
	virtual void SetSelection(size_t page);

	/// Returns the current selection page index
	virtual int GetSelection() { return m_iActivePage; }

	/// Advances the selection
	/**
	\param bForward - if set to true then selection should be advanced forward otherwise - backward
	*/
	virtual void AdvanceSelection(bool bForward);

	/// Return the number of pages
	virtual size_t GetPageCount() { return m_pagesInfoVec.size(); }

	/// Returns the page caption
	/**
	\param page - page index
	*/
	virtual wxString GetPageText(size_t page) { return m_pagesInfoVec[page].GetCaption(); }

	/// Set the caption of the page
	/**
	\param page - page index
	\param text - new page caption
	*/
	virtual bool SetPageText(size_t page, const wxString& text) { m_pagesInfoVec[page].SetCaption(text); return true; }

	/// Sets an image index of specified page
	/**
	\param page - page index
	\param imgindex - new image index
	*/
	virtual void SetPageImageIndex(size_t page, int imgindex);
	/// Returns an image index of specified page
	/**
	\param page - page index
	*/
	virtual int GetPageImageIndex(size_t page);

	/// Enable / Disable page
	/**
	\param page - page to enable/diable
	\param enabled - set to true to enable the tab, false otherwise
	*/
	virtual void Enable(size_t page, bool enabled);

	/// Return Returns true if if the page is enabled
	/**
	\param page - page index
	*/
	virtual bool GetEnabled(size_t page);

	/// Style helper methods
	bool HasFlag(int flag);

	/**
	* Return a vector containing the tabs informations (used by the redereres)
	* \return tabs info vector
	*/
	wxPageInfoArray& GetPageInfoVector() { return m_pagesInfoVec; }

	/**
	* Return the first graident colour ("from")
	* \return gradient colour 1
	*/
	const wxColour&  GetGradientColourFrom() const { return m_colorFrom; }

	/**
	* Return the second graident colour ("to")
	* \return gradient colour 2
	*/
	const wxColour&  GetGradientColourTo() const { return m_colorTo; }
	/**
	* Return tab's border colour
	* \return border colour
	*/
	const wxColour&  GetBorderColour() const { return m_colorBorder; }

	/**
	* Return non active tab's text colour
	* \return non active tab's text colour
	*/
	const wxColour&  GetNonoActiveTextColor() const { return m_nonActiveTextColor; }

	/**
	 * Return the active tab colour
	 * \return tab colour
	 */
	const wxColour&  GetActiveTabColour() const { return m_activeTabColor; }

	/**
	 * Get the previous selected tab, wxNOT_FOUND if none
	 * \return index of previous selected tab
	 */
	int GetPreviousSelection() const;// { return m_iPreviousActivePage; }

	/**
	 * Draw a tab preview 
	 */
	void DrawDragHint();
	
	/**
	 * Set the customization options available for this notebook, can be one of the wxFNB_CUSTOM_* values
	 * this values is by default set to wxFNB_CUSTOM_ALL
	 */
	void SetCustomizeOptions(long options);
	
	/**
	 * Get the customization options available for this notebook
	 */
	long GetCustomizeOptions() const;

	DECLARE_EVENT_TABLE()
	// Event handlers
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSize(wxSizeEvent& WXUNUSED(event));
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);
	virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnMiddleDown(wxMouseEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { }
	virtual void OnMouseLeave(wxMouseEvent& event);
	virtual void OnMouseEnterWindow(wxMouseEvent& event);
	virtual void OnLeftDClick(wxMouseEvent &event);
	virtual void OnTabMenuSelection(wxCommandEvent &event);
	virtual void OnShowCustomizeDialog(wxCommandEvent &event);

protected:
	
	void RotateLeft();
	void RotateRight();

	/**
	 * Popup a menu that contains all the tabs to be selected by user
	 */
	void PopupTabsMenu();

	void ClearFlag(int flag);

	/// return true if tabIdx has image
	bool TabHasImage(int tabIdx);

	/// Check whether the style is set to default
	virtual bool IsDefaultTabs();

	/// Return the color of the single line border
	virtual wxColor GetSingleLineBorderColor();

	/// Return true if page is visible
	virtual bool IsTabVisible(size_t page);

	/// Return if pt is anywhere on a tab, button or anywhere else
	virtual int HitTest(const wxPoint& pt, wxPageInfo& pageInfo, int &tabIdx);

	/// Display tool tip when mouse is hovering a tab
	virtual void ShowTabTooltip(int tabIdx);

	/// A wrapper from calling the DoDeletePage()
	virtual void DeletePage(size_t page);

	/// Remove all pages from the container (it also deletes them)
	virtual void DeleteAllPages();

	/// Perform the actual deletion of a tab from the container
	/// The window is also deleted using this function
	virtual void DoDeletePage(size_t page);

	/// Preform the actual page selection
	virtual void DoSetSelection(size_t page);

	/// Return the index of the last visible index
	virtual int  GetLastVisibleTab();

	/// Return the number of tabs that can be scrolled left
	/// starting from the first visible tab (i.e. m_nFrom)
	virtual int  GetNumTabsCanScrollLeft();

	/// Return the number of visible tabs
	virtual int GetNumOfVisibleTabs();

	/**
	* \brief Drop event handler, to be passed as function pointer to CTextDropTarget class.
	* \param x X coordinate where the drop take place
	* \param y Y coordinate where the drop take place
	* \param nTabPage page index
	* \param wnd_oldContainer pointer to wxPageContainer object that contained dragged page
	* \return Drag operation identifier
	*/
	virtual wxDragResult OnDropTarget(wxCoord x, wxCoord y, int nTabPage, wxWindow * wnd_oldContainer);

	/**
	* \brief Moves the tab page from one location to another
	* \param nMove The index of the tab page to be moved.
	* \param nMoveTo The index for the tab page, where it has to be moved
	*/
	virtual void MoveTabPage(int nMove, int nMoveTo);

	/// Check whether page can fit to the current
	/// screen or a scrolling is  required
	/**
	\param page - page index
	*/
	virtual bool CanFitToScreen(size_t page);
	
	void PushPageHistory(int page);

	//remove page from the history by its value
	//after the page removal, all items in the history 
	//are updated if needed
	void PopPageHistory(int page);

protected:

	wxPageInfoArray m_pagesInfoVec;
	int m_iActivePage;
	int m_nFrom;

	/// Drop target for enabling drag'n'drop of tabs
	wxFNBDropTarget<wxPageContainer> *m_pDropTarget;

private:
	/// Pointer to the parent window
	wxWindow *m_pParent;

	/// The right click menu
	wxMenu* m_pRightClickMenu;

	/// Gradient colors
	wxColour m_colorFrom, m_colorTo, m_colorBorder, m_activeTextColor, m_nonActiveTextColor, m_tabAreaColor, m_activeTabColor;

	/// X,>,< buttons status, can be one of
	/// - Pressed
	/// - Hover
	/// - None
	int m_nXButtonStatus, m_nLeftButtonStatus, m_nRightButtonStatus, m_nTabXButtonStatus;

	/// holds the button id in case a left click is done on one of them
	int m_nLeftClickZone;

	//int m_iPreviousActivePage;
	wxArrayInt m_history;
	int m_nArrowDownButtonStatus;

	/// Customize menu
	wxMenu *m_customMenu;
	long m_customizeOptions;
	int m_fixedTabWidth;
};

/**
* \brief Holds information about events associated with wxFlatNotebook objects
*/
class WXDLLIMPEXP_FNB wxFlatNotebookEvent : public wxNotifyEvent
{
	//DECLARE_DYNAMIC_CLASS(wxFlatNotebookEvent)
	size_t sel, oldsel;

public:
	/// Constructor
	/**
	\param commandType - event type
	\param winid - window ID
	\param nSel - current selection
	\param nOldSel - old selection
	*/
	wxFlatNotebookEvent(wxEventType commandType = wxEVT_NULL, int winid = 0, int nSel = -1, int nOldSel = -1)
		: wxNotifyEvent(commandType, winid), sel(nSel), oldsel(nOldSel)
	{}
	/// Sets the value of current selection
	/**
	\param s - index of currently selected page
	*/
	void SetSelection(int s) { sel = s; }
	/// Sets the value of previous selection
	/**
	\param s - index of previously selected page
	*/
	void SetOldSelection(int s) { oldsel = s; }
	/// Returns the index of currently selected page
	int  GetSelection() { return (int)sel; }
	/// Returns the index of previously selected page
	int  GetOldSelection() { return (int)oldsel; }
};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FNB, wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED, 50000)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FNB, wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGING, 50001)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FNB, wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, 50002)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FNB, wxEVT_COMMAND_FLATNOTEBOOK_CONTEXT_MENU, 50003)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_FNB, wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED, 50004)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxFlatNotebookEventFunction)(wxFlatNotebookEvent&);

#define wxFlatNotebookEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxFlatNotebookEventFunction, &func)

#define EVT_FLATNOTEBOOK_PAGE_CHANGED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED, winid, wxFlatNotebookEventHandler(fn))

#define EVT_FLATNOTEBOOK_PAGE_CHANGING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGING, winid, wxFlatNotebookEventHandler(fn))

#define EVT_FLATNOTEBOOK_PAGE_CLOSING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, winid, wxFlatNotebookEventHandler(fn))

#define EVT_FLATNOTEBOOK_CONTEXT_MENU(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_FLATNOTEBOOK_CONTEXT_MENU, winid, wxFlatNotebookEventHandler(fn))

#define EVT_FLATNOTEBOOK_PAGE_CLOSED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED, winid, wxFlatNotebookEventHandler(fn))

#endif // WXFLATNOTEBOOK_H
