#include "Notebook.h"
#include <wx/sizer.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/wupdlock.h>
#include <algorithm>
#include <wx/image.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/menu.h>
#include <algorithm>
#include <wx/dnd.h>
#include <wx/regex.h>

#if defined(WXUSINGDLL_CL) || defined(USE_SFTP) || defined(PLUGINS_DIR)
#define CL_BUILD 1
#endif

#ifdef __WXMSW__
int clTabInfo::Y_SPACER = 3;
#else
int clTabInfo::Y_SPACER = 3;
#endif

int clTabInfo::X_SPACER = 5;
int clTabInfo::BOTTOM_AREA_HEIGHT = 3;
int clTabInfo::MAJOR_CURVE_WIDTH = 15;
int clTabInfo::SMALL_CURVE_WIDTH = 4;
// int clTabInfo::TAB_HEIGHT = 30;
static int OVERLAP_WIDTH = 20;
static int V_OVERLAP_WIDTH = 3;

#if CL_BUILD
#include "cl_command_event.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include "drawingutils.h"
#endif

wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_NAVIGATING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);

#define IS_VERTICAL_TABS(style) ((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs))

extern void Notebook_Init_Bitmaps();

Notebook::Notebook(wxWindow* parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
    : wxPanel(parent, id, pos, size, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL, name)
{
    static bool once = false;
    if(!once) {
        // Add PNG and Bitmap handler
        wxImage::AddHandler(new wxPNGHandler);
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        Notebook_Init_Bitmaps();
        once = true;
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_tabCtrl = new clTabCtrl(this, style);
    m_windows = new WindowStack(this);

    if(GetStyle() & kNotebook_BottomTabs) {
        sizer->Add(m_windows, 1, wxEXPAND);
        sizer->Add(m_tabCtrl, 0, wxEXPAND);

    } else {
        sizer->Add(m_tabCtrl, 0, wxEXPAND);
        sizer->Add(m_windows, 1, wxEXPAND);
    }
    Layout();
}

Notebook::~Notebook() {}

void Notebook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab(new clTabInfo(GetStyle(), page, label, bmp));
    tab->SetActive(selected, GetStyle());
    m_tabCtrl->AddPage(tab);
}

void Notebook::DoChangeSelection(wxWindow* page) { m_windows->Select(page); }

bool Notebook::InsertPage(size_t index, wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab(new clTabInfo(GetStyle(), page, label, bmp));
    tab->SetActive(selected, GetStyle());
    return m_tabCtrl->InsertPage(index, tab);
}

void Notebook::SetStyle(size_t style)
{
    m_tabCtrl->SetStyle(style);
    GetSizer()->Detach(m_windows);
    GetSizer()->Detach(m_tabCtrl);
    wxSizer* sz = NULL;

    // Replace the sizer
    if(IsVerticalTabs()) {
        sz = new wxBoxSizer(wxHORIZONTAL);
    } else {
        sz = new wxBoxSizer(wxVERTICAL);
    }

    if((style & kNotebook_BottomTabs) || (style & kNotebook_RightTabs)) {
        sz->Add(m_windows, 1, wxEXPAND);
        sz->Add(m_tabCtrl, 0, wxEXPAND);

    } else {
        sz->Add(m_tabCtrl, 0, wxEXPAND);
        sz->Add(m_windows, 1, wxEXPAND);
    }
    SetSizer(sz);
    Layout();
    m_tabCtrl->Refresh();
}

wxWindow* Notebook::GetCurrentPage() const
{
    if(m_tabCtrl->GetSelection() == wxNOT_FOUND) return NULL;
    return m_tabCtrl->GetPage(m_tabCtrl->GetSelection());
}

int Notebook::FindPage(wxWindow* page) const { return m_tabCtrl->FindPage(page); }

bool Notebook::RemovePage(size_t page, bool notify) { return m_tabCtrl->RemovePage(page, notify, false); }
bool Notebook::DeletePage(size_t page) { return m_tabCtrl->RemovePage(page, true, true); }
bool Notebook::DeleteAllPages() { return m_tabCtrl->DeleteAllPages(); }

void Notebook::EnableStyle(NotebookStyle style, bool enable)
{
    size_t flags = GetStyle();
    if(enable) {
        flags |= style;
    } else {
        flags &= ~style;
    }
    SetStyle(flags);
}

void Notebook::SetTabDirection(wxDirection d)
{
    size_t flags = GetStyle();
    // Clear all direction styles
    flags &= ~kNotebook_BottomTabs;
    flags &= ~kNotebook_LeftTabs;
    flags &= ~kNotebook_RightTabs;

    if(d == wxBOTTOM) {
        flags |= kNotebook_BottomTabs;
    } else if(d == wxRIGHT) {
        flags |= kNotebook_RightTabs;
    } else if(d == wxLEFT) {
        flags |= kNotebook_LeftTabs;
    }
    SetStyle(flags);
}

//----------------------------------------------------------
// Tab label
//----------------------------------------------------------
void clTabInfo::Draw(wxDC& dc, const clTabInfo::Colours& colours, size_t style)
{
    const int TOP_SMALL_HEIGHT = 2;
    wxColour bgColour(IsActive() ? colours.activeTabBgColour : colours.inactiveTabBgColour);
    wxColour penColour(IsActive() ? colours.activeTabPenColour : colours.inactiveTabPenColour);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    dc.SetTextForeground(IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);
    dc.SetFont(font);

    if(style & kNotebook_BottomTabs) {
        // Bottom tabs
        {
            wxPoint points[6];
            points[0] = m_rect.GetTopLeft();

            points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
            points[1].y = m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT;

            points[2].x = points[1].x + SMALL_CURVE_WIDTH;
            points[2].y = points[1].y + TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (m_rect.GetWidth() - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
            points[3].y = points[2].y;

            points[4].x = points[3].x + SMALL_CURVE_WIDTH;
            points[4].y = points[1].y;

            points[5] = m_rect.GetTopRight();

            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }
        {
            wxPoint points[6];
            points[0] = m_rect.GetTopLeft();
            points[0].x += 1;

            points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
            points[1].y = m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT - 1;

            points[2].x = points[1].x + SMALL_CURVE_WIDTH;
            points[2].y = points[1].y + TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (m_rect.GetWidth() - 2 - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
            points[3].y = points[2].y;

            points[4].x = points[3].x + SMALL_CURVE_WIDTH;
            points[4].y = points[1].y;

            points[5] = m_rect.GetTopRight();
            points[5].x -= 2;

            dc.SetPen(IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }
    } else if(IS_VERTICAL_TABS(style)) {

        // Left side tabs
        wxRect rotatedRect(0, 0, m_rect.GetHeight(), m_rect.GetWidth());
        wxBitmap b(rotatedRect.GetSize());

        wxMemoryDC tmpDC(b);
        tmpDC.SetPen(colours.tabAreaColour);
        tmpDC.SetBrush(colours.tabAreaColour);
        tmpDC.DrawRectangle(rotatedRect);
        tmpDC.SetFont(font);
        tmpDC.SetTextForeground(IsActive() ? colours.activeTabTextColour : colours.inactiveTabTextColour);

        tmpDC.SetPen(penColour);
        tmpDC.SetBrush(bgColour);
        if(IsActive()) {
            {
                wxPoint points[6];
                points[0] = rotatedRect.GetBottomLeft();

                points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
                points[1].y = rotatedRect.GetLeftTop().y + TOP_SMALL_HEIGHT;

                points[2].x = points[1].x + SMALL_CURVE_WIDTH;
                points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                points[3].x = points[0].x + (rotatedRect.GetWidth() - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
                points[3].y = points[2].y;

                points[4].x = points[3].x + SMALL_CURVE_WIDTH;
                points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                points[5] = rotatedRect.GetBottomRight();

                tmpDC.SetPen(penColour);
                tmpDC.SetBrush(bgColour);
                tmpDC.DrawPolygon(6, points);
            }

            {
                wxPoint points[6];
                points[0] = rotatedRect.GetBottomLeft();
                points[0].x += 1;

                points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
                points[1].y = rotatedRect.GetLeftTop().y + TOP_SMALL_HEIGHT + 1;

                points[2].x = points[1].x + SMALL_CURVE_WIDTH;
                points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                points[3].x = points[0].x + (rotatedRect.GetWidth() - 2 - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
                points[3].y = points[2].y;

                points[4].x = points[3].x + SMALL_CURVE_WIDTH;
                points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                points[5] = rotatedRect.GetBottomRight();
                points[5].x -= 2;

                tmpDC.SetPen(IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
                tmpDC.SetBrush(bgColour);
                tmpDC.DrawPolygon(6, points);
            }
        } else {
            // Inactive tabs
            wxDirection direction = wxNORTH;
            wxPoint basePoint = ((style & kNotebook_LeftTabs) ? rotatedRect.GetLeftTop() : rotatedRect.GetRightTop());
            {
                wxPoint pt = basePoint;
                pt.x -= 1;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabPenColour, bgColour, direction);
            }
            {
                wxPoint pt = basePoint;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabInnerPenColour, bgColour, direction);
            }
            {
                wxPoint pt = basePoint;
                pt.x += 1;
                wxRect gr(pt, wxSize(1, rotatedRect.GetHeight()));
                tmpDC.SetPen(*wxTRANSPARENT_PEN);
                tmpDC.GradientFillLinear(gr, colours.inactiveTabPenColour, bgColour, direction);
            }
        }

        // Vertical tabs
        // Draw bitmap
        if(m_bitmap.IsOk()) {
            tmpDC.DrawBitmap(m_bitmap, m_bmpY, m_bmpX);
        }

        tmpDC.DrawText(m_label, m_textY, m_textX);
        if(IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            tmpDC.DrawBitmap(colours.closeButton, m_bmpCloseY, m_bmpCloseX);
        }
        tmpDC.SelectObject(wxNullBitmap);
        wxImage img = b.ConvertToImage();
        img = img.Rotate90((style & kNotebook_RightTabs));
        b = wxBitmap(img);
        dc.DrawBitmap(b, m_rect.GetTopLeft());

    } else {
        // Default tabs (placed at the top)
        {
            wxPoint points[6];
            points[0] = m_rect.GetBottomLeft();

            points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
            points[1].y = m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT;

            points[2].x = points[1].x + SMALL_CURVE_WIDTH;
            points[2].y = points[1].y - TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (m_rect.GetWidth() - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
            points[3].y = points[2].y;

            points[4].x = points[3].x + SMALL_CURVE_WIDTH;
            points[4].y = points[3].y + TOP_SMALL_HEIGHT;

            points[5] = m_rect.GetBottomRight();

            dc.SetPen(penColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }

        {
            wxPoint points[6];
            points[0] = m_rect.GetBottomLeft();
            points[0].x += 1;

            points[1].x = points[0].x + MAJOR_CURVE_WIDTH;
            points[1].y = m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT + 1;

            points[2].x = points[1].x + SMALL_CURVE_WIDTH;
            points[2].y = points[1].y - TOP_SMALL_HEIGHT;

            points[3].x = points[0].x + (m_rect.GetWidth() - 2 - (MAJOR_CURVE_WIDTH + SMALL_CURVE_WIDTH));
            points[3].y = points[2].y;

            points[4].x = points[3].x + SMALL_CURVE_WIDTH;
            points[4].y = points[3].y + TOP_SMALL_HEIGHT;

            points[5] = m_rect.GetBottomRight();
            points[5].x -= 2;

            dc.SetPen(IsActive() ? colours.activeTabInnerPenColour : colours.inactiveTabInnerPenColour);
            dc.SetBrush(bgColour);
            dc.DrawPolygon(6, points);
        }
    }

    if(!IS_VERTICAL_TABS(style)) {
        // Draw bitmap
        if(m_bitmap.IsOk()) {
            dc.DrawBitmap(m_bitmap, m_bmpX + m_rect.GetX(), m_bmpY);
        }
        dc.DrawText(m_label, m_textX + m_rect.GetX(), m_textY);
        if(IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
            dc.DrawBitmap(colours.closeButton, m_bmpCloseX + m_rect.GetX(), m_bmpCloseY);
        }
    }
}

clTabInfo::clTabInfo(size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp)
    : m_label(text)
    , m_bitmap(bmp)
    , m_window(page)
    , m_active(false)
{
    CalculateOffsets(style);
}

clTabInfo::clTabInfo()
    : m_window(NULL)
    , m_active(false)
    , m_textX(wxNOT_FOUND)
    , m_textY(wxNOT_FOUND)
    , m_bmpX(wxNOT_FOUND)
    , m_bmpY(wxNOT_FOUND)
    , m_bmpCloseX(wxNOT_FOUND)
    , m_bmpCloseY(wxNOT_FOUND)
{

    CalculateOffsets(0);
}

void clTabInfo::CalculateOffsets(size_t style)
{
    wxBitmap b(1, 1);
    wxMemoryDC memDC(b);
    m_bmpCloseX = wxNOT_FOUND;
    m_bmpCloseY = wxNOT_FOUND;

    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    memDC.SetFont(font);

    wxSize sz = memDC.GetTextExtent(m_label);
    wxSize fixedHeight = memDC.GetTextExtent("Tp");
    if(IS_VERTICAL_TABS(style)) {
        m_height = fixedHeight.GetHeight() + (5 * Y_SPACER);
    } else {
        m_height = fixedHeight.GetHeight() + (4 * Y_SPACER);
    }

#ifdef __WXGTK__
    // On GTK, limit the tab height
    if(m_height >= 30) {
        m_height = 30;
    }
#endif

    m_width = 0;
    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += MAJOR_CURVE_WIDTH;
        m_width += SMALL_CURVE_WIDTH;
    }
    m_width += X_SPACER;

    // bitmap
    m_bmpX = wxNOT_FOUND;
    m_bmpY = wxNOT_FOUND;

    if(m_bitmap.IsOk()) {
        m_bmpX = m_width;
        m_width += X_SPACER;
        m_width += m_bitmap.GetWidth();
        m_bmpY = ((m_height - m_bitmap.GetHeight()) / 2);
    }

    // Text
    m_textX = m_width;
    m_textY = ((m_height - sz.y) / 2);
    m_width += sz.x;

    // x button
    if((style & kNotebook_CloseButtonOnActiveTab)) {
        m_width += X_SPACER;
        m_bmpCloseX = m_width;
        m_bmpCloseY = ((m_height - 12) / 2);
        m_width += 12; // X button is 10 pixels in size
    }

    m_width += X_SPACER;
    if(!IS_VERTICAL_TABS(style) || true) {
        m_width += MAJOR_CURVE_WIDTH;
        m_width += SMALL_CURVE_WIDTH;
    }

    // Update the rect width
    m_rect.SetWidth(m_width);

    if((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs)) {

        // swap the x and y coordinates
        wxSwap(m_height, m_width);
        wxSwap(m_bmpCloseY, m_bmpCloseX);
        wxSwap(m_bmpY, m_bmpX);
        wxSwap(m_textX, m_textY);

        m_rect.SetWidth(m_width);
        m_rect.SetHeight(m_height);
        m_rect.SetX(0);
        m_rect.SetY(0);
    }
}

void clTabInfo::SetBitmap(const wxBitmap& bitmap, size_t style)
{
    this->m_bitmap = bitmap;
    CalculateOffsets(style);
}

void clTabInfo::SetLabel(const wxString& label, size_t style)
{
    this->m_label = label;
    CalculateOffsets(style);
}

void clTabInfo::SetActive(bool active, size_t style)
{
    this->m_active = active;
    CalculateOffsets(style);
}

//----------------------------------------------------------
// Notebook header
//----------------------------------------------------------
// -------------------------------------------------------------------------------
// clTabCtrl class.
// This is where things are actually getting done
// -------------------------------------------------------------------------------
clTabCtrl::clTabCtrl(wxWindow* notebook, size_t style)
    : wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL)
    , m_style(style)
    , m_closeButtonClickedIndex(wxNOT_FOUND)
    , m_contextMenu(NULL)
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    memDC.SetFont(font);

    wxSize sz = memDC.GetTextExtent("Tp");
    m_height = sz.GetHeight() + (4 * clTabInfo::Y_SPACER);
    m_vTabsWidth = sz.GetHeight() + (5 * clTabInfo::Y_SPACER);
#ifdef __WXGTK__
    // On GTK, limit the tab height
    if(m_height >= 30) {
        m_height = 30;
    }
#endif
    SetDropTarget(new clTabCtrlDropTarget(this));
    if(IsVerticalTabs()) {
        SetSizeHints(wxSize(m_vTabsWidth, -1));
        SetSize(m_vTabsWidth, -1);
    } else {
        SetSizeHints(wxSize(-1, m_height));
        SetSize(-1, m_height);
    }

    Bind(wxEVT_PAINT, &clTabCtrl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clTabCtrl::OnEraseBG, this);
    Bind(wxEVT_SIZE, &clTabCtrl::OnSize, this);
    Bind(wxEVT_LEFT_DOWN, &clTabCtrl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clTabCtrl::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clTabCtrl::OnMouseMotion, this);
    Bind(wxEVT_MIDDLE_UP, &clTabCtrl::OnMouseMiddleClick, this);
    Bind(wxEVT_CONTEXT_MENU, &clTabCtrl::OnContextMenu, this);
    Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    Bind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);

    notebook->Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    if(m_style & kNotebook_DarkTabs) {
        m_colours.InitDarkColours();
    } else {
        m_colours.InitLightColours();
    }
    // The history object
    m_history.reset(new clTabHistory());
}

bool clTabCtrl::ShiftRight(clTabInfo::Vec_t& tabs)
{
    // Move the first tab from the list and adjust the remainder
    // of the tabs x coordiate
    if(!tabs.empty()) {
        clTabInfo::Ptr_t t = tabs.at(0);
        int width = t->GetWidth();
        tabs.erase(tabs.begin() + 0);

        for(size_t i = 0; i < tabs.size(); ++i) {
            clTabInfo::Ptr_t t = tabs.at(i);
            t->GetRect().SetX(t->GetRect().x - width + OVERLAP_WIDTH);
        }
        return true;
    }
    return false;
}

bool clTabCtrl::IsActiveTabInList(const clTabInfo::Vec_t& tabs) const
{
    for(size_t i = 0; i < tabs.size(); ++i) {
        if(tabs.at(i)->IsActive()) return true;
    }
    return false;
}

bool clTabCtrl::IsActiveTabVisible(const clTabInfo::Vec_t& tabs) const
{
    wxRect clientRect(GetClientRect());
    if((GetStyle() & kNotebook_ShowFileListButton) && !IsVerticalTabs()) {
        clientRect.SetWidth(clientRect.GetWidth() - 30);
    } else if((GetStyle() & kNotebook_ShowFileListButton) && IsVerticalTabs()) {
        // Vertical tabs
        clientRect.SetHeight(clientRect.GetHeight() - 30);
    }

    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t t = tabs.at(i);
        if(t->IsActive() && clientRect.Intersects(t->GetRect())) return true;
    }
    return false;
}

clTabCtrl::~clTabCtrl()
{
    wxDELETE(m_contextMenu);
    Unbind(wxEVT_PAINT, &clTabCtrl::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clTabCtrl::OnEraseBG, this);
    Unbind(wxEVT_SIZE, &clTabCtrl::OnSize, this);
    Unbind(wxEVT_LEFT_DOWN, &clTabCtrl::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clTabCtrl::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clTabCtrl::OnMouseMotion, this);
    Unbind(wxEVT_MIDDLE_UP, &clTabCtrl::OnMouseMiddleClick, this);
    Unbind(wxEVT_CONTEXT_MENU, &clTabCtrl::OnContextMenu, this);
    Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    Unbind(wxEVT_LEFT_DCLICK, &clTabCtrl::OnLeftDClick, this);
    GetParent()->Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
}

void clTabCtrl::OnWindowKeyDown(wxKeyEvent& event)
{
    if(GetStyle() & kNotebook_EnableNavigationEvent) {
        if(event.ControlDown()) {
            switch(event.GetKeyCode()) {
            case WXK_TAB:
            case WXK_PAGEDOWN:
            case WXK_PAGEUP: {
                // Fire the navigation event
                wxBookCtrlEvent e(wxEVT_BOOK_NAVIGATING);
                e.SetEventObject(GetParent());
                GetParent()->GetEventHandler()->AddPendingEvent(e);
                return;
            }
            default:
                break;
            }
        }
    }
    event.Skip();
}

void clTabCtrl::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_visibleTabs.clear();
    Refresh();
}

void clTabCtrl::OnEraseBG(wxEraseEvent& e) { wxUnusedVar(e); }

void clTabCtrl::OnPaint(wxPaintEvent& e)
{
    wxBufferedPaintDC dc(this);
    wxRect clientRect(GetClientRect());
    if(clientRect.width <= 3) return;
    if(clientRect.height <= 3) return;

    m_chevronRect = wxRect();
    wxRect rect(GetClientRect());

    if((GetStyle() & kNotebook_ShowFileListButton)) {
        if(IsVerticalTabs()) {
            rect.SetHeight(rect.GetHeight() - 16);
            m_chevronRect = wxRect(rect.GetBottomLeft(), wxSize(rect.GetWidth(), 20));
            if(GetStyle() & kNotebook_RightTabs) {
                m_chevronRect.x += clTabInfo::BOTTOM_AREA_HEIGHT;
            } else {
                m_chevronRect.x -= 2;
            }
            m_chevronRect.SetWidth(m_chevronRect.GetWidth() - clTabInfo::BOTTOM_AREA_HEIGHT);
            rect.SetHeight(rect.GetHeight() + 16);
        } else {
            // Reduce the length of the tabs bitmap by 16 pixels (we will draw there the drop down
            // button)
            rect.SetWidth(rect.GetWidth() - 16);
            m_chevronRect = wxRect(rect.GetTopRight(), wxSize(20, rect.GetHeight()));
            if(GetStyle() & kNotebook_BottomTabs) {
                m_chevronRect.y += clTabInfo::BOTTOM_AREA_HEIGHT;
            }
            m_chevronRect.SetHeight(m_chevronRect.GetHeight() - clTabInfo::BOTTOM_AREA_HEIGHT);
            rect.SetWidth(rect.GetWidth() + 16);
        }
    }

    if(m_tabs.empty()) {
        // Draw the default bg colour
        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        dc.DrawRectangle(GetClientRect());
        return;
    }

    // Draw the tab area background colours
    clTabInfo::Ptr_t active_tab = GetActiveTabInfo();
    wxColour tabAreaBgCol = m_colours.tabAreaColour;
    clTabInfo::Colours activeTabColours = m_colours;

#if CL_BUILD
    if(active_tab && (GetStyle() & kNotebook_EnableColourCustomization)) {
        // the background colour is set according to the active tab colour
        clColourEvent colourEvent(wxEVT_COLOUR_TAB);
        colourEvent.SetPage(active_tab->GetWindow());
        if(EventNotifier::Get()->ProcessEvent(colourEvent)) {
            tabAreaBgCol = colourEvent.GetBgColour();
            activeTabColours.InitFromColours(colourEvent.GetBgColour(), colourEvent.GetFgColour());
        }
    }
#endif

    // Draw background
    dc.SetPen(tabAreaBgCol);
    dc.SetBrush(tabAreaBgCol);
    dc.DrawRectangle(GetClientRect());

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs.at(i)->CalculateOffsets(GetStyle());
    }

    if(rect.GetSize().x > 0 && rect.GetSize().y > 0) {
        // wxBitmap bmpTabs(rect.GetSize());
        // wxMemoryDC memDC(bmpTabs);
        wxGCDC gcdc(dc);

        gcdc.SetPen(tabAreaBgCol);
        gcdc.SetBrush(tabAreaBgCol);
        gcdc.DrawRectangle(rect.GetSize());

        UpdateVisibleTabs();

        int activeTabInex = wxNOT_FOUND;
        for(int i = (m_visibleTabs.size() - 1); i >= 0; --i) {
            clTabInfo::Ptr_t tab = m_visibleTabs.at(i);
            if(tab->IsActive()) {
                activeTabInex = i;
            }

#if CL_BUILD
            // send event per tab to get their colours
            clColourEvent colourEvent(wxEVT_COLOUR_TAB);
            colourEvent.SetPage(tab->GetWindow());
            if((GetStyle() & kNotebook_EnableColourCustomization) && EventNotifier::Get()->ProcessEvent(colourEvent)) {
                clTabInfo::Colours colours;
                colours.InitFromColours(colourEvent.GetBgColour(), colourEvent.GetFgColour());
                tab->Draw(gcdc, colours, m_style);
            } else {
                tab->Draw(gcdc, m_colours, m_style);
            }
#else
            tab->Draw(gcdc, m_colours, m_style);
#endif
        }

        // Redraw the active tab
        if(activeTabInex != wxNOT_FOUND) {
            m_visibleTabs.at(activeTabInex)->Draw(gcdc, activeTabColours, m_style);
        }

        if(activeTabInex != wxNOT_FOUND) {
            clTabInfo::Ptr_t activeTab = m_visibleTabs.at(activeTabInex);
            if(!(GetStyle() & kNotebook_VerticalButtons)) {
                DoDrawBottomBox(activeTab, clientRect, gcdc, activeTabColours);
            }
        }

        if((GetStyle() & kNotebook_ShowFileListButton)) {
            // Draw the chevron
            wxCoord chevronX =
                m_chevronRect.GetTopLeft().x + ((m_chevronRect.GetWidth() - m_colours.chevronDown.GetWidth()) / 2);
            wxCoord chevronY =
                m_chevronRect.GetTopLeft().y + ((m_chevronRect.GetHeight() - m_colours.chevronDown.GetHeight()) / 2);
            // dc.SetPen(activeTabColours.tabAreaColour);
            // dc.SetBrush(*wxTRANSPARENT_BRUSH);
            // dc.DrawRectangle(m_chevronRect);
            dc.DrawBitmap(m_colours.chevronDown, chevronX, chevronY);
        }

    } else {
        m_visibleTabs.clear();
    }
}

