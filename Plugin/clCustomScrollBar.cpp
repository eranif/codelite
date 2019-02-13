#include "clCustomScrollBar.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/settings.h>

clCustomScrollBar::clCustomScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxBORDER_NONE | wxWANTS_CHARS)
    , m_sbStyle(style)
{
    Bind(wxEVT_PAINT, &clCustomScrollBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
    Bind(wxEVT_LEFT_DOWN, &clCustomScrollBar::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clCustomScrollBar::OnMouseLeftUp, this);
    Bind(wxEVT_MOTION, &clCustomScrollBar::OnMotion, this);
    if(style == wxSB_HORIZONTAL) {
        SetSize(-1, 12);
    } else {
        SetSize(12, -1);
    }
    Bind(wxEVT_SIZE, &clCustomScrollBar::OnSize, this);
    m_colours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

clCustomScrollBar::~clCustomScrollBar()
{
    Unbind(wxEVT_PAINT, &clCustomScrollBar::OnPaint, this);
    Unbind(wxEVT_LEFT_DOWN, &clCustomScrollBar::OnMouseLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clCustomScrollBar::OnMouseLeftUp, this);
    Unbind(wxEVT_MOTION, &clCustomScrollBar::OnMotion, this);
    Unbind(wxEVT_SIZE, &clCustomScrollBar::OnSize, this);
}

void clCustomScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool refresh)
{
    m_thumbPosition = position;
    m_thumbSize = thumbSize;
    m_range = range;
    m_pageSize = pageSize;
    if(refresh) { Refresh(); }
}

void clCustomScrollBar::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC bdc(this);
    wxGCDC dc(bdc);

    wxRect rect = GetClientRect();
    double major = IsHorizontal() ? rect.GetWidth() : rect.GetHeight();

    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(rect);

    m_thumbRect = wxRect();
    if(m_range <= 0) { return; }

    // Draw the thumb but only if the number of items in the thumb is smaller than the total range
    if(m_thumbSize < m_range) {
        double ratio = m_thumbSize / m_range;
        int thumbMajor = major * ratio;
        int thumbPos = major * (m_thumbPosition / m_range);
        if(thumbMajor < 10) { thumbMajor = 10; }
        wxRect thumbRect(rect);
        if(IsHorizontal()) {
            thumbRect.SetX(thumbPos);
            thumbRect.SetWidth(thumbMajor);
        } else {
            thumbRect.SetY(thumbPos);
            thumbRect.SetHeight(thumbMajor);
        }
        m_thumbRect = thumbRect;
        dc.SetPen(m_colours.GetBorderColour());
        dc.SetBrush(m_colours.GetBorderColour());
        dc.DrawRoundedRectangle(m_thumbRect, 3.0);
    }
}

void clCustomScrollBar::OnMouseLeftDown(wxMouseEvent& e)
{
    e.Skip();
    m_mouseCapturePoint = wxPoint();
    m_dragging = false;
    if(m_thumbRect.Contains(e.GetPosition())) {
        // Mouse down is inside the thumb rect
        m_mouseCapturePoint = e.GetPosition();
        m_thumbCapturePoint = m_thumbRect.GetTopLeft();
        CaptureMouse();
        m_dragging = true;
    }
}
void clCustomScrollBar::UpdateDrag(const wxPoint& pt)
{
    wxRect rect = GetClientRect();
    int pos = 0;
    if(IsVertical()) {
        int diff = pt.y - m_mouseCapturePoint.y;
        // Update the new thumb Y coordinate
        m_thumbCapturePoint.y += diff;
        // Based on the new thumb position (we don't move it just yet)
        // calculate the new starting position
        pos = (m_thumbCapturePoint.y * m_range) / rect.GetHeight();

    } else {
        int diff = pt.x - m_mouseCapturePoint.x;
        // Update the new thumb Y coordinate
        m_thumbCapturePoint.x += diff;
        // Based on the new thumb position (we don't move it just yet)
        // calculate the new starting position
        pos = (m_thumbCapturePoint.x * m_range) / rect.GetWidth();
    }

    m_mouseCapturePoint = pt;
    if(pos < 0) { pos = 0; }
    if((pos + m_thumbSize) > m_range) {
        pos = (m_range - m_thumbSize);
        if(IsVertical()) { pos += 1; }
    }

    wxScrollEvent e(wxEVT_SCROLL_THUMBTRACK);
    e.SetEventObject(this);
    e.SetPosition(pos);
    GetEventHandler()->AddPendingEvent(e);
}

void clCustomScrollBar::OnMouseLeftUp(wxMouseEvent& e)
{
    e.Skip();
    // Calculate the new starting position
    if(HasCapture()) { ReleaseMouse(); }
    UpdateDrag(e.GetPosition());
    m_mouseCapturePoint = wxPoint();
    m_thumbCapturePoint = wxPoint();
    m_dragging = false;
}

void clCustomScrollBar::OnMotion(wxMouseEvent& e)
{
    e.Skip();
    if(m_dragging && wxGetMouseState().LeftIsDown()) { UpdateDrag(e.GetPosition()); }
}

void clCustomScrollBar::OnSize(wxSizeEvent& e)
{
    e.Skip();
    Refresh();
}

void clCustomScrollBar::SetColours(const clColours& colours)
{
    m_colours = colours;
    Refresh();
}
