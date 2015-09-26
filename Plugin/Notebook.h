#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <wx/panel.h>
#include <wx/simplebook.h>
#include <vector>
#include <list>
#include <wx/settings.h>
#include <wx/dcmemory.h>
#include <wx/sharedptr.h>
#include <wx/bookctrl.h>
#include "windowstack.h"
#include <wx/dynarray.h>
#include <wx/dnd.h>

#ifdef WXDLLIMPEXP_SDK
#undef WXDLLIMPEXP_SDK
#endif

#ifdef __WXMSW__
#ifdef WXMAKINGDLL_SDK
#define WXDLLIMPEXP_SDK __declspec(dllexport)
#elif defined(WXUSINGDLL_SDK)
#define WXDLLIMPEXP_SDK __declspec(dllimport)
#else // not making nor using DLL
#define WXDLLIMPEXP_SDK
#endif
#else
#define WXDLLIMPEXP_SDK
#endif

enum NotebookStyle {
    /// Use the built-in light tab colours
    kNotebook_LightTabs = (1 << 0),
    /// Use the built-in dark tab colours
    kNotebook_DarkTabs = (1 << 1),
    /// Allow tabs to move using DnD
    kNotebook_AllowDnD = (1 << 2),
    /// Draw X button on the active tab
    kNotebook_CloseButtonOnActiveTab = (1 << 3),
    /// Mouse middle click closes tab
    kNotebook_MouseMiddleClickClosesTab = (1 << 4),
    /// Show a drop down button for displaying all tabs list
    kNotebook_ShowFileListButton = (1 << 5),
    /// Mouse middle click on a tab fires an event
    kNotebook_MouseMiddleClickFireEvent = (1 << 6),
    /// Clicking the X button on the active button fires an event
    /// instead of closing the tab (i.e. let the container a complete control)
    kNotebook_CloseButtonOnActiveTabFireEvent = (1 << 7),
    /// Fire navigation event for Ctrl-TAB et al
    kNotebook_EnableNavigationEvent = (1 << 8),
    /// Place tabs at the bottom
    kNotebook_BottomTabs = (1 << 9),
    /// Enable colour customization events
    kNotebook_EnableColourCustomization = (1 << 10),
    /// Place the tabs on the right
    kNotebook_RightTabs = (1 << 11),
    /// Place th tabs on the left
    kNotebook_LeftTabs = (1 << 12),
    /// Vertical tabs as buttons
    kNotebook_VerticalButtons = (1 << 13),
    /// Default notebook
    kNotebook_Default = kNotebook_LightTabs | kNotebook_ShowFileListButton,
};

/**
 * @class clTabInfo
 * @author Eran Ifrah
 * @brief contains information (mainly for drawing purposes) about a single tab label
 */
class WXDLLIMPEXP_SDK clTabInfo
{
    wxString m_label;
    wxBitmap m_bitmap;
    wxString m_tooltip;
    wxWindow* m_window;
    wxRect m_rect;
    bool m_active;
    int m_textX;
    int m_textY;
    int m_bmpX;
    int m_bmpY;
    int m_bmpCloseX;
    int m_bmpCloseY;
    int m_width;
    int m_height;
    int m_vTabsWidth;

public:
    class WXDLLIMPEXP_SDK Colours
    {
    public:
        // Active tab colours
        wxColour inactiveTabBgColour;
        wxColour inactiveTabPenColour;
        wxColour inactiveTabTextColour;
        wxColour inactiveTabInnerPenColour;

        // Inactive tab colours
        wxColour activeTabTextColour;
        wxColour activeTabBgColour;
        wxColour activeTabPenColour;
        wxColour activeTabInnerPenColour;

        // the tab area colours
        wxColour tabAreaColour;

        // close button bitmaps (MUST be 12x12)
        wxBitmap closeButton;

        /// Chevron down arrow used as the button for showing tab list
        wxBitmap chevronDown;

        Colours();
        virtual ~Colours() {}

        /**
         * @brief initialize the colours from base colour and text colour
         */
        void InitFromColours(const wxColour& baseColour, const wxColour& textColour);