void clTabCtrl::DoUpdateCoordiantes(clTabInfo::Vec_t& tabs)
{
    int majorDimension = 5;
    if(IsVerticalTabs()) {
        majorDimension = 0;
    }

    for(size_t i = 0; i < tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = tabs.at(i);
        if(IsVerticalTabs()) {
            if(GetStyle() & kNotebook_VerticalButtons) {
                tab->GetRect().SetX(0);
            } else {
                tab->GetRect().SetX(GetStyle() & kNotebook_LeftTabs ? 1 : 0);
            }
            tab->GetRect().SetY(majorDimension);
            tab->GetRect().SetWidth(tab->GetWidth());
            tab->GetRect().SetHeight(tab->GetHeight());
            majorDimension += tab->GetHeight() - V_OVERLAP_WIDTH;
        } else {
            tab->GetRect().SetX(majorDimension);
            tab->GetRect().SetY(0);
            tab->GetRect().SetWidth(tab->GetWidth());
            tab->GetRect().SetHeight(tab->GetHeight());
            majorDimension += tab->GetWidth() - OVERLAP_WIDTH;
        }
    }
}

void clTabCtrl::UpdateVisibleTabs()
{
    // don't update the list if we don't need to
    if(!IsVerticalTabs()) {
        if(IsActiveTabInList(m_visibleTabs) && IsActiveTabVisible(m_visibleTabs)) return;
    }

    // set the physical coords for each tab (we do this for all the tabs)
    DoUpdateCoordiantes(m_tabs);

    // Start shifting right tabs until the active tab is visible
    m_visibleTabs = m_tabs;
    if(!IsVerticalTabs()) {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftRight(m_visibleTabs)) break;
        }
    } else {
        while(!IsActiveTabVisible(m_visibleTabs)) {
            if(!ShiftBottom(m_visibleTabs)) break;
        }
    }
}

