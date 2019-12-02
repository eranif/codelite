#include "clScrollBar.h"
#include "clScrolledPanel.h"
#include <wx/dcscreen.h>
#include <wx/log.h>
#include <wx/settings.h>
#include <wx/sizer.h>

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

clScrolledPanel::clScrolledPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxWindow(parent, id, pos, size, style)
{
    DoInitialize();
}

bool clScrolledPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!wxWindow::Create(parent, id, pos, size, style)) { return false; }
    DoInitialize();
    return true;
}

void clScrolledPanel::DoInitialize()
{
    m_dragStartTime = (time_t)-1;
    m_vsb = new ScrollBar_t(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
    DoPositionVScrollbar();

    m_hsb = new ScrollBar_t(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    DoPositionHScrollbar();
#if CL_USE_NATIVE_SCROLLBAR
    m_vsb->Bind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);

    m_hsb->Bind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnHScroll, this);
    m_hsb->Bind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnHScroll, this);
#else
    m_vsb->Bind(wxEVT_CUSTOM_SCROLL, &clScrolledPanel::OnVCustomScroll, this);
    m_hsb->Bind(wxEVT_CUSTOM_SCROLL, &clScrolledPanel::OnHCustomScroll, this);
#endif

    Bind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
    Bind(wxEVT_IDLE, &clScrolledPanel::OnIdle, this);
    Bind(wxEVT_LEFT_DOWN, &clScrolledPanel::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clScrolledPanel::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clScrolledPanel::OnMotion, this);
    Bind(wxEVT_LEAVE_WINDOW, &clScrolledPanel::OnLeaveWindow, this);
    Bind(wxEVT_SIZE, &clScrolledPanel::OnSize, this);

    Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& event) {
        event.Skip();
        Refresh();
    });
    Bind(wxEVT_KILL_FOCUS, [&](wxFocusEvent& event) {
        event.Skip();
        Refresh();
    });

#ifdef __WXGTK__
    /// On GTK, UP/DOWN arrows is used to navigate between controls
    /// Disable it by capturing the event and calling 'Skip(false)'
    Bind(wxEVT_KEY_DOWN, [&](wxKeyEvent& event) {
        if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_DOWN) {
            event.Skip(false);
        } else {
            event.Skip(true);
        }
    });
#endif
    m_tmpBmp = wxBitmap(1, 1);
    m_memDC = new wxMemoryDC(m_tmpBmp);
    m_gcdc = new wxGCDC(*m_memDC);
}

clScrolledPanel::~clScrolledPanel()
{
    // Destory the DCs in the reverse order of their creation
    wxDELETE(m_gcdc);
    wxDELETE(m_memDC);
#if CL_USE_NATIVE_SCROLLBAR
    m_vsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);

    m_hsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnHScroll, this);
    m_hsb->Unbind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnHScroll, this);
#else
    m_vsb->Unbind(wxEVT_CUSTOM_SCROLL, &clScrolledPanel::OnVCustomScroll, this);
    m_hsb->Unbind(wxEVT_CUSTOM_SCROLL, &clScrolledPanel::OnHCustomScroll, this);
#endif

    Unbind(wxEVT_SIZE, &clScrolledPanel::OnSize, this);
    Unbind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
    Unbind(wxEVT_IDLE, &clScrolledPanel::OnIdle, this);
    Unbind(wxEVT_LEFT_DOWN, &clScrolledPanel::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clScrolledPanel::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clScrolledPanel::OnMotion, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clScrolledPanel::OnLeaveWindow, this);
}

