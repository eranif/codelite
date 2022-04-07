#ifndef CLTABRENDERER_H
#define CLTABRENDERER_H

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

#include "clTabRenderer.h"
#include "cl_defs.h"
#include "drawingutils.h"
#include "wxStringHash.h"

#include <vector>
#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/dc.h>
#include <wx/sharedptr.h>
#include <wx/window.h>

#define CHEVRON_SIZE 20

class clTabCtrl;
enum NotebookStyle {
    /// Add a "+" button for creating new empty pages
    kNotebook_NewButton = (1 << 0),

    /// Hide the tab-bar
    kNotebook_HideTabBar = (1 << 1),

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

    /// Allow DnD between different book controls
    kNotebook_AllowForeignDnD = (1 << 10),

    /// Place the tabs on the right
    kNotebook_RightTabs = (1 << 11),

    /// Place th tabs on the left
    kNotebook_LeftTabs = (1 << 12),

    /// The notebook colours are changing based on the current editor theme
    kNotebook_DynamicColours = (1 << 13),

    /// Underline the active tab with a 2 pixel line
    kNotebook_UnderlineActiveTab = (1 << 14),

    /// When scrolling with the mouse button when hovering the tab control, switch between tabs
    kNotebook_MouseScrollSwitchTabs = (1 << 15),

    /// Default notebook
    kNotebook_Default = kNotebook_ShowFileListButton,

};

#define IS_VERTICAL_TABS(style) ((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs))

class clTabRenderer;
class WXDLLIMPEXP_SDK clTabColours
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

    /// Marker colour
    wxColour markerColour;

    clTabColours();
    virtual ~clTabColours() {}

    /**
     * @brief update colours based on the current theme
     */
    void UpdateColours(size_t notebookStyle);

    bool IsDarkColours() const;
};

/**
 * @class clTabInfo
 * @author Eran Ifrah
 * @brief contains information (mainly for drawing purposes) about a single tab label
 */
class WXDLLIMPEXP_SDK clTabInfo
{
    int m_bitmap = wxNOT_FOUND;
    int m_disabledBitmp = wxNOT_FOUND;

public:
    clTabCtrl* m_tabCtrl = nullptr;
    wxString m_label;
    wxString m_shortLabel;
    wxString m_tooltip;
    wxWindow* m_window = nullptr;
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
    int m_textWidth;
    eButtonState m_xButtonState = eButtonState::kNormal;

public:
    void CalculateOffsets(size_t style);
    void CalculateOffsets(size_t style, wxDC& dc);
    const wxString& GetBestLabel(size_t style) const;

public:
    typedef wxSharedPtr<clTabInfo> Ptr_t;
    typedef std::vector<clTabInfo::Ptr_t> Vec_t;

    clTabInfo(clTabCtrl* tabCtrl);
    clTabInfo(clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, int bitmapId = wxNOT_FOUND);
    virtual ~clTabInfo() {}

    void CreateDisabledBitmap();
    bool IsValid() const { return m_window != NULL; }
    void SetBitmap(int bitmap, size_t style);
    void SetLabel(const wxString& label, size_t style);
    void SetActive(bool active, size_t style);
    void SetRect(const wxRect& rect) { this->m_rect = rect; }
    int GetBitmap() const { return m_bitmap; }
    const wxString& GetLabel() const { return m_label; }
    const wxString& GetShortLabel() const { return m_shortLabel; }
    void SetShortLabel(const wxString& shortLabel) { this->m_shortLabel = shortLabel; }
    const wxRect& GetRect() const { return m_rect; }
    wxRect& GetRect() { return m_rect; }
    wxRect GetCloseButtonRect() const;
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
    int GetDisabledBitmp() const { return m_disabledBitmp; }
    const wxBitmap& GetBitmap(int index, bool disabled) const;
    bool HasDisableBitmap() const;
    bool HasBitmap() const;
};

class WXDLLIMPEXP_SDK clTabRenderer
{
public:
    typedef wxSharedPtr<clTabRenderer> Ptr_t;

    // Geometry
    int bottomAreaHeight;     // BOTTOM_AREA_HEIGHT = 3;
    int majorCurveWidth;      // MAJOR_CURVE_WIDTH = 15;
    int smallCurveWidth;      // SMALL_CURVE_WIDTH = 4;
    int overlapWidth;         // OVERLAP_WIDTH = 20;
    int verticalOverlapWidth; // V_OVERLAP_WIDTH = 3;
    int xSpacer;
    int ySpacer;
    wxString m_name;
    static std::unordered_map<wxString, clTabRenderer*> ms_Renderes;
    bool use_bold_font = false;

protected:
    void ClearActiveTabExtraLine(clTabInfo::Ptr_t activeTab, wxDC& dc, const clTabColours& colours, size_t style);
    void DrawMarker(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style);
    void DrawMarkerLine(wxDC& dc, const wxPoint& p1, const wxPoint& p2, wxDirection direction);
    static void RegisterRenderer(clTabRenderer* renderer);
    static clTabRenderer* Create(const wxWindow* parent, const wxString& name);

public:
    clTabRenderer(const wxString& name, const wxWindow* parent);
    virtual ~clTabRenderer() {}
    virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const clTabInfo& tabInfo, const clTabColours& colours,
                      size_t style, eButtonState buttonState) = 0;
    virtual void DrawBottomRect(wxWindow* parent, clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc,
                                const clTabColours& colours, size_t style) = 0;

    void SetUseBoldFont(bool use_bold_font) { this->use_bold_font = use_bold_font; }
    bool IsUseBoldFont() const { return use_bold_font; }

    /**
     * @brief draw the tab area background, return the colour used to actually
     * paint it
     */
    virtual wxColour DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const clTabColours& colours,
                                    size_t style);

    /**
     * @brief finalise the background after all elements have been drawn on the tab area colour. Default is
     * to do nothing
     */
    virtual void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const wxRect& activeTabRect,
                                    const clTabColours& colours, size_t style);
    /**
     * @brief reutrn font suitable for drawing the tab label
     */
    static wxFont GetTabFont(bool bold);

    /**
     * @brief draw a button in a given state at a give location
     */
    static void DrawButton(wxWindow* win, wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours,
                           eButtonState state);

    /**
     * @brief draw cheveron button
     */
    static void DrawChevron(wxWindow* win, wxDC& dc, const wxRect& rect, const clTabColours& colours);

    static int GetXButtonSize();
    /**
     * @brief Adjust colours per renderer
     * @param colours [in/out]
     * @param style the notebook style
     */
    virtual void AdjustColours(clTabColours& colours, size_t style);
    static int GetDefaultBitmapHeight(int Y_spacer);

    /**
     * @brief allocate new renderer based on CodeLite's settings
     */
    static clTabRenderer::Ptr_t CreateRenderer(const wxWindow* win, size_t tabStyle);
    /**
     * @brief return list of availale renderers
     */
    static wxArrayString GetRenderers();

    /**
     * @brief return the marker pen width
     * @return
     */
    static int GetMarkerWidth();
    void SetName(const wxString& name) { this->m_name = name; }
    const wxString& GetName() const { return m_name; }
    virtual clTabRenderer* New(const wxWindow* parent) const = 0;
    /**
     * @brief return true if this renderer supports vertical tabs
     */
    virtual bool IsVerticalTabSupported() const = 0;
};
#endif // CLTABRENDERER_H