void clTabCtrl::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    m_closeButtonClickedIndex = wxNOT_FOUND;

    if((GetStyle() & kNotebook_ShowFileListButton) && m_chevronRect.Contains(event.GetPosition())) {
        // we will handle this later in the "Mouse Up" event
        return;
    }

    int tabHit, realPos;
    TestPoint(event.GetPosition(), realPos, tabHit);
    if(tabHit == wxNOT_FOUND) return;

    // Did we hit the active tab?
    bool clickWasOnActiveTab = (GetSelection() == realPos);

    // If the click was not on the active tab, set the clicked
    // tab as the new selection and leave this function
    if(!clickWasOnActiveTab) {
        SetSelection(realPos);
        return;
    }

    // If we clicked on the active and we have a close button - handle it here
    if((GetStyle() & kNotebook_CloseButtonOnActiveTab) && clickWasOnActiveTab) {
        // we clicked on the selected index
        clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
        wxRect xRect(t->GetRect().x + t->GetBmpCloseX(), t->GetRect().y + t->GetBmpCloseY(), 16, 16);
        if(xRect.Contains(event.GetPosition())) {
            m_closeButtonClickedIndex = tabHit;
            return;
        }
    }

    // We clicked on the active tab, start DnD operation
    if((m_style & kNotebook_AllowDnD) && clickWasOnActiveTab) {
        // We simply drag the active tab index
        wxString dragText;
        dragText << "{Class:Notebook,TabIndex:" << GetSelection() << "}";
        wxTextDataObject dragContent(dragText);
        wxDropSource dragSource(this);
        dragSource.SetData(dragContent);
        wxDragResult result = dragSource.DoDragDrop(true);
        wxUnusedVar(result);
    }
}

