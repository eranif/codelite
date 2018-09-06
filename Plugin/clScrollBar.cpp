#include "clScrollBar.h"
#include <cmath>
#include <wx/dc.h>
#include <wx/log.h>
#include <wx/settings.h>

clScrollBarHelper::clScrollBarHelper(wxWindow* parent, wxOrientation orientation)
    : m_parent(parent)
    , m_orientation(orientation)
{
    m_buttonColour = wxColour("rgb(133,146,158)");
    m_bgColour = wxColour("rgb(52,73,94)");
    m_parent->Bind(wxEVT_MOUSE_CAPTURE_LOST, [&](wxMouseCaptureLostEvent& e) {
        e.Skip();
        DoCancelScrolling();
    });
    m_parent->Bind(wxEVT_LEFT_DOWN, &clScrollBarHelper::OnMouseLeftDown, this);
    m_parent->Bind(wxEVT_LEFT_UP, &clScrollBarHelper::OnMouseLeftUp, this);
    m_parent->Bind(wxEVT_MOTION, &clScrollBarHelper::OnMouseMotion, this);
    m_parent->Bind(wxEVT_KEY_DOWN, &clScrollBarHelper::OnKeyDown, this);
}

clScrollBarHelper::~clScrollBarHelper()
{
    m_parent->Unbind(wxEVT_LEFT_DOWN, &clScrollBarHelper::OnMouseLeftDown, this);
    m_parent->Unbind(wxEVT_LEFT_UP, &clScrollBarHelper::OnMouseLeftUp, this);
    m_parent->Unbind(wxEVT_MOTION, &clScrollBarHelper::OnMouseMotion, this);
    m_parent->Unbind(wxEVT_KEY_DOWN, &clScrollBarHelper::OnKeyDown, this);
}

void clScrollBarHelper::Render(wxDC& dc)
{
    if(m_thumbSize == 0 || m_range == 0 || m_pageSize == 0 || m_lineInPixels == 0) { return; }

    wxRect rect = GetClientRect();
    m_thumbRect = wxRect();
    wxRect parentRect = GetParent()->GetClientRect();

    // Get the button size
    int buttonSize = DoGetButtonSize();

    // Calculate the button position
    int buttonPos = DoGetButtonPosition();
    if(IsVertical()) {
        m_thumbRect = wxRect((parentRect.GetWidth() - rect.GetWidth()), buttonPos, rect.GetWidth(), buttonSize);
        if((m_thumbRect.GetHeight() + m_thumbRect.GetY()) > rect.GetHeight()) {
            m_thumbRect.SetY(rect.GetHeight() - m_thumbRect.GetHeight());
        }
        // Do we need to show the bar?
        if(m_thumbRect.GetHeight() >= rect.GetHeight()) { return; }
    } else {
        m_thumbRect = wxRect(buttonPos, (parentRect.GetHeight() - rect.GetHeight()), buttonSize, rect.GetHeight());
        if((m_thumbRect.GetWidth() + m_thumbRect.GetX()) > rect.GetWidth()) {
            m_thumbRect.SetX(rect.GetWidth() - m_thumbRect.GetWidth());
        }
        // Do we need to show the bar?
        if(m_thumbRect.GetWidth() >= rect.GetWidth()) { return; }
    }

    // Draw the background
    dc.SetPen(m_bgColour);
    dc.SetBrush(m_bgColour);
    dc.DrawRectangle(rect);

    // Draw the thumb
    dc.SetPen(m_buttonColour);
    dc.SetBrush(m_buttonColour);
    m_thumbRect.Deflate(1, 1);
    dc.DrawRoundedRectangle(m_thumbRect, 3.0);
}

void clScrollBarHelper::SetScrollbar(int position, int thumbSize, int range, int lineInPixels)
{
    m_position = position;
    m_thumbSize = thumbSize;
    m_range = range;
    m_pageSize = thumbSize - 1;
    m_lineInPixels = lineInPixels;
}

wxRect clScrollBarHelper::GetClientRect() const
{
    wxRect parentSize = GetParent()->GetClientRect();
    wxRect rect;
    if(IsVertical()) {
        rect.SetWidth(GetSize());
        rect.SetX(parentSize.GetWidth() - rect.GetWidth());
        rect.SetY(0);
        rect.SetHeight(parentSize.GetHeight());
    } else {
        rect.SetHeight(GetSize());
        rect.SetX(0);
        rect.SetY(parentSize.GetHeight() - rect.GetHeight());
        rect.SetWidth(parentSize.GetWidth());
    }
    return rect;
}

