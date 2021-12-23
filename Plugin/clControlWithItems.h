#ifndef CLCONTROLWITHITEMS_H
#define CLCONTROLWITHITEMS_H

#include "clColours.h"
#include "clHeaderBar.h"
#include "clRowEntry.h"
#include "clScrolledPanel.h"

#include <array>
#include <memory>
#include <wx/imaglist.h>

#ifdef __WXOSX__
#define SCROLL_TICK 2
#else
#define SCROLL_TICK 2
#endif

class clSearchControl;
class clControlWithItems;
class clRowEntry;

// Search flags
#define wxTR_SEARCH_METHOD_EXACT (1 << 0)    // Use an exact string comparison method
#define wxTR_SEARCH_METHOD_CONTAINS (1 << 1) // Compare using wxString::Contains method
#define wxTR_SEARCH_VISIBLE_ITEMS (1 << 2)   // Search reachable items (i.e. they can be visible if scrolled enough)
#define wxTR_SEARCH_ICASE (1 << 3)           // Search incase-sensitive
#define wxTR_SEARCH_INCLUDE_CURRENT_ITEM \
    (1 << 4) // When calling the search API, FindNext/FindPrev include the 'starting' item
#define wxTR_SEARCH_DEFAULT \
    (wxTR_SEARCH_METHOD_CONTAINS | wxTR_SEARCH_VISIBLE_ITEMS | wxTR_SEARCH_ICASE | wxTR_SEARCH_INCLUDE_CURRENT_ITEM)

class WXDLLIMPEXP_SDK clSearchText
{
    bool m_enabled = false;

public:
    static bool Matches(const wxString& findWhat, size_t col, const wxString& text,
                        size_t searchFlags = wxTR_SEARCH_DEFAULT, clMatchResult* matches = nullptr);
    clSearchText();
    virtual ~clSearchText();
    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
};

/// base class for custom row renderers
class WXDLLIMPEXP_SDK clControlWithItemsRowRenderer
{
public:
    clControlWithItemsRowRenderer() {}
    virtual ~clControlWithItemsRowRenderer() {}

    /**
     * @brief override this method to provide a custom row drawing.
     * The text, bitmap and other info that needs to be drawn are stored
     * in the `entry` field.
     */
    virtual void Render(wxWindow* window, wxDC& dc, const clColours& colours, int row_index, clRowEntry* entry) = 0;
};

class WXDLLIMPEXP_SDK clControlWithItems : public clScrolledPanel
{
public:
    typedef std::vector<wxBitmap> BitmapVec_t;

protected:
    clHeaderBar* m_viewHeader = nullptr;
    clColours m_colours;
    clRowEntry* m_firstItemOnScreen = nullptr;
    int m_firstColumn = 0;
    int m_lineHeight = 0;
    int m_indent = 0;
    BitmapVec_t* m_bitmaps = nullptr;
    BitmapVec_t* m_bitmapsInternal = nullptr;
    int m_scrollTick = SCROLL_TICK;
    clSearchText m_search;
    clSearchControl* m_searchControl = nullptr;
    bool m_maxList = false;
    bool m_nativeTheme = false;
    std::unique_ptr<clControlWithItemsRowRenderer> m_customRenderer;
    wxFont m_defaultFont = wxNullFont;

protected:
    void DoInitialize();
    int GetNumLineCanFitOnScreen(bool fully_fit = false) const;
    virtual clRowEntry* GetFirstItemOnScreen();
    virtual void SetFirstItemOnScreen(clRowEntry* item);
    void RenderItems(wxDC& dc, const clRowEntry::Vec_t& items);
    void AssignRects(const clRowEntry::Vec_t& items);
    void OnSize(wxSizeEvent& event);
    void DoUpdateHeader(clRowEntry* row);
    wxSize GetTextSize(const wxString& label) const;
    virtual void OnMouseScroll(wxMouseEvent& event);
    virtual bool DoKeyDown(const wxKeyEvent& event);
    virtual void DoMouseScroll(const wxMouseEvent& event);
    clSearchText& GetSearch() { return m_search; }
    const clSearchText& GetSearch() const { return m_search; }

