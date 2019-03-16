#include "clCustomScrollBar.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/settings.h>

wxDEFINE_EVENT(wxEVT_CUSTOM_SCROLL, clScrollEvent);

#ifdef __WXOSX__
#define SB_WIDTH 12
#define SB_RADIUS 0.0
#elif defined(__WXGTK__)
#define SB_WIDTH 12
#define SB_RADIUS 0.0
#else
#define SB_WIDTH 12
#define SB_RADIUS 0.0
#endif

clCustomScrollBar::clCustomScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style)
    : wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxBORDER_NONE | wxWANTS_CHARS)
    , m_sbStyle(style)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &clCustomScrollBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
    Bind(wxEVT_LEFT_DOWN, &clCustomScrollBar::OnMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &clCustomScrollBar::OnMouseLeftUp, this);
    Bind(wxEVT_MOTION, &clCustomScrollBar::OnMotion, this);
    if(style == wxSB_HORIZONTAL) {
        SetSize(-1, SB_WIDTH);
    } else {
        SetSize(SB_WIDTH, -1);
    }
    Bind(wxEVT_SIZE, &clCustomScrollBar::OnSize, this);
    Bind(wxEVT_IDLE, &clCustomScrollBar::OnIdle, this);
    m_colours.InitFromColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
}

clCustomScrollBar::~clCustomScrollBar()
{
    Unbind(wxEVT_PAINT, &clCustomScrollBar::OnPaint, this);
    Unbind(wxEVT_LEFT_DOWN, &clCustomScrollBar::OnMouseLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &clCustomScrollBar::OnMouseLeftUp, this);
    Unbind(wxEVT_MOTION, &clCustomScrollBar::OnMotion, this);
    Unbind(wxEVT_SIZE, &clCustomScrollBar::OnSize, this);
    Unbind(wxEVT_IDLE, &clCustomScrollBar::OnIdle, this);
}

void clCustomScrollBar::UpdateScroll(int thumbSize, int range, int pageSize, int position)
{
    m_thumbRect = wxRect();
    m_thumbSize = thumbSize;
    m_range = range;
    m_pageSize = pageSize;

    // Set position triggers a refresh
    SetPosition(position, false);
}