#if CL_USE_NATIVE_SCROLLBAR
void clScrolledPanel::OnHScroll(wxScrollEvent& event)
{
    int newColumn = wxNOT_FOUND;
    if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) {
        newColumn = event.GetPosition();
        ScollToColumn(newColumn);

    } else {
        int steps = wxNOT_FOUND;
        wxDirection direction = wxRIGHT;
        if(event.GetEventType() == wxEVT_SCROLL_LINEUP) {
            steps = 1;
            direction = wxLEFT;
        } else if(event.GetEventType() == wxEVT_SCROLL_LINEDOWN) {
            steps = 1;
            direction = wxRIGHT;
        } else if(event.GetEventType() == wxEVT_SCROLL_PAGEUP) {
            steps = m_hsb->GetPageSize();
            direction = wxLEFT;
        } else if(event.GetEventType() == wxEVT_SCROLL_PAGEDOWN) {
            steps = m_hsb->GetPageSize();
            direction = wxRIGHT;
        } else if(event.GetEventType() == wxEVT_SCROLL_TOP) {
            steps = 0;
            direction = wxLEFT;
        } else if(event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
            steps = 0;
            direction = wxRIGHT;
        }
        if(steps != wxNOT_FOUND) { ScrollColumns(steps, direction); }
    }
}

void clScrolledPanel::OnVScroll(wxScrollEvent& event)
{
    wxDirection direction = wxDOWN;
    int newTopLine = wxNOT_FOUND;
    if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) {
        newTopLine = event.GetPosition();
        ScrollToRow(newTopLine);
    } else {
        int steps = wxNOT_FOUND;
        if(event.GetEventType() == wxEVT_SCROLL_LINEUP) {
            steps = 1;
            direction = wxUP;
        } else if(event.GetEventType() == wxEVT_SCROLL_LINEDOWN) {
            steps = 1;
            direction = wxDOWN;
        } else if(event.GetEventType() == wxEVT_SCROLL_PAGEUP) {
            steps = m_vsb->GetPageSize();
            direction = wxUP;
        } else if(event.GetEventType() == wxEVT_SCROLL_PAGEDOWN) {
            steps = m_vsb->GetPageSize();
            direction = wxDOWN;
        } else if(event.GetEventType() == wxEVT_SCROLL_TOP) {
            steps = 0;
            direction = wxUP;
        } else if(event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
            steps = 0;
            direction = wxDOWN;
        }
        if(steps != wxNOT_FOUND) { ScrollRows(steps, direction); }
    }
}
#endif

#if CL_USE_CUSTOM_SCROLLBAR
void clScrolledPanel::OnVCustomScroll(clScrollEvent& event) { ScrollToRow(event.GetPosition()); }
void clScrolledPanel::OnHCustomScroll(clScrollEvent& event) { ScollToColumn(event.GetPosition()); }
#endif

void clScrolledPanel::UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize)
{
    // Sanity
    if(m_neverShowVScrollbar || pageSize <= 0 || position < 0 || thumbSize <= 0 || rangeSize <= 0) {
        m_vsb->Hide();
        return;
    }

    // Keep the values
    m_pageSize = pageSize;
    m_position = position;
    m_thumbSize = thumbSize;
    m_rangeSize = rangeSize;

    // Hide the scrollbar if needed
    bool should_show = (thumbSize < rangeSize) && (m_vsb->IsShown() || !m_showSBOnFocus);
    if(!should_show && m_vsb && m_vsb->IsShown()) {
        m_vsb->Hide();
    } else if(should_show && m_vsb && !m_vsb->IsShown()) {
        DoPositionVScrollbar();
        m_vsb->Show();
    }

#if CL_USE_NATIVE_SCROLLBAR
    m_vsb->SetScrollbar(position, thumbSize, rangeSize, pageSize);
    m_vsb->Refresh();
#else
    m_vsb->UpdateScroll(thumbSize, rangeSize, pageSize, position);
#endif
    CallAfter(&clScrolledPanel::DoPositionHScrollbar);
}