    void DoPositionHScrollbar();
    void DoPositionVScrollbar();

public:
    clControlWithItems(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clControlWithItems();
    clControlWithItems();

    virtual void SetDefaultFont(const wxFont& font);
    virtual wxFont GetDefaultFont() const;

    /**
     * @brief set a custom renderer to draw the rows for this control
     * `this` takes ownership for the renderer (i.e. it will free it)
     */
    void SetCustomRenderer(clControlWithItemsRowRenderer* renderer);

    void SetNativeTheme(bool nativeTheme);
    bool IsNativeTheme() const { return m_nativeTheme; }
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0);
    virtual int GetIndent() const { return m_indent; }

    virtual void SetFirstColumn(int firstColumn) { this->m_firstColumn = firstColumn; }
    virtual int GetFirstColumn() const { return m_firstColumn; }

    virtual void SetLineHeight(int lineHeight) { this->m_lineHeight = lineHeight; }
    virtual int GetLineHeight() const { return m_lineHeight; }

    virtual void SetBitmaps(BitmapVec_t* bitmaps) { this->m_bitmaps = bitmaps; }
    virtual void SetImageList(wxImageList* images);
    virtual const BitmapVec_t* GetBitmaps() const { return m_bitmaps; }
    virtual BitmapVec_t* GetBitmaps() { return m_bitmaps; }

    void SetScrollTick(int scrollTick) { this->m_scrollTick = scrollTick; }
    int GetScrollTick() const { return m_scrollTick; }

    /**
     * @brief return bitmap at a given index
     */
    const wxBitmap& GetBitmap(size_t index) const;

    /**
     * @brief set the item's indent size
     */
    virtual void SetIndent(int size) { m_indent = size; }

    /**
     * @brief return the items rect area, excluding header
     */
    wxRect GetItemsRect() const;

    /**
     * @brief draw the header + items
     */
    void Render(wxDC& dc);

    /**
     * @brief Get a pointer to the header, create one if needed
     */
    clHeaderBar* GetHeader() const;

    /**
     * @brief sets a column width. If the width is less than 0, this function does nothing. If the column is out of
     * bound this function does nothing. Two sepcial values are allowed here: wxCOL_WIDTH_DEFAULT and
     * wxCOL_WIDTH_AUTOSIZE
     * @param col the column index
     * @param width the width. Can contain one of the special values: wxCOL_WIDTH_DEFAULT and wxCOL_WIDTH_AUTOSIZE
     */
    void SetColumnWidth(size_t col, int width);

    /**
     * @brief should we show the header bar?
     */
    void SetShowHeader(bool b);
    /**
     * @brief is the heaer bar visible?
     */
    bool IsHeaderVisible() const;

    /**
     * @brief update the scrollbar with the current view status
     * subclass should call this method whenver the view changes (re-sized, items are expanding, collapsing etc)
     */
    virtual void UpdateScrollBar();

    void SetColours(const clColours& colours);

    const clColours& GetColours() const { return m_colours; }
    clColours& GetColours() { return m_colours; }

    // Horizontal scrolling implementation
    void ScollToColumn(int firstColumn);
    void ScrollColumns(int steps, wxDirection direction);

    //===-----------------------------------------
    //===-----------------------------------------

    /**
     * @brief override this
     */
    virtual bool IsEmpty() const = 0;
    /**
     * @brief return the total of numbers of items we can scroll
     * If the view has collpased items, the range _excludes_ them
     */
    virtual int GetRange() const = 0;

    /**
     * @brief return the row number of the first visible item in the view
     */
    virtual int GetFirstItemPosition() const = 0;

    void SearchControlDismissed();
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TREE_SEARCH_TEXT, wxTreeEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TREE_CLEAR_SEARCH, wxTreeEvent);

#endif // CLCONTROLWITHITEMS_H
