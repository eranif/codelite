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

#include <wx/colour.h>
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/sharedptr.h>
#include <vector>

class clTabCtrl;
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
    
    /// Underline the active tab with a 2 pixel line
    kNotebook_UnderlineActiveTab = (1 << 14),

    /// Default notebook
    kNotebook_Default = kNotebook_LightTabs | kNotebook_ShowFileListButton,
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

    // close button bitmaps (MUST be 12x12)
    wxBitmap closeButton;

    /// Chevron down arrow used as the button for showing tab list
    wxBitmap chevronDown;
    
    /// Marker colour
    wxColour markerColour;
    
    clTabColours();
    virtual ~clTabColours() {}

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

/**
 * @class clTabInfo
 * @author Eran Ifrah
 * @brief contains information (mainly for drawing purposes) about a single tab label
 */
class WXDLLIMPEXP_SDK clTabInfo
{
public:
    clTabCtrl* m_tabCtrl;
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
    void CalculateOffsets(size_t style);

public:
    typedef wxSharedPtr<clTabInfo> Ptr_t;
    typedef std::vector<clTabInfo::Ptr_t> Vec_t;

    clTabInfo(clTabCtrl* tabCtrl);
    clTabInfo(
        clTabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp = wxNullBitmap);
    virtual ~clTabInfo() {}

    bool IsValid() const { return m_window != NULL; }
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

public:
    clTabRenderer();
    virtual ~clTabRenderer() {}
    virtual void Draw(wxDC& dc, const clTabInfo& tabInfo, const clTabColours& colours, size_t style) = 0;
    virtual void DrawBottomRect(
        clTabInfo::Ptr_t activeTab, const wxRect& clientRect, wxDC& dc, const clTabColours& colours, size_t style) = 0;
};

#endif // CLTABRENDERER_H