int clTabCtrl::ChangeSelection(size_t tabIdx)
{
    wxWindowUpdateLocker locker(GetParent());
    int oldSelection = GetSelection();
    if(!IsIndexValid(tabIdx)) return oldSelection;

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        tab->SetActive((i == tabIdx), GetStyle());
    }

    clTabInfo::Ptr_t activeTab = GetActiveTabInfo();
    if(activeTab) {
        static_cast<Notebook*>(GetParent())->DoChangeSelection(activeTab->GetWindow());
        activeTab->GetWindow()->CallAfter(&wxWindow::SetFocus);
    }

    Refresh();
    return oldSelection;
}

int clTabCtrl::SetSelection(size_t tabIdx)
{
#ifdef __WXMSW__
    DoChangeSelection(tabIdx);
#else
    CallAfter(&clTabCtrl::DoChangeSelection, tabIdx);
#endif
    return wxNOT_FOUND;
}

int clTabCtrl::GetSelection() const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->IsActive()) return i;
    }
    return wxNOT_FOUND;
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index)
{
    if(!IsIndexValid(index)) return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(size_t index) const
{
    if(!IsIndexValid(index)) return clTabInfo::Ptr_t(NULL);
    return m_tabs.at(index);
}

clTabInfo::Ptr_t clTabCtrl::GetTabInfo(wxWindow* page)
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        if(tab->GetWindow() == page) return tab;
    }
    return clTabInfo::Ptr_t(NULL);
}

