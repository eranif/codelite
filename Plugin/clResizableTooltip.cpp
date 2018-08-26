#include "clResizableTooltip.h"
#include <wx/wupdlock.h>
#include "event_notifier.h"
#include <wx/gdicmn.h>

wxDEFINE_EVENT(wxEVT_TOOLTIP_DESTROY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TOOLTIP_ITEM_EXPANDING, clCommandEvent);

clResizableTooltip::clResizableTooltip(wxEvtHandler* owner)
    : clResizableTooltipBase(EventNotifier::Get()->TopFrame())
    , m_dragging(false)
    , m_owner(owner)
{
    m_treeCtrl->SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
}

clResizableTooltip::~clResizableTooltip()
{
    if(m_panelStatus->HasCapture()) {
        m_panelStatus->ReleaseMouse();
    }
}

void clResizableTooltip::OnCheckMousePosition(wxTimerEvent& event)
{
#ifdef __WXMSW__
    // On Windows, wxPopupWindow does not return a correct position
    // in screen coordinates. So we use the inside tree-ctrl to
    // calc our position and size
    wxPoint tipPoint = m_treeCtrl->GetPosition();
    tipPoint = m_treeCtrl->ClientToScreen(tipPoint);
    wxSize tipSize = GetSize();
    // Construct a wxRect from the tip size/position
    wxRect rect(tipPoint, tipSize);
#else
    // Linux and OSX it works
    wxRect rect(GetRect());
#endif

    // and increase it by 15 pixels
    rect.Inflate(15, 15);
    if(!rect.Contains(::wxGetMousePosition())) {
        if(m_panelStatus->HasCapture()) {
            m_panelStatus->ReleaseMouse();
        }

        // Notify to the owner that this tooltip should be destroyed
        clCommandEvent destroyEvent(wxEVT_TOOLTIP_DESTROY);
        destroyEvent.SetEventObject(this);
        m_owner->AddPendingEvent(destroyEvent);
    }
}

void clResizableTooltip::OnStatusBarLeftDown(wxMouseEvent& event)
{
    m_dragging = true;
#ifndef __WXGTK__
    wxSetCursor(wxCURSOR_SIZING);
#endif
    m_panelStatus->CaptureMouse();
}

void clResizableTooltip::OnStatusBarLeftUp(wxMouseEvent& event)
{
    event.Skip();
    DoUpdateSize(true);
}

void clResizableTooltip::OnStatusBarMotion(wxMouseEvent& event)
{
    event.Skip();
    if(m_dragging) {
        wxRect curect = GetScreenRect();
        wxPoint curpos = ::wxGetMousePosition();

        int xDiff = curect.GetBottomRight().x - curpos.x;
        int yDiff = curect.GetBottomRight().y - curpos.y;

        if((abs(xDiff) > 3) || (abs(yDiff) > 3)) {
            DoUpdateSize(false);
        }
    }
}

void clResizableTooltip::OnStatusEnterWindow(wxMouseEvent& event)
{
    event.Skip();
#ifndef __WXGTK__
    ::wxSetCursor(wxCURSOR_SIZING);
#endif
}

void clResizableTooltip::OnStatusLeaveWindow(wxMouseEvent& event)
{
    event.Skip();
#ifndef __WXGTK__
    ::wxSetCursor(wxNullCursor);
#endif
}

void clResizableTooltip::OnItemExpanding(wxTreeEvent& event) { event.Skip(); }

void clResizableTooltip::Clear() { m_treeCtrl->DeleteAllItems(); }

void clResizableTooltip::DoUpdateSize(bool performClean)
{
    if(m_dragging) {
        wxCoord ww, hh;
        wxPoint mousePt = ::wxGetMousePosition();
        ww = mousePt.x - m_topLeft.x;
        hh = mousePt.y - m_topLeft.y;
        
        wxRect curect(m_topLeft, wxSize(ww, hh));
        if(curect.GetHeight() > 100 && curect.GetWidth() > 100) {
#ifdef __WXMSW__
            wxWindowUpdateLocker locker(EventNotifier::Get()->TopFrame());
#endif
            SetSize(curect);
        }
    }

    if(performClean) {
        m_dragging = false;
        if(m_panelStatus->HasCapture()) {
            m_panelStatus->ReleaseMouse();
        }
#ifndef __WXGTK__
        wxSetCursor(wxNullCursor);
#endif
    }
}
void clResizableTooltip::OnCaptureLost(wxMouseCaptureLostEvent& event)
{
    event.Skip();
    if(m_panelStatus->HasCapture()) {
        m_panelStatus->ReleaseMouse();
        m_dragging = true;
    }
}

void clResizableTooltip::ShowTip()
{
    m_topLeft = ::wxGetMousePosition();
    Move(m_topLeft);
    wxPopupWindow::Show();
    if(GetTreeCtrl() && GetTreeCtrl()->IsShown()) {
        GetTreeCtrl()->SetFocus();
    }
    
}