void clScrollBarHelper::OnMouseLeftDown(wxMouseEvent& event)
{
    if(m_thumbRect.Contains(event.GetPosition())) {
        m_scrolling = true;
        m_anchorPoint = event.GetPosition();
        m_parent->CaptureMouse();
    } else {
        event.Skip();
    }
}

void clScrollBarHelper::OnMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    if(m_parent->HasCapture()) {
        wxRect clientRect = GetClientRect();
        int maxjorAxis = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
        double pixelsPerLine = (double)maxjorAxis / (double)m_range;
        if(pixelsPerLine == 0.0) { return; }

        if(clientRect.GetHeight() <= 0) {
            DoCancelScrolling();
            return;
        }

        wxPoint currpoint = event.GetPosition();
        wxRect buttonRect = m_thumbRect;
        double lines = 0.0;
        bool moving_up = false;
        if(IsVertical()) {
            moving_up = ((currpoint.y - m_anchorPoint.y) < 0);
            m_remainder = currpoint.y - m_anchorPoint.y;
            int diff = std::abs((double)m_remainder);
            buttonRect.SetY(buttonRect.GetY() + (moving_up ? -diff : diff));
            lines = ((double)buttonRect.GetY() - (double)m_thumbRect.GetY()) / pixelsPerLine;
        } else {
            moving_up = ((currpoint.x - m_anchorPoint.x) < 0);
            m_remainder = currpoint.x - m_anchorPoint.x;
            
            int diff = std::abs((double)m_remainder);
            buttonRect.SetX(buttonRect.GetX() + (moving_up ? -diff : diff));
            lines = ((double)buttonRect.GetX() - (double)m_thumbRect.GetX()) / pixelsPerLine;
        }
        if(std::abs(lines) < 1.0) {
            return;
        }
        m_remainder = 0;
        wxScrollEvent scrollEvent(wxEVT_SCROLL_THUMBTRACK);
        scrollEvent.SetEventObject(m_parent);
        scrollEvent.SetPosition(std::round(lines)); // Negative number means we moved up
        ProcessEvent(scrollEvent);
        m_anchorPoint = currpoint;
    }
}

void clScrollBarHelper::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();
    DoCancelScrolling();
}

void clScrollBarHelper::DoCancelScrolling()
{
    if(m_parent->HasCapture()) { m_parent->ReleaseMouse(); }
    m_anchorPoint = wxPoint();
    m_scrolling = false;
}

int clScrollBarHelper::DoGetButtonSize()
{
    wxRect clientRect = GetClientRect();
    int majorAxis = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
    int buttonSize = ((double)m_thumbSize / (double)m_range) * majorAxis;

    // Dont let the button size to be too small
    if(buttonSize < GetMinButtonSize()) { buttonSize = GetMinButtonSize(); }
    return buttonSize;
}

int clScrollBarHelper::DoGetButtonPosition()
{
    wxRect clientRect = GetClientRect();
    int majorAxis = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
    int pos = ((double)m_position / (double)m_range) * majorAxis;
    return pos + m_remainder;
}

wxRect clScrollBarHelper::GetVirtualRect() const
{
    wxRect clientRect = GetClientRect();
    int majorAxis = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
    double pixelsPerLine = (double)majorAxis / (double)m_range;
    if(IsVertical()) {
        clientRect.SetY(m_position * pixelsPerLine);
    } else {
        clientRect.SetX(m_position * pixelsPerLine);
    }
    return clientRect;
}

void clScrollBarHelper::OnKeyDown(wxKeyEvent& event)
{
    switch(event.GetKeyCode()) {
    case WXK_UP: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_LINEUP);
        scrollEvent.SetPosition(1);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    case WXK_DOWN: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_LINEDOWN);
        scrollEvent.SetPosition(1);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    case WXK_PAGEUP: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_PAGEUP);
        scrollEvent.SetPosition(m_pageSize);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    case WXK_PAGEDOWN: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_PAGEDOWN);
        scrollEvent.SetPosition(m_pageSize);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    case WXK_HOME: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_TOP);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    case WXK_END: {
        wxScrollEvent scrollEvent(wxEVT_SCROLL_BOTTOM);
        scrollEvent.SetEventObject(m_parent);
        ProcessEvent(scrollEvent);
        break;
    }
    default:
        event.Skip();
        return;
    }
}