void clScrolledPanel::OnCharHook(wxKeyEvent& event)
{
    event.Skip();
    wxKeyEvent keyDown = event;
    keyDown.SetEventType(wxEVT_KEY_DOWN);
    if(DoKeyDown(keyDown)) {
        event.Skip(false);
        return;
    }

    // Always process the HOME/END buttons
    // The following can be processed only once
    if(event.GetEventObject() == this) {
        if(event.GetKeyCode() == WXK_HOME) {
            ScrollRows(0, wxUP);
        } else if(event.GetKeyCode() == WXK_END) {
            ScrollRows(0, wxDOWN);
        } else if(event.GetKeyCode() == WXK_UP) {
            ScrollRows(1, wxUP);
        } else if(event.GetKeyCode() == WXK_DOWN) {
            ScrollRows(1, wxDOWN);
        } else if(event.GetKeyCode() == WXK_PAGEUP) {
            ScrollRows(GetPageSize(), wxUP);
        } else if(event.GetKeyCode() == WXK_PAGEDOWN) {
            ScrollRows(GetPageSize(), wxDOWN);
        }
    }
}

int clScrolledPanel::GetPageSize() const { return m_pageSize; }

void clScrolledPanel::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(m_vsb && m_showSBOnFocus) {
        wxWindow* focus_win = wxWindow::FindFocus();
        bool inOurWindows = IsDescendant(focus_win);
        if(ShouldShowScrollBar() && !m_vsb->IsShown() && inOurWindows) {
            // Update the scrollbar with the latest values
            m_vsb->Show();
            DoPositionVScrollbar();
            m_vsb->SetScrollbar(m_position, m_thumbSize, m_rangeSize, m_pageSize);
        } else if(!inOurWindows && m_vsb->IsShown()) {
            m_vsb->Hide();
        }
    }
    ProcessIdle();
}

bool clScrolledPanel::ShouldShowScrollBar() const { return m_vsb && m_vsb->ShouldShow(); }

void clScrolledPanel::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    // Not considering D'n'D so reset any saved values
    DoCancelDrag();

    // Prepare to DnDclTreeCtrl_DnD
    if(event.LeftIsDown()) {
        m_dragStartTime = wxDateTime::UNow();
        m_dragStartPos = wxPoint(event.GetX(), event.GetY());
    }
}

void clScrolledPanel::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();
    if(m_dragging) {
        wxTreeItemId dropTarget = GetRow(event.GetPosition());
        if(dropTarget.IsOk()) {
            wxTreeEvent event(wxEVT_TREE_END_DRAG);
            event.SetEventObject(this);
            event.SetItem(dropTarget);
            GetEventHandler()->ProcessEvent(event);
        }
    }
    DoCancelDrag();
}

void clScrolledPanel::OnMotion(wxMouseEvent& event)
{
    event.Skip();
    if(m_dragStartTime.IsValid() && event.LeftIsDown() &&
       !m_dragging) { // If we're tugging on the tab, consider starting D'n'D
        wxTimeSpan diff = wxDateTime::UNow() - m_dragStartTime;
        if(diff.GetMilliseconds() > 100 && // We need to check both x and y distances as tabs may be vertical
           ((abs(m_dragStartPos.x - event.GetX()) > 5) || (abs(m_dragStartPos.y - event.GetY()) > 5))) {
            DoBeginDrag(); // Sufficient time and distance since the LeftDown for a believable D'n'D start
        }
    }
}

void clScrolledPanel::OnLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
    DoCancelDrag();
}

void clScrolledPanel::DoBeginDrag()
{
    if(!GetRow(m_dragStartPos).IsOk()) {
        DoCancelDrag();
        return;
    }

    wxTreeEvent event(wxEVT_TREE_BEGIN_DRAG);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
    if(!event.IsAllowed()) { return; }

    // Change the cursor indicating DnD in progress
    SetCursor(wxCURSOR_HAND);
    m_dragging = true;
}

void clScrolledPanel::DoCancelDrag()
{
    m_dragStartTime.Set((time_t)-1); // Reset the saved values
    m_dragStartPos = wxPoint();
    SetCursor(wxCURSOR_DEFAULT);
    m_dragging = false;
}