        /**
         * @brief initialize the dark colours
         */
        virtual void InitDarkColours();
        /**
         * @brief initialize the light colours
         */
        virtual void InitLightColours();
    };

public:
    // Geometry
    static int Y_SPACER;
    static int X_SPACER;
    static int BOTTOM_AREA_HEIGHT;
    static int MAJOR_CURVE_WIDTH;
    static int SMALL_CURVE_WIDTH;
    // static int TAB_HEIGHT;

public:
    void CalculateOffsets(size_t style);

public:
    typedef wxSharedPtr<clTabInfo> Ptr_t;
    typedef std::vector<clTabInfo::Ptr_t> Vec_t;

    clTabInfo();
    clTabInfo(size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp = wxNullBitmap);
    virtual ~clTabInfo() {}

    bool IsValid() const { return m_window != NULL; }

    /**
     * @brief render the using the provided wxDC
     */
    void Draw(wxDC& dc, const clTabInfo::Colours& colours, size_t style);
    void SetBitmap(const wxBitmap& bitmap, size_t style);
    void SetLabel(const wxString& label, size_t style);
    void SetActive(bool active, size_t style);
    void SetRect(const wxRect& rect) { this->m_rect = rect; }
    const wxBitmap& GetBitmap() const { return m_bitmap; }
    const wxString& GetLabel() const { return m_label; }
    const wxRect& GetRect() const { return m_rect; }
    wxRect& GetRect() { return m_rect; }
    wxWindow* GetWindow() { return m_window; }
    wxWindow* GetWindow() const { return m_window; }
    void SetWindow(wxWindow* window) { this->m_window = window; }
    bool IsActive() const { return m_active; }
    int GetBmpCloseX() const { return m_bmpCloseX; }
    int GetBmpCloseY() const { return m_bmpCloseY; }
    int GetHeight() const { return m_height; }
    int GetWidth() const { return m_width; }
    void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
    const wxString& GetTooltip() const { return m_tooltip; }
};

class Notebook;
class wxMenu;
class clTabCtrl;

// DnD support of tabs
class WXDLLIMPEXP_SDK clTabCtrlDropTarget : public wxTextDropTarget
{
    clTabCtrl* m_tabCtrl;

public:
    clTabCtrlDropTarget(clTabCtrl* tabCtrl);
    virtual ~clTabCtrlDropTarget();
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
};

class WXDLLIMPEXP_SDK clTabHistory
{
    wxArrayPtrVoid m_history;
    wxWindow* m_page; /// The page to add to the hisotry

public:
    typedef wxSharedPtr<clTabHistory> Ptr_t;

public:
    clTabHistory();
    virtual ~clTabHistory();

    void Push(wxWindow* page);
    void Pop(wxWindow* page);
    wxWindow* PrevPage();
    /**
     * @brief clear the history
     */
    void Clear();

    /**
     * @brief return the tabbing history
     * @return
     */
    const wxArrayPtrVoid& GetHistory() const { return m_history; }
};

/**
 * @class clTabCtrl
 * @author Eran Ifrah
 * @brief The Window that all the tabs are drawn on
 */
class WXDLLIMPEXP_SDK clTabCtrl : public wxPanel
{
    int m_height;
    int m_vTabsWidth;
    clTabInfo::Vec_t m_tabs;
    friend class Notebook;
    friend class clTabCtrlDropTarget;

    size_t m_style;
    clTabInfo::Colours m_colours;
    clTabInfo::Vec_t m_visibleTabs;
    int m_closeButtonClickedIndex;
    wxMenu* m_contextMenu;
    wxRect m_chevronRect;
    clTabHistory::Ptr_t m_history;

