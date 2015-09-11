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
    void DoAddKnownRefs(const std::map<int, wxString>& refs, const wxTreeItemId& parent);
    void DoAddUnKnownRefs(const std::map<int, wxString>& refs, const wxTreeItemId& parent);
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