bool clTabCtrl::SetPageText(size_t page, const wxString& text)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(!tab) return false;
    tab->SetLabel(text, GetStyle());
    Refresh();
    return true;
}

clTabInfo::Ptr_t clTabCtrl::GetActiveTabInfo()
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->IsActive()) {
            return m_tabs.at(i);
        }
    }
    return clTabInfo::Ptr_t(NULL);
}

void clTabCtrl::AddPage(clTabInfo::Ptr_t tab) { InsertPage(m_tabs.size(), tab); }

WindowStack* clTabCtrl::GetStack() { return reinterpret_cast<Notebook*>(GetParent())->m_windows; }

bool clTabCtrl::InsertPage(size_t index, clTabInfo::Ptr_t tab)
{
    int oldSelection = GetSelection();
    if(index > m_tabs.size()) return false;
    m_tabs.insert(m_tabs.begin() + index, tab);
    bool sendPageChangedEvent = (oldSelection == wxNOT_FOUND) || tab->IsActive();

    int tabIndex = index;
    GetStack()->Add(tab->GetWindow(), tab->IsActive());
    if(sendPageChangedEvent) {
        ChangeSelection(tabIndex);

        // Send an event
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        event.SetOldSelection(oldSelection);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }

    tab->GetWindow()->Bind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);
    m_history->Push(tab->GetWindow());
    Refresh();
    return true;
}

wxString clTabCtrl::GetPageText(size_t page) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(tab) return tab->GetLabel();
    return "";
}

wxBitmap clTabCtrl::GetPageBitmap(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) return tab->GetBitmap();
    return wxNullBitmap;
}

void clTabCtrl::SetPageBitmap(size_t index, const wxBitmap& bmp)
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) {

        int oldWidth = tab->GetWidth();
        tab->SetBitmap(bmp, GetStyle());
        int newWidth = tab->GetWidth();
        int diff = (newWidth - oldWidth);

        // Update the coordinates starting from the current tab
        clTabInfo::Vec_t tabsToUpdate;
        bool foundActiveTab = false;
        for(size_t i = 0; i < m_tabs.size(); ++i) {
            if(!foundActiveTab && (m_tabs.at(i)->GetWindow() == tab->GetWindow())) {
                foundActiveTab = true;
            } else if(foundActiveTab) {
                m_tabs.at(i)->GetRect().SetX(m_tabs.at(i)->GetRect().GetX() + diff);
            }
        }
        Refresh();
    }
}

void clTabCtrl::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();

    // First check if the chevron was clicked. We do this because the chevron could overlap the buttons drawing
    // area
    if((GetStyle() & kNotebook_ShowFileListButton) && m_chevronRect.Contains(event.GetPosition())) {
        // Show the drop down list
        CallAfter(&clTabCtrl::DoShowTabList);

    } else {
        int tabHit, realPos;
        TestPoint(event.GetPosition(), realPos, tabHit);
        if(tabHit != wxNOT_FOUND) {
            if((GetStyle() & kNotebook_CloseButtonOnActiveTab) && m_visibleTabs.at(tabHit)->IsActive()) {
                // we clicked on the selected index
                clTabInfo::Ptr_t t = m_visibleTabs.at(tabHit);
                wxRect xRect(t->GetRect().x + t->GetBmpCloseX(), t->GetRect().y + t->GetBmpCloseY(), 16, 16);
                xRect.Inflate(2); // don't be picky if we did not click exactly on the 16x16 bitmap...

                if(m_closeButtonClickedIndex == tabHit && xRect.Contains(event.GetPosition())) {
                    if(GetStyle() & kNotebook_CloseButtonOnActiveTabFireEvent) {
                        // let the user process this
                        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
                        event.SetEventObject(GetParent());
                        event.SetSelection(realPos);
                        GetParent()->GetEventHandler()->AddPendingEvent(event);
                    } else {
                        CallAfter(&clTabCtrl::DoDeletePage, realPos);
                    }
                }
            }
        }
    }
}

void clTabCtrl::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    int realPos, tabHit;
    wxString curtip = GetToolTipText();
    TestPoint(event.GetPosition(), realPos, tabHit);
    if(tabHit == wxNOT_FOUND || realPos == wxNOT_FOUND) {
        if(!curtip.IsEmpty()) {
            SetToolTip("");
        }
    } else {
        wxString pagetip = m_tabs.at(realPos)->GetTooltip();
        if(pagetip != curtip) {
            SetToolTip(pagetip);
        }
    }
}

