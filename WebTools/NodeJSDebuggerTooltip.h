//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NodeJSDebuggerTooltip.h
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

#ifndef NODEJSDEBUGGERTOOLTIP_H
#define NODEJSDEBUGGERTOOLTIP_H

#include "clResizableTooltip.h"
#include "NodeJSOuptutParser.h"

class NodeJSDebuggerTooltip : public clResizableTooltip
{
    std::map<int, NodeJSHandle> m_handles;
    wxString m_expression;
    std::vector<PendingLookupT> m_pendingLookupRefs;

protected:
    class ClientData : public wxTreeItemData
    {
        NodeJSHandle m_handle;
        bool m_expanded;

    public:
        ClientData(const NodeJSHandle& h, bool expanded = false)
            : m_handle(h)
            , m_expanded(expanded)
        {
        }
        virtual ~ClientData() {}

        void SetExpanded(bool expanded) { this->m_expanded = expanded; }
        bool IsExpanded() const { return m_expanded; }
        const NodeJSHandle& GetHandle() const { return m_handle; }
    };

protected:
    void DoAddKnownRefs(const std::vector<std::pair<int, wxString> >& refs, const wxTreeItemId& parent);
    void DoAddUnKnownRefs(const std::vector<std::pair<int, wxString> >& refs, const wxTreeItemId& parent);
    wxTreeItemId AddLocal(const wxTreeItemId& parent, const wxString& name, int refId);

public:
    NodeJSDebuggerTooltip(wxEvtHandler* owner, const wxString& expression);
    virtual ~NodeJSDebuggerTooltip();
    void ShowTip(const wxString& jsonOutput);

protected:
    virtual void OnItemExpanding(wxTreeEvent& event);
    void OnLookup(clDebugEvent& event);
};

#endif // NODEJSDEBUGGERTOOLTIP_H
