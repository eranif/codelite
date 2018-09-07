#include "clScrollBar.h"
#include "clScrolledPanel.h"
#include <wx/log.h>
#include <wx/sizer.h>

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

clScrolledPanel::clScrolledPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_vsb = new clScrollBarHelper(this, wxVERTICAL);
    GetSizer()->Add(0, 0, 1, wxALL | wxEXPAND);
    GetSizer()->Add(m_vsb, 0, wxEXPAND | wxALIGN_RIGHT);
    GetSizer()->Layout();

    m_vsb->Bind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Bind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);
    Bind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
    
#ifdef __WXGTK__
    /// On GTK, UP/DOWN arrows is used to navigate between controls
    /// Disable it by capturing the event and calling 'Skip(false)'
    Bind(wxEVT_KEY_DOWN, [&](wxKeyEvent& event){
        if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_DOWN) {
            event.Skip(false);
        } else {
            event.Skip(true);
        }
    });
#endif
}

clScrolledPanel::~clScrolledPanel()
{
    m_vsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_LINEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_PAGEUP, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_BOTTOM, &clScrolledPanel::OnVScroll, this);
    m_vsb->Unbind(wxEVT_SCROLL_TOP, &clScrolledPanel::OnVScroll, this);
    Unbind(wxEVT_CHAR_HOOK, &clScrolledPanel::OnCharHook, this);
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
        ScrollLines(steps, direction);
    } else if(newTopLine != wxNOT_FOUND) {
        ScrollToLine(newTopLine);
    }
}

void clScrolledPanel::UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize)
{
    // Hide the scrollbar if needed
    if(rangeSize <= thumbSize) {
        m_vsb->Hide();
        GetSizer()->Layout();
    } else {
        m_vsb->Show();
        GetSizer()->Layout();
    }
    m_pageSize = pageSize;
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

    if(event.GetEventObject() == this) {
        if(event.GetKeyCode() == WXK_UP) {
            ScrollLines(1, wxUP);
        } else if(event.GetKeyCode() == WXK_DOWN) {
            ScrollLines(1, wxDOWN);
        } else if(event.GetKeyCode() == WXK_PAGEUP) {
            ScrollLines(GetPageSize(), wxUP);
        } else if(event.GetKeyCode() == WXK_PAGEDOWN) {
            ScrollLines(GetPageSize(), wxDOWN);
        } else if(event.GetKeyCode() == WXK_HOME) {
            ScrollLines(0, wxUP);
        } else if(event.GetKeyCode() == WXK_END) {
            ScrollLines(0, wxDOWN);
        }
    }
}

int clScrolledPanel::GetPageSize() const { return m_pageSize; }

void clScrolledPanel::GrabFocus()
{
#ifdef __WXGTK__
    gtk_widget_set_can_focus(GTK_WIDGET(this->GetHandle()), true);
    gtk_widget_grab_focus(GTK_WIDGET(this->GetHandle()));
#else
    SetCanFocus(true);
    SetFocus();
#endif
}