void clCustomScrollBar::OnPaint(wxPaintEvent& e)
{
    wxAutoBufferedPaintDC bdc(this);
    wxGCDC dc(bdc);

    wxRect rect = GetClientRect();

    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(rect);

    if(!m_thumbRect.IsEmpty()) {
        dc.SetPen(m_colours.GetBorderColour());
        dc.SetBrush(m_colours.GetBorderColour());
        dc.DrawRoundedRectangle(m_thumbRect, SB_RADIUS);
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
    if(IsVertical()) {
        int diff = pt.y - m_mouseCapturePoint.y;
        // Update the new thumb Y coordinate
        m_thumbCapturePoint.y += diff;

    } else {
        int diff = pt.x - m_mouseCapturePoint.x;
        // Update the new thumb Y coordinate
        m_thumbCapturePoint.x += diff;
    }
    if(IsVertical()) {
        int offset = pt.y - m_mouseCapturePoint.y;
        m_thumbRect.Offset(0, offset);
        if(m_thumbRect.GetBottom() >= rect.GetHeight()) {
            m_thumbRect.SetY(rect.GetHeight() - m_thumbRect.GetHeight());
        } else if(m_thumbRect.GetY() < 0) {
            m_thumbRect.SetY(0);
        }
    } else {
        int offset = pt.x - m_mouseCapturePoint.x;
        m_thumbRect.Offset(offset, 0);
        if(m_thumbRect.GetRight() >= rect.GetWidth()) {
            m_thumbRect.SetX(rect.GetWidth() - m_thumbRect.GetWidth());
        } else if(m_thumbRect.GetX() < 0) {
            m_thumbRect.SetX(0);
        }
    }
    wxPanel::Refresh();

    int pos = GetPositionFromPoint(m_thumbRect.GetTopLeft());
    if(m_thumbPosition != pos) {
        m_thumbPosition = pos;
        m_notifyScroll = true;
    }
    m_mouseCapturePoint = pt;
}

void clCustomScrollBar::OnMouseLeftUp(wxMouseEvent& e)
{
    e.Skip();
    // Calculate the new starting position
    if(HasCapture()) { ReleaseMouse(); }

    if(m_dragging) {
        UpdateDrag(e.GetPosition());

    } else {
        int pos = GetPositionFromPoint(e.GetPosition());
        if(m_thumbPosition != pos) { SetPosition(pos, true); }
    }
    m_mouseCapturePoint = wxPoint();
    m_thumbCapturePoint = wxPoint();
    m_dragging = false;
}

void clCustomScrollBar::OnMotion(wxMouseEvent& e)
{
    e.Skip();
    if(m_dragging && wxGetMouseState().LeftIsDown()) { CallAfter(&clCustomScrollBar::UpdateDrag, e.GetPosition()); }
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

void clCustomScrollBar::SetPosition(int pos, bool notify)
{
    if(pos >= m_range || pos < 0) { pos = 0; }
    m_thumbPosition = pos;

    // Normalise position
    if((m_thumbPosition + m_thumbSize) > m_range) { m_thumbPosition = m_range - m_thumbSize; }

    wxRect clientRect = GetClientRect();
    double majorDim = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
    if(majorDim == 0.0) {
        m_thumbPosition = 0;
        m_thumbRect = wxRect();
        Refresh();
    }
    double percent = (double)m_thumbSize / m_range;
    double thumbMajorDim = percent * majorDim;
    double thumbCoord = (double)(m_thumbPosition / m_range) * majorDim;

    // Make sure that the thumb is always visible
    if(thumbMajorDim < 10) { thumbMajorDim = 10; }
    if(IsVertical()) {
        m_thumbRect.SetY(thumbCoord);
        m_thumbRect.SetX(0);
        m_thumbRect.SetWidth(clientRect.GetWidth());
        m_thumbRect.SetHeight(thumbMajorDim);
    } else {
        m_thumbRect.SetX(thumbCoord);
        m_thumbRect.SetY(0);
        m_thumbRect.SetHeight(clientRect.GetHeight());
        m_thumbRect.SetWidth(thumbMajorDim);
    }
    Refresh();
    if(notify) {
        // fire scroll event
        clScrollEvent event(wxEVT_CUSTOM_SCROLL);
        event.SetEventObject(this);
        event.SetPosition(m_thumbPosition);
        GetEventHandler()->ProcessEvent(event);
    }
    return;
}

int clCustomScrollBar::GetPositionFromPoint(const wxPoint& pt) const
{
    wxRect clientRect = GetClientRect();
    double majorDim = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
    if(majorDim == 0.0) { return wxNOT_FOUND; }
    double thumbCoord = IsVertical() ? pt.y : pt.x;
    return std::ceil((double)((thumbCoord / majorDim) * m_range));
}

void clCustomScrollBar::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    if(m_notifyScroll) {
        m_notifyScroll = false;
        clScrollEvent e(wxEVT_CUSTOM_SCROLL);
        e.SetEventObject(this);
        e.SetPosition(m_thumbPosition);
        GetEventHandler()->ProcessEvent(e);
    }
}

bool clCustomScrollBar::ShouldShow() const
{
    return ((m_thumbSize > 0) && (m_thumbSize < m_range));
}

//=============================================================
// clScrollEvent
//=============================================================
clScrollEvent::clScrollEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
{
}

clScrollEvent::clScrollEvent(const clScrollEvent& event)
    : wxCommandEvent(event)
{
    *this = event;
}

clScrollEvent::~clScrollEvent() {}

clScrollEvent& clScrollEvent::operator=(const clScrollEvent& src)
{
    // wxCommandEvent operator =
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    m_clientData = src.m_clientData;
    m_clientObject = src.m_clientObject;

    // this class members
    m_position = src.m_position;
    return *this;
}

wxEvent* clScrollEvent::Clone() const { return new clScrollEvent(*this); }