void clTabCtrl::TestPoint(const wxPoint& pt, int& realPosition, int& tabHit)
{
    realPosition = wxNOT_FOUND;
    tabHit = wxNOT_FOUND;

    if(m_visibleTabs.empty()) return;

    // Because the tabs are overlapping, we need to test
    // the active tab first
    clTabInfo::Ptr_t activeTab = GetActiveTabInfo();
    if(activeTab && activeTab->GetRect().Contains(pt)) {
        for(size_t i = 0; i < m_visibleTabs.size(); ++i) {
            if(m_visibleTabs.at(i)->GetWindow() == activeTab->GetWindow()) {
                tabHit = i;
                realPosition = DoGetPageIndex(m_visibleTabs.at(i)->GetWindow());
                return;
            }
        }
    }

    for(size_t i = 0; i < m_visibleTabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_visibleTabs.at(i);
        if(tab->GetRect().Contains(pt)) {
            tabHit = i;
            realPosition = DoGetPageIndex(tab->GetWindow());
            return;
        }
    }
}

void clTabCtrl::SetStyle(size_t style)
{
    this->m_style = style;
    if(IsVerticalTabs()) {
        SetSizeHints(wxSize(m_vTabsWidth, -1));
        SetSize(m_vTabsWidth, -1);
    } else {
        SetSizeHints(wxSize(-1, m_height));
        SetSize(-1, m_height);
    }

    if(style & kNotebook_DarkTabs) {
        m_colours.InitDarkColours();
    } else {
        m_colours.InitLightColours();
    }

    for(size_t i = 0; i < m_tabs.size(); ++i) {
        m_tabs.at(i)->CalculateOffsets(GetStyle());
    }
    m_visibleTabs.clear();
    Layout();
    Refresh();
}

wxWindow* clTabCtrl::GetPage(size_t index) const
{
    clTabInfo::Ptr_t tab = GetTabInfo(index);
    if(tab) return tab->GetWindow();
    return NULL;
}

bool clTabCtrl::IsIndexValid(size_t index) const { return (index < m_tabs.size()); }

clTabInfo::Colours::Colours() { InitDarkColours(); }

void clTabInfo::Colours::InitFromColours(const wxColour& baseColour, const wxColour& textColour)
{
#if CL_BUILD
    if(DrawingUtils::IsDark(baseColour)) {
        activeTabTextColour = "WHITE";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = baseColour.ChangeLightness(120);

        inactiveTabTextColour = "WHITE";
        inactiveTabBgColour = baseColour.ChangeLightness(110);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = inactiveTabBgColour.ChangeLightness(120);

        tabAreaColour = baseColour.ChangeLightness(130);
        // 12x12 bitmap
        closeButton = wxXmlResource::Get()->LoadBitmap("notebook-dark-x");
        chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-grey");
    } else {
        activeTabTextColour = "BLACK";
        activeTabBgColour = baseColour;
        activeTabPenColour = baseColour.ChangeLightness(80);
        activeTabInnerPenColour = "WHITE";

        inactiveTabTextColour = "BLACK";
        inactiveTabBgColour = baseColour.ChangeLightness(90);
        inactiveTabPenColour = inactiveTabBgColour.ChangeLightness(80);
        inactiveTabInnerPenColour = baseColour;

        tabAreaColour = baseColour.ChangeLightness(130);
        // 12x12 bitmap
        closeButton = wxXmlResource::Get()->LoadBitmap("notebook-light-x");
        chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-black");
    }
#else
    wxUnusedVar(baseColour);
    wxUnusedVar(textColour);
#endif
}

void clTabInfo::Colours::InitDarkColours()
{
    activeTabTextColour = "WHITE";
    activeTabBgColour = wxColour("#211e1e");
    activeTabPenColour = wxColour("#0e0d0d");
    activeTabInnerPenColour = wxColour("#343131");

    inactiveTabTextColour = wxColour("rgb(200, 200, 200)");
    inactiveTabBgColour = wxColour("#393838");
    inactiveTabPenColour = wxColour("#100f0f");
    inactiveTabInnerPenColour = wxColour("#535252");

    tabAreaColour = wxColour("#131111").ChangeLightness(115);
    // 12x12 bitmap
    closeButton = wxXmlResource::Get()->LoadBitmap("notebook-dark-x");
    chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-grey");
}

void clTabInfo::Colours::InitLightColours()
{
    activeTabTextColour = "#444444";
    activeTabPenColour = "#b9b9b9";
    activeTabInnerPenColour = "#ffffff";

    inactiveTabTextColour = "#444444";
//#ifdef __WXMSW__
//    activeTabBgColour = "#FBFBFB";
//#else
    activeTabBgColour = "#f0f0f0";
//#endif
    inactiveTabBgColour = "#e5e5e5";
    inactiveTabPenColour = "#b9b9b9";
    inactiveTabInnerPenColour = "#ffffff";

    tabAreaColour = "#dcdcdc"; // wxColour("rgb(64, 64, 64)");
    // 12x12 bitmap
    closeButton = wxXmlResource::Get()->LoadBitmap("notebook-light-x");
    chevronDown = wxXmlResource::Get()->LoadBitmap("chevron-down-black");
}

int clTabCtrl::FindPage(wxWindow* page) const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == page) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool clTabCtrl::RemovePage(size_t page, bool notify, bool deletePage)
{
    wxWindow* nextSelection = NULL;
    if(!IsIndexValid(page)) return false;
    bool deletingSelection = ((int)page == GetSelection());

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
        event.SetEventObject(GetParent());
        event.SetSelection(page);
        GetParent()->GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    // Remove the tab from the "all-tabs" list
    clTabInfo::Ptr_t tab = m_tabs.at(page);
    m_tabs.erase(m_tabs.begin() + page);

    // Remove this page from the history
    m_history->Pop(tab->GetWindow());
    tab->GetWindow()->Unbind(wxEVT_KEY_DOWN, &clTabCtrl::OnWindowKeyDown, this);

    // Remove the tabs from the visible tabs list
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == tab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_visibleTabs.end()) {
        iter = m_visibleTabs.erase(iter);

        for(; iter != m_visibleTabs.end(); ++iter) {
            // update the remainding tabs coordinates
            if(IsVerticalTabs()) {
                (*iter)->GetRect().SetY((*iter)->GetRect().GetY() - tab->GetHeight() + V_OVERLAP_WIDTH);
            } else {
                (*iter)->GetRect().SetX((*iter)->GetRect().GetX() - tab->GetWidth() + OVERLAP_WIDTH);
            }
        }
    }

    // Choose a new selection, but only if we are deleting the active tab
    nextSelection = NULL;
    if(deletingSelection) {
        while(!m_history->GetHistory().IsEmpty() && !nextSelection) {
            nextSelection = m_history->PrevPage();
            if(!GetTabInfo(nextSelection)) {
                // The history contains a tab that no longer exists
                m_history->Pop(nextSelection);
                nextSelection = NULL;
            }
        }
        // It is OK to end up with a null next selection, we will handle it later
    }

    // Now remove the page from the notebook. We will delete the page
    // ourself, so there is no need to call DeletePage
    GetStack()->Remove(tab->GetWindow());
    if(deletePage) {
        // Destory the page
        tab->GetWindow()->Destroy();
    }

    if(notify) {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        GetParent()->GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) {
            // Vetoed
            return false;
        }
    }

    // Choose the next page
    if(deletingSelection) {
        // Always make sure we have something to select...
        if(!nextSelection && !m_tabs.empty()) {
            nextSelection = m_tabs.at(0)->GetWindow();
        }

        int nextSel = DoGetPageIndex(nextSelection);
        if(nextSel != wxNOT_FOUND) {
            ChangeSelection(nextSel);
            if(notify) {
                wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                event.SetEventObject(GetParent());
                event.SetSelection(GetSelection());
                GetParent()->GetEventHandler()->ProcessEvent(event);
            }
        } else {
            Refresh();
        }
    } else {
        Refresh();
    }
    return true;
}

