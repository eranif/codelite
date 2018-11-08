#include "clResizableTooltip.h"
#include "cl_config.h"
#include "event_notifier.h"
#include <wx/gdicmn.h>
#include <wx/wupdlock.h>

wxDEFINE_EVENT(wxEVT_TOOLTIP_DESTROY, clCommandEvent);
wxDEFINE_EVENT(wxEVT_TOOLTIP_ITEM_EXPANDING, clCommandEvent);

clResizableTooltip::clResizableTooltip(wxEvtHandler* owner)
    : clResizableTooltipBase(EventNotifier::Get()->TopFrame())
    , m_owner(owner)
{
    m_treeCtrl->SetSortFunction(nullptr);
    int height = clConfig::Get().Read("Tooltip/Height", 200);
    int width = clConfig::Get().Read("Tooltip/Width", 300);
    wxSize toolsize(width, height);
    if((toolsize.GetWidth() < 300) || (toolsize.GetHeight() < 200)) { toolsize = wxSize(300, 200); }
    SetSize(toolsize);
}

clResizableTooltip::~clResizableTooltip()
{
    clConfig::Get().Write("Tooltip/Height", GetSize().GetHeight());
    clConfig::Get().Write("Tooltip/Width", GetSize().GetWidth());
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
        // Notify to the owner that this tooltip should be destroyed
        clCommandEvent destroyEvent(wxEVT_TOOLTIP_DESTROY);
        destroyEvent.SetEventObject(this);
        m_owner->AddPendingEvent(destroyEvent);
    }
}

void clResizableTooltip::OnItemExpanding(wxTreeEvent& event) { event.Skip(); }

void clResizableTooltip::Clear() { m_treeCtrl->DeleteAllItems(); }

void clResizableTooltip::ShowTip()
{
    m_topLeft = ::wxGetMousePosition();
    Move(m_topLeft);
    wxFrame::Show();
    if(GetTreeCtrl()) { GetTreeCtrl()->CallAfter(&clThemedTreeCtrl::SetFocus); }
}
