#include "clScrolledPanel.h"
#include "clScrollBar.h"
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/settings.h>

clScrolledPanel::clScrolledPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxWindow(parent, id, pos, size, style)
    , m_dragStartTime((time_t)-1)
{
    SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_vsb = new clScrollBar(this, wxVERTICAL);
    GetSizer()->Add(0, 0, 1, wxALL | wxEXPAND);
    GetSizer()->Add(m_vsb, 0, wxEXPAND);
    GetSizer()->Layout();

    m_vsb->Bind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);
    Bind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
    Bind(wxEVT_IDLE, &clScrolledPanel::OnIdle, this);
    Bind(wxEVT_LEFT_DOWN, &clScrolledPanel::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clScrolledPanel::OnLeftUp, this);
    Bind(wxEVT_MOTION, &clScrolledPanel::OnMotion, this);
    Bind(wxEVT_LEAVE_WINDOW, &clScrolledPanel::OnLeaveWindow, this);

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

    m_vsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);
    Unbind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
    Unbind(wxEVT_IDLE, &clScrolledPanel::OnIdle, this);
    Unbind(wxEVT_LEFT_DOWN, &clScrolledPanel::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clScrolledPanel::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clScrolledPanel::OnMotion, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clScrolledPanel::OnLeaveWindow, this);
}

void clScrolledPanel::OnVScroll(wxScrollEvent& event)
{
    int steps = 0;
    wxDirection direction = wxDOWN;
    int newTopLine = wxNOT_FOUND;
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
        steps = -1;
        direction = wxUP;
    } else if(event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
        steps = -1;
        direction = wxDOWN;
    } else if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) {
        newTopLine = event.GetPosition();
    }

    if(steps) {
        ScrollRows(steps, direction);
    } else if(newTopLine != wxNOT_FOUND) {
        ScrollToRow(newTopLine);
    }
}

void clScrolledPanel::UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize)
{
    // Sanity
    if(pageSize <= 0 || position < 0 || thumbSize <= 0 || rangeSize <= 0) { return; }

    // Keep the values
    m_pageSize = pageSize;
    m_position = position;
    m_thumbSize = thumbSize;
    m_rangeSize = rangeSize;

    // Hide the scrollbar if needed
    bool should_show = thumbSize < rangeSize;
    if(!should_show && m_vsb && m_vsb->IsShown()) {
        m_vsb->Hide();
        GetSizer()->Layout();
    } else if(should_show && m_vsb && !m_vsb->IsShown()) {
        m_vsb->Show();
        GetSizer()->Layout();
    }

    m_vsb->SetScrollbar(position, thumbSize, rangeSize, pageSize);
    m_vsb->Refresh();
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
    if(event.GetKeyCode() == WXK_HOME) {
        ScrollRows(0, wxUP);
    } else if(event.GetKeyCode() == WXK_END) {
        ScrollRows(0, wxDOWN);
    }

    // The following can be processed only once
    if(event.GetEventObject() == this) {
        if(event.GetKeyCode() == WXK_UP) {
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
            GetSizer()->Layout();
            m_vsb->SetScrollbar(m_position, m_thumbSize, m_rangeSize, m_pageSize);
        } else if(!inOurWindows && m_vsb->IsShown()) {
            m_vsb->Hide();
            GetSizer()->Layout();
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
    wxLogMessage("Left window, dnd cancelled");
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
    // for some reason, drawing the font size on GTK3 with wxGCDC is too small
    f.SetPointSize(f.GetPointSize() + 2);
#endif
    return f;
}