int clTabCtrl::DoGetPageIndex(wxWindow* win) const
{
    if(!win) return wxNOT_FOUND;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetWindow() == win) return i;
    }
    return wxNOT_FOUND;
}

bool clTabCtrl::DeleteAllPages()
{
    GetStack()->Clear();
    m_tabs.clear();
    m_visibleTabs.clear();
    m_history->Clear();
    Refresh();
    return true;
}

void clTabCtrl::OnMouseMiddleClick(wxMouseEvent& event)
{
    event.Skip();
    if(GetStyle() & kNotebook_MouseMiddleClickClosesTab) {
        int realPos, tabHit;
        TestPoint(event.GetPosition(), realPos, tabHit);
        if(realPos != wxNOT_FOUND) {
            CallAfter(&clTabCtrl::DoDeletePage, realPos);
        }
    } else if(GetStyle() & kNotebook_MouseMiddleClickFireEvent) {
        int realPos, tabHit;
        TestPoint(event.GetPosition(), realPos, tabHit);
        if(realPos != wxNOT_FOUND) {
            // Just fire an event
            wxBookCtrlEvent e(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
            e.SetEventObject(GetParent());
            e.SetSelection(realPos);
            GetParent()->GetEventHandler()->AddPendingEvent(e);
        }
    }
}

void clTabCtrl::GetAllPages(std::vector<wxWindow*>& pages)
{
    std::for_each(
        m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t tabInfo) { pages.push_back(tabInfo->GetWindow()); });
}

void clTabCtrl::SetMenu(wxMenu* menu)
{
    wxDELETE(m_contextMenu);
    m_contextMenu = menu;
}

void clTabCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    event.Skip();
    if(!m_contextMenu) return;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    int realPos, tabHit;
    TestPoint(pt, realPos, tabHit);

    if((realPos != wxNOT_FOUND) && (realPos == GetSelection())) {
        // Show context menu for active tabs only
        PopupMenu(m_contextMenu);
    }
}

void clTabCtrl::DoShowTabList()
{
    if(m_tabs.empty()) return;

    int curselection = GetSelection();
    wxMenu menu;
    const int firstTabPageID = 13457;
    int pageMenuID = firstTabPageID;
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        clTabInfo::Ptr_t tab = m_tabs.at(i);
        wxMenuItem* item = new wxMenuItem(&menu, pageMenuID, tab->GetLabel(), "", wxITEM_CHECK);
        menu.Append(item);
        item->Check(tab->IsActive());
        pageMenuID++;
    }

    int selection = GetPopupMenuSelectionFromUser(menu, m_chevronRect.GetBottomLeft());
    if(selection != wxID_NONE) {
        selection -= firstTabPageID;
        // don't change the selection unless the selection is really changing
        if(curselection != selection) {
            SetSelection(selection);
        }
    }
}

bool clTabCtrl::SetPageToolTip(size_t page, const wxString& tooltip)
{
    clTabInfo::Ptr_t tab = GetTabInfo(page);
    if(tab) {
        tab->SetTooltip(tooltip);
        return true;
    }
    return false;
}

int clTabCtrl::DoGetPageIndex(const wxString& label) const
{
    for(size_t i = 0; i < m_tabs.size(); ++i) {
        if(m_tabs.at(i)->GetLabel() == label) return i;
    }
    return wxNOT_FOUND;
}

void clTabCtrl::DoChangeSelection(size_t index)
{
    // sanity
    if(index >= m_tabs.size()) return;

    int oldSelection = GetSelection();
    /// Do nothing if the tab is already selected
    if(oldSelection == (int)index) {
        ChangeSelection(index);
        return;
    }

    {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGING);
        event.SetEventObject(GetParent());
        event.SetSelection(oldSelection);
        event.SetOldSelection(wxNOT_FOUND);
        GetParent()->GetEventHandler()->ProcessEvent(event);

        if(!event.IsAllowed()) {
            return; // Vetoed by the user
        }
    }
    ChangeSelection(index);

    // Keep this page
    m_history->Push(GetPage(index));

    // Fire an event
    {
        wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
        event.SetEventObject(GetParent());
        event.SetSelection(GetSelection());
        event.SetOldSelection(oldSelection);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }
}

bool clTabCtrl::MoveActiveToIndex(int newIndex)
{
    int activeTabInex = GetSelection();

    // Sanity
    if(newIndex == wxNOT_FOUND) return false;
    if(activeTabInex == wxNOT_FOUND) return false;

    bool movingTabRight = (newIndex > activeTabInex);

    clTabInfo::Ptr_t movingTab = GetActiveTabInfo();
    clTabInfo::Ptr_t insertBeforeTab = m_tabs.at(newIndex);

    if(!movingTab) return false;

    // Step 1:
    // Remove the tab from both the active and from the visible tabs array
    clTabInfo::Vec_t::iterator iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_visibleTabs.end()) {
        m_visibleTabs.erase(iter);
    }
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == movingTab->GetWindow()) {
            return true;
        }
        return false;
    });
    if(iter != m_tabs.end()) {
        m_tabs.erase(iter);
    }

    // Step 2:
    // Insert 'tab' in its new position (in both arrays)
    iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](clTabInfo::Ptr_t t) {
        if(t->GetWindow() == insertBeforeTab->GetWindow()) {
            return true;
        }
        return false;
    });

    if(movingTabRight) {
        ++iter;
        // inser the new tab _after_
        if(iter != m_tabs.end()) {
            m_tabs.insert(iter, movingTab);
        } else {
            m_tabs.push_back(movingTab);
        }

        iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
            if(t->GetWindow() == insertBeforeTab->GetWindow()) {
                return true;
            }
            return false;
        });
        ++iter;
        if(iter != m_visibleTabs.end()) {
            m_visibleTabs.insert(iter, movingTab);
        } else {
            m_visibleTabs.push_back(movingTab);
        }
    } else {
        if(iter != m_tabs.end()) {
            m_tabs.insert(iter, movingTab);
        }

        iter = std::find_if(m_visibleTabs.begin(), m_visibleTabs.end(), [&](clTabInfo::Ptr_t t) {
            if(t->GetWindow() == insertBeforeTab->GetWindow()) {
                return true;
            }
            return false;
        });
        if(iter != m_visibleTabs.end()) {
            m_visibleTabs.insert(iter, movingTab);
        }
    }
    // Step 3:
    // Update the visible tabs coordinates
    DoUpdateCoordiantes(m_visibleTabs);

    // And finally: Refresh
    Refresh();
    return true;
}