wxFont clScrolledPanel::GetDefaultFont()
{
    wxFont f = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#ifdef __WXGTK__
    static double ratio = 1.0;
    static bool once = false;
    if(!once) {
        GdkScreen* screen = gdk_screen_get_default();
        if(screen) {
            double res = gdk_screen_get_resolution(screen);
            ratio = (res / 96.);
        }
        once = true;
    }
#if wxCHECK_VERSION(3, 1, 2)
    float pointSize = f.GetFractionalPointSize() * ratio;
    f.SetFractionalPointSize(pointSize);
#else
    int pointSize = f.GetPointSize() * ratio;
    f.SetPointSize(pointSize);
#endif
#endif
    return f;
}

void clScrolledPanel::DoPositionVScrollbar()
{
    wxRect clientRect = GetClientRect();
    wxSize vsbSize = m_vsb->GetSize();

    int height = clientRect.GetHeight();
    if(m_hsb && m_hsb->IsShown()) { height -= m_hsb->GetSize().GetHeight(); }
    int width = vsbSize.GetWidth();
    int x = clientRect.GetWidth() - vsbSize.GetWidth();
    int y = 0;

    m_vsb->SetSize(width, height);
    m_vsb->Move(x, y);
}

void clScrolledPanel::DoPositionHScrollbar()
{
    wxRect clientRect = GetClientRect();
    wxSize hsbSize = m_hsb->GetSize();

    int width = clientRect.GetWidth();
    if(m_vsb && m_vsb->IsShown()) { width -= m_vsb->GetSize().GetWidth(); }
    int height = hsbSize.GetHeight();
    int x = 0;
    int y = clientRect.GetHeight() - hsbSize.GetHeight();

    m_hsb->SetSize(width, height);
    m_hsb->Move(x, y);
}

void clScrolledPanel::OnSize(wxSizeEvent& event)
{
    event.Skip();
    DoPositionVScrollbar();
    DoPositionHScrollbar();
}

void clScrolledPanel::UpdateHScrollBar(int position, int thumbSize, int rangeSize, int pageSize)
{
    if(rangeSize <= 0 || m_neverShowHScrollbar) {
        m_hsb->Hide();
        return;
    }
    bool should_show = thumbSize < rangeSize;
    if(should_show && !m_hsb->IsShown()) {
        DoPositionHScrollbar();
        m_hsb->Show();
    } else if(!should_show && m_hsb->IsShown()) {
        m_hsb->Hide();
    }
#if CL_USE_NATIVE_SCROLLBAR
    m_hsb->SetScrollbar(position, thumbSize, rangeSize, pageSize);
    m_hsb->Refresh();
#else
    m_hsb->UpdateScroll(thumbSize, rangeSize, pageSize, position);
#endif
    CallAfter(&clScrolledPanel::DoPositionVScrollbar);
}

wxRect clScrolledPanel::GetClientArea() const
{
    wxRect r = GetClientRect();
#ifdef __WXOSX__
    // on OSX, the top-left is set to 2,2
    r.SetTopLeft(wxPoint(0, 0));
#endif

    if(m_hsb && m_hsb->IsShown()) { r.SetHeight(r.GetHeight() - m_hsb->GetSize().GetHeight()); }
    if(m_vsb && m_vsb->IsShown()) { r.SetWidth(r.GetWidth() - m_vsb->GetSize().GetWidth()); }
    return r;
}

void clScrolledPanel::SetNeverShowScrollBar(wxOrientation d, bool b)
{
    if(d == wxVERTICAL) {
        m_neverShowVScrollbar = b;
    } else if(d == wxHORIZONTAL) {
        m_neverShowHScrollbar = b;
    }
    if(m_vsb) {
        UpdateVScrollBar(m_vsb->GetThumbPosition(), m_vsb->GetThumbSize(), m_vsb->GetRange(), m_vsb->GetPageSize());
    }
    if(m_hsb) {
        UpdateHScrollBar(m_hsb->GetThumbPosition(), m_hsb->GetThumbSize(), m_hsb->GetRange(), m_hsb->GetPageSize());
    }
    Refresh();
}