    void DoChangeSelection(size_t index);

protected:
    void OnPaint(wxPaintEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void OnSize(wxSizeEvent& event);
    void OnWindowKeyDown(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnLeftDClick(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseMiddleClick(wxMouseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    int DoGetPageIndex(wxWindow* win) const;
    int DoGetPageIndex(const wxString& label) const;
    void
    DoDrawBottomBox(clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc, const clTabInfo::Colours& colours);
    bool ShiftRight(clTabInfo::Vec_t& tabs);
    bool ShiftBottom(clTabInfo::Vec_t& tabs);
    bool IsActiveTabInList(const clTabInfo::Vec_t& tabs) const;
    bool IsActiveTabVisible(const clTabInfo::Vec_t& tabs) const;

    /**
     * @brief loop over the tabs and set their coordiantes
     */
    void DoUpdateCoordiantes(clTabInfo::Vec_t& tabs);
    /**
     * @brief get list of tabs to draw. This call always returns the active tab as part of the list
     * It also ensures that we draw as much tabs as we can.
     * @param offset reset the 0 based index from m_tabs
     */
    void UpdateVisibleTabs();

    clTabInfo::Ptr_t GetTabInfo(size_t index);
    clTabInfo::Ptr_t GetTabInfo(size_t index) const;
    clTabInfo::Ptr_t GetTabInfo(wxWindow* page);
    clTabInfo::Ptr_t GetActiveTabInfo();

    WindowStack* GetStack();

    void DoDeletePage(size_t page) { RemovePage(page, true, true); }
    void DoShowTabList();

public:
    clTabCtrl(wxWindow* notebook, size_t style);
    virtual ~clTabCtrl();
    
    bool IsVerticalTabs() const;
    
    void SetColours(const clTabInfo::Colours& colours) { this->m_colours = colours; }
    const clTabInfo::Colours& GetColours() const { return m_colours; }

    /**
     * @brief test if pt is on one of the visible tabs return its index
     * @param pt mouse click position
     * @param realPosition [output] the index position in the m_tabs array
     * @param tabHit [output] the index position in the m_visibleTabs array
     */
    void TestPoint(const wxPoint& pt, int& realPosition, int& tabHit);

    /**
     * @brief Move the active tab to a new position
     * @param newIndex the new position. 0-based index in the m_tabs array
     */
    bool MoveActiveToIndex(int newIndex);

    /**
     * @brief return true if index is in the tabs vector range
     */
    bool IsIndexValid(size_t index) const;

    void SetStyle(size_t style);
    size_t GetStyle() const { return m_style; }

    /**
     * @brief update the selected tab. This function also fires an event
     */
    int SetSelection(size_t tabIdx);

    /**
     * @brief update the selected tab. This function does not fire an event
     */
    int ChangeSelection(size_t tabIdx);

    /**
     * @brief return the current selection. return wxNOT_FOUND if non is selected
     */
    int GetSelection() const;

    /**
     * @brief Sets the text for the given page.
     */
    bool SetPageText(size_t page, const wxString& text);
    wxString GetPageText(size_t page) const;

    void AddPage(clTabInfo::Ptr_t tab);
    bool InsertPage(size_t index, clTabInfo::Ptr_t tab);

    void SetPageBitmap(size_t index, const wxBitmap& bmp);
    wxBitmap GetPageBitmap(size_t index) const;
    wxWindow* GetPage(size_t index) const;
    void GetAllPages(std::vector<wxWindow*>& pages);
    int FindPage(wxWindow* page) const;
    bool RemovePage(size_t page, bool notify, bool deletePage);
    bool DeleteAllPages();
    void SetMenu(wxMenu* menu);
    bool SetPageToolTip(size_t page, const wxString& tooltip);
    const clTabInfo::Vec_t& GetTabs() const { return m_tabs; }
    clTabHistory::Ptr_t GetHistory() const { return m_history; }
};

/**
 * @class Notebook
 * @author Eran Ifrah
 * @brief A modern notebook (similar to the ones seen on Sublime Text and Atom editors
 * for wxWidgets. The class implementation uses wxSimplebook as the tab container and a
 * custom drawing tab area (see above the class clTabCtrl)
 */
class WXDLLIMPEXP_SDK Notebook : public wxPanel
{
    WindowStack* m_windows;
    clTabCtrl* m_tabCtrl;
    friend class clTabCtrl;

protected:
    void DoChangeSelection(wxWindow* page);
    bool IsVerticalTabs() const {
        return m_tabCtrl->IsVerticalTabs();
    }
    
public:
    /**
     * Constructor
     */
    Notebook(wxWindow* parent,
             wxWindowID id = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxEmptyString);

    /**
     * @brief set the notebook style. The style bits are kNotebook_* (you can set several
     * styles OR-ed)
     */
    void SetStyle(size_t style);
    
    /**
     * @brief set the tab direction
     */
    void SetTabDirection(wxDirection d);
    
    /**
     * @brief return the book style
     */
    size_t GetStyle() const { return m_tabCtrl->GetStyle(); }

    /**
     * @brief enable a specific style in the notebook
     */
    void EnableStyle(NotebookStyle style, bool enable);

    /**
     * destructor
     */
    virtual ~Notebook();

    /**
     * @brief append page to the notebook
     */
    void AddPage(wxWindow* page, const wxString& label, bool selected = false, const wxBitmap& bmp = wxNullBitmap);

    /**
     * @brief insert page at a specified position
     */
    bool InsertPage(size_t index,
                    wxWindow* page,
                    const wxString& label,
                    bool selected = false,
                    const wxBitmap& bmp = wxNullBitmap);

    /**
     * @brief return the currently selected page or null
     */
    wxWindow* GetCurrentPage() const;

    /**
     * @brief Returns the index of the specified tab window or wxNOT_FOUND if not found
     */
    int FindPage(wxWindow* page) const;

    /**
     * @brief Deletes the specified page, without deleting the associated window
     */
    bool RemovePage(size_t page, bool notify = false);

    /**
     * @brief Deletes the specified page and the associated window
     */
    bool DeletePage(size_t page);

    /**
     * @brief Deletes all pages
     */
    bool DeleteAllPages();

    /**
     * @brief set a new selection. This function fires an event that can be vetoed
     */
    int SetSelection(size_t selection) { return m_tabCtrl->SetSelection(selection); }
    /**
     * @brief set new selection. No events are fired
     */
    int ChangeSelection(size_t selection) { return m_tabCtrl->ChangeSelection(selection); }

    /**
     * @brief return the currently selected page, return wxNOT_FOUND if non found
     */
    int GetSelection() const { return m_tabCtrl->GetSelection(); }

    /**
     * @brief Sets the text for the given page.
     */
    bool SetPageText(size_t page, const wxString& text) { return m_tabCtrl->SetPageText(page, text); }

    /**
     * @brief Returns the string for the given page
     */
    wxString GetPageText(size_t page) const { return m_tabCtrl->GetPageText(page); }

    /**
     * @brief set the image for the given page
     */
    void SetPageBitmap(size_t index, const wxBitmap& bmp) { m_tabCtrl->SetPageBitmap(index, bmp); }

    /**
     * @brief return bitmap for a given page. Return wxNullBitmap if invalid page
     */
    wxBitmap GetPageBitmap(size_t index) const { return m_tabCtrl->GetPageBitmap(index); }

    // Base class members...
    virtual bool SetPageImage(size_t page, int image)
    {
        wxUnusedVar(page);
        wxUnusedVar(image);
        return false;
    }
    virtual int GetPageImage(size_t n) const { return wxNOT_FOUND; }

    /**
     * @brief return the index of a given window in the tab control
     * @param window
     * @return return window index, or wxNOT_FOUND
     */
    int GetPageIndex(wxWindow* window) const { return m_tabCtrl->DoGetPageIndex(window); }

    /**
     * @brief return the index of a given window by its title
     */
    int GetPageIndex(const wxString& label) const { return m_tabCtrl->DoGetPageIndex(label); }

    /**
     * @brief Returns the number of pages in the control
     */
    size_t GetPageCount() const { return m_tabCtrl->GetTabs().size(); }

    /**
     * @brief Returns the window at the given page position.
     */
    wxWindow* GetPage(size_t index) const { return m_tabCtrl->GetPage(index); }

    /**
     * @brief return an array of all the windows managed by this notebook
     */
    void GetAllPages(std::vector<wxWindow*>& pages) { m_tabCtrl->GetAllPages(pages); }

    /**
     * @brief return all tabs info
     * @param tabs [output]
     */
    size_t GetAllTabs(clTabInfo::Vec_t& tabs)
    {
        tabs = m_tabCtrl->GetTabs();
        return tabs.size();
    }

    /**
     * @brief set a context menu to be shown whe context menu is requested
     * on a tab label
     */
    void SetMenu(wxMenu* menu) { m_tabCtrl->SetMenu(menu); }

    /**
     * @brief Sets the tool tip displayed when hovering over the tab label of the page
     * @return true if tool tip was updated, false if it failed, e.g. because the page index is invalid.
     */
    bool SetPageToolTip(size_t page, const wxString& tooltip) { return m_tabCtrl->SetPageToolTip(page, tooltip); }

    /**
     * @brief return the tabbing history
     * @return
     */
    clTabHistory::Ptr_t GetHistory() const { return m_tabCtrl->GetHistory(); }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_NAVIGATING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);

#endif // NOTEBOOK_H