void clTabCtrl::OnLeftDClick(wxMouseEvent& event)
{
    event.Skip();
    int realPos, tabHit;
    TestPoint(event.GetPosition(), realPos, tabHit);
    if(tabHit == wxNOT_FOUND) {
        // Fire background d-clicked event
        wxBookCtrlEvent e(wxEVT_BOOK_TABAREA_DCLICKED);
        e.SetEventObject(GetParent());
        GetParent()->GetEventHandler()->AddPendingEvent(e);
    } else {
        // a tab was hit
        wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
        e.SetEventObject(GetParent());
        e.SetSelection(realPos);
        GetParent()->GetEventHandler()->AddPendingEvent(e);
    }
}

void clTabCtrl::DoDrawBottomBox(clTabInfo::Ptr_t activeTab,
                                const wxRect& clientRect,
                                wxDC& dc,
                                const clTabInfo::Colours& colours)
{
    if(GetStyle() & kNotebook_LeftTabs) {
        // Draw 3 lines on the right
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopRight();
        wxSize rectSize(clTabInfo::BOTTOM_AREA_HEIGHT + 2, clientRect.height);
        topLeft.x -= clTabInfo::BOTTOM_AREA_HEIGHT;
        wxRect bottomRect(topLeft, rectSize);

        // We intentionally move the rect out of the client rect
        // so the top and bottom lines will be drawn out of screen
        bottomRect.y -= 1;
        bottomRect.height += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = activeTab->GetRect().GetTopRight();
        to = activeTab->GetRect().GetBottomRight();
        from.x = bottomRect.GetTopLeft().x;
        to.x = bottomRect.GetTopLeft().x;
        from.y += 2;
        to.y -= 2;
        
        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
#ifdef __WXOSX__
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
#endif
    } else if(GetStyle() & kNotebook_RightTabs) {
        // Draw 3 lines on the right
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopLeft();
        wxSize rectSize(clTabInfo::BOTTOM_AREA_HEIGHT + 2, clientRect.height);
        wxRect bottomRect(topLeft, rectSize);

        // We intentionally move the rect out of the client rect
        // so the top and bottom lines will be drawn out of screen
        bottomRect.y -= 1;
        bottomRect.height += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = activeTab->GetRect().GetTopLeft();
        to = activeTab->GetRect().GetBottomLeft();
        from.x = bottomRect.GetTopRight().x;
        to.x = bottomRect.GetTopRight().x;
        from.y += 2;
        to.y -= 2;
        
        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
#ifdef __WXOSX__
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
#endif

    } else if(GetStyle() & kNotebook_BottomTabs) {
        // Draw 3 lines at the top
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetTopLeft();
        wxSize rectSize(clientRect.width, clTabInfo::BOTTOM_AREA_HEIGHT);
        topLeft.y = 0;
        wxRect bottomRect(topLeft, rectSize);
        // We intentionally move the rect out of the client rect
        // so the left and right lines will be drawn out of screen
        bottomRect.x -= 1;
        bottomRect.width += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = activeTab->GetRect().GetTopLeft();
        to = activeTab->GetRect().GetTopRight();
        from.y += clTabInfo::BOTTOM_AREA_HEIGHT - 1;
        from.x += 2;
        to.y += clTabInfo::BOTTOM_AREA_HEIGHT - 1;
        to.x -= 2;

        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
#ifdef __WXOSX__
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
#endif

    } else {
        // Draw 3 lines at the bottom
        dc.SetPen(colours.activeTabPenColour);
        dc.SetBrush(colours.activeTabBgColour);
        wxPoint topLeft = clientRect.GetBottomLeft();
        wxSize rectSize(clientRect.width, clTabInfo::BOTTOM_AREA_HEIGHT);
        topLeft.y -= rectSize.GetHeight() - 1;
        wxRect bottomRect(topLeft, rectSize);
        // We intentionally move the rect out of the client rect
        // so the left and right lines will be drawn out of screen
        bottomRect.x -= 1;
        bottomRect.width += 2;
        dc.DrawRectangle(bottomRect);

        // Draw a line under the active tab
        // that will erase the line drawn by the above rect
        wxPoint from, to;
        from = activeTab->GetRect().GetBottomLeft();
        to = activeTab->GetRect().GetBottomRight();
        from.y -= clTabInfo::BOTTOM_AREA_HEIGHT - 1;
        from.x += 2;
        to.y -= clTabInfo::BOTTOM_AREA_HEIGHT - 1;
        to.x -= 2;

        dc.SetPen(colours.activeTabBgColour);
        dc.DrawLine(from, to);
#ifdef __WXOSX__
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
        dc.DrawLine(from, to);
#endif
    }
}

bool clTabCtrl::IsVerticalTabs() const { return (m_style & kNotebook_RightTabs) || (m_style & kNotebook_LeftTabs); }

bool clTabCtrl::ShiftBottom(clTabInfo::Vec_t& tabs)
{
    // Move the first tab from the list and adjust the remainder
    // of the tabs y coordiate
    if(!tabs.empty()) {
        clTabInfo::Ptr_t t = tabs.at(0);
        int height = t->GetHeight();
        tabs.erase(tabs.begin() + 0);

        for(size_t i = 0; i < tabs.size(); ++i) {
            clTabInfo::Ptr_t t = tabs.at(i);
            t->GetRect().SetY(t->GetRect().y - height + V_OVERLAP_WIDTH);
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------
// clTabHistory
// ----------------------------------------------------------------------

clTabHistory::clTabHistory() {}

clTabHistory::~clTabHistory() {}

void clTabHistory::Clear() { m_history.clear(); }

void clTabHistory::Pop(wxWindow* page)
{
    if(!page) return;

    int where = m_history.Index(page);
    if(where != wxNOT_FOUND) {
        m_history.Remove((void*)page);
    }
}

wxWindow* clTabHistory::PrevPage()
{
    if(m_history.empty()) {
        return NULL;
    }
    // return the top of the heap
    return static_cast<wxWindow*>(m_history.Item(0));
}

void clTabHistory::Push(wxWindow* page)
{
    if(page == NULL) return;
    Pop(page);
    m_history.Insert((void*)page, 0);
}

// ---------------------------------------------------------------------------
// DnD
// ---------------------------------------------------------------------------

clTabCtrlDropTarget::clTabCtrlDropTarget(clTabCtrl* tabCtrl)
    : m_tabCtrl(tabCtrl)
{
}

clTabCtrlDropTarget::~clTabCtrlDropTarget() {}

bool clTabCtrlDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
    // Extract the content dragged using regular expression
    wxRegEx re("\\{Class:Notebook,TabIndex:([0-9]+)\\}");
    if(!re.Matches(data)) return false;
    wxString tabIndex = re.GetMatch(data, 1);
    long nTabIndex = wxNOT_FOUND;
    tabIndex.ToCLong(&nTabIndex);
    // Sanity
    if(nTabIndex == wxNOT_FOUND) return false;

    // Test the drop tab index
    int realPos, tabHit;
    m_tabCtrl->TestPoint(wxPoint(x, y), realPos, tabHit);

    // if the tab being dragged and the one we drop it on are the same
    // return false
    if(nTabIndex == realPos) return false;
    m_tabCtrl->MoveActiveToIndex(realPos);
    return true;
}
