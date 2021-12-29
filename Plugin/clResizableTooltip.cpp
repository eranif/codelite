#include "clResizableTooltip.h"

#include "cl_config.h"
#include "event_notifier.h"
#include "file_logger.h"

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
    if((toolsize.GetWidth() < 300) || (toolsize.GetHeight() < 200)) {
        toolsize = wxSize(300, 200);
    }
    SetSize(toolsize);
}

clResizableTooltip::~clResizableTooltip()
{
    clConfig::Get().Write("Tooltip/Height", GetSize().GetHeight());
    clConfig::Get().Write("Tooltip/Width", GetSize().GetWidth());
}

void clResizableTooltip::OnItemExpanding(wxTreeEvent& event) { event.Skip(); }

void clResizableTooltip::Clear() { m_treeCtrl->DeleteAllItems(); }

void clResizableTooltip::ShowTip()
{
    m_topLeft = ::wxGetMousePosition();
    Move(m_topLeft);
    CallAfter(&clResizableTooltip::DoSetFocus);
}

void clResizableTooltip::OnKeyDown(wxTreeEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        //  Cancel this tip
        clCommandEvent destroyEvent(wxEVT_TOOLTIP_DESTROY);
        EventNotifier::Get()->AddPendingEvent(destroyEvent);
    }
}

void clResizableTooltip::DoSetFocus()
{
    Show(true);
    if(!m_treeCtrl->IsEmpty()) {
        m_treeCtrl->SetFocus();
        m_treeCtrl->SelectItem(m_treeCtrl->GetRootItem());
    }
}
