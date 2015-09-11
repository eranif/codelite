#ifndef CLRESIZABLETOOLTIP_H
#define CLRESIZABLETOOLTIP_H
#include "wxcrafter_plugin.h"
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TOOLTIP_DESTROY, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TOOLTIP_ITEM_EXPANDING, clCommandEvent);

class WXDLLIMPEXP_SDK clResizableTooltip : public clResizableTooltipBase
{
    bool m_dragging;
    wxEvtHandler* m_owner;
    wxPoint m_topLeft;

protected:
    virtual void OnCaptureLost(wxMouseCaptureLostEvent& event);
    void DoUpdateSize(bool performClean);

public:
    clResizableTooltip(wxEvtHandler* owner);
    virtual ~clResizableTooltip();

    /**
     * @brief return the event owner for this tooltip
     * @return
     */
    wxEvtHandler* GetOwner() { return m_owner; }

    /**
     * @brief delete all items in the tree control
     */
    virtual void Clear();

    /**
     * @brief move the tooltip to the mouse position and show it
     */
    virtual void ShowTip();

protected:
    /**
     * @brief user is expanding a tree item
     * @param event
     */
    virtual void OnItemExpanding(wxTreeEvent& event);
    virtual void OnStatusBarLeftDown(wxMouseEvent& event);
    virtual void OnStatusBarLeftUp(wxMouseEvent& event);
    virtual void OnStatusBarMotion(wxMouseEvent& event);
    virtual void OnStatusEnterWindow(wxMouseEvent& event);
    virtual void OnStatusLeaveWindow(wxMouseEvent& event);
    virtual void OnCheckMousePosition(wxTimerEvent& event);
};
#endif // CLRESIZABLETOOLTIP_H
