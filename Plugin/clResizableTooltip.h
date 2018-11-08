//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clResizableTooltip.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLRESIZABLETOOLTIP_H
#define CLRESIZABLETOOLTIP_H

#include "clResizableTooltipBase.h"
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TOOLTIP_DESTROY, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_TOOLTIP_ITEM_EXPANDING, clCommandEvent);

class WXDLLIMPEXP_SDK clResizableTooltip : public clResizableTooltipBase
{
    bool m_dragging;
    wxEvtHandler* m_owner;
    wxPoint m_topLeft;

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
    void DoSetFocus();

    virtual void OnKeyDown(wxTreeEvent& event);
    /**
     * @brief user is expanding a tree item
     * @param event
     */
    virtual void OnItemExpanding(wxTreeEvent& event);
};
#endif // CLRESIZABLETOOLTIP_H
