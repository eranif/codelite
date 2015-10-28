#include <algorithm>
#include "NodeJSDebuggerTooltip.h"
#include "json_node.h"
#include "NodeJSOuptutParser.h"
#include "globals.h"
#include "macros.h"
#include "NoteJSWorkspace.h"
#include "event_notifier.h"
#include "NodeJSEvents.h"

NodeJSDebuggerTooltip::NodeJSDebuggerTooltip(wxEvtHandler* owner, const wxString& expression)
    : clResizableTooltip(owner)
    , m_expression(expression)
{
    MSWSetNativeTheme(m_treeCtrl);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_TOOLTIP_LOOKUP, &NodeJSDebuggerTooltip::OnLookup, this);
}

NodeJSDebuggerTooltip::~NodeJSDebuggerTooltip()
{
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_TOOLTIP_LOOKUP, &NodeJSDebuggerTooltip::OnLookup, this);
}

void NodeJSDebuggerTooltip::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET(event.GetItem());

    ClientData* d = dynamic_cast<ClientData*>(m_treeCtrl->GetItemData(event.GetItem()));

    CHECK_PTR_RET(d);
    if(d->IsExpanded()) {
        // nothing to be done here
        return;
    }

    d->SetExpanded(true);

    // Prepare list of refs that we don't have
    std::vector<std::pair<int, wxString> > unknownRefs;
    std::vector<std::pair<int, wxString> > knownRefs;
    const NodeJSHandle& h = d->GetHandle();
    std::for_each(h.properties.begin(), h.properties.end(), [&](const std::pair<int, wxString>& p) {
        if(m_handles.count(p.first) == 0) {
            unknownRefs.push_back(p);
        } else {
            knownRefs.push_back(p);
        }
    });
    CallAfter(&NodeJSDebuggerTooltip::DoAddKnownRefs, knownRefs, event.GetItem());
    CallAfter(&NodeJSDebuggerTooltip::DoAddUnKnownRefs, unknownRefs, event.GetItem());

    // Delete the dummy node
    m_treeCtrl->CallAfter(&wxTreeCtrl::DeleteChildren, event.GetItem());
}

void NodeJSDebuggerTooltip::ShowTip(const wxString& jsonOutput)
{
    JSONRoot root(jsonOutput);
    JSONElement body = root.toElement().namedObject("body");

    NodeJSOuptutParser p;
    NodeJSHandle h = p.ParseRef(body, m_handles);

    wxString rootText;
    rootText << m_expression;
    if(!h.value.IsEmpty()) {
        rootText << " = " << h.value;
    }

    wxTreeItemId rootItem = m_treeCtrl->AddRoot(rootText, -1, -1, new ClientData(h));
    if(!h.properties.empty()) {
        m_treeCtrl->AppendItem(rootItem, "Loading...");
    }

    // Add this handle
    clResizableTooltip::ShowTip();
}

void NodeJSDebuggerTooltip::DoAddKnownRefs(const std::vector<std::pair<int, wxString> >& refs,
                                           const wxTreeItemId& parent)
{
    std::for_each(
        refs.begin(), refs.end(), [&](const std::pair<int, wxString>& p) { AddLocal(parent, p.second, p.first); });
}

void NodeJSDebuggerTooltip::DoAddUnKnownRefs(const std::vector<std::pair<int, wxString> >& refs,
                                             const wxTreeItemId& parent)
{
    if(!NodeJSWorkspace::Get()->GetDebugger()) return;

    std::vector<int> handles;
    std::for_each(refs.begin(), refs.end(), [&](const std::pair<int, wxString>& p) {
        PendingLookupT pl;
        pl.parent = parent;
        pl.name = p.second;
        pl.refID = p.first;
        m_pendingLookupRefs.push_back(pl);
        handles.push_back(p.first);
    });
    NodeJSWorkspace::Get()->GetDebugger()->Lookup(handles, kNodeJSContextTooltip);
}

wxTreeItemId NodeJSDebuggerTooltip::AddLocal(const wxTreeItemId& parent, const wxString& name, int refId)
{
    // extract the value
    if(m_handles.count(refId)) {
        NodeJSHandle h = m_handles.find(refId)->second;
        wxString text;
        text << name;
        if(!h.value.IsEmpty()) {
            text << " = " << h.value;
        }
        wxTreeItemId child = m_treeCtrl->AppendItem(parent, text, -1, -1, new ClientData(h));
        if(!h.properties.empty()) {
            m_treeCtrl->AppendItem(child, "Loading...");
        }
        return child;
    }
    return wxTreeItemId();
}

void NodeJSDebuggerTooltip::OnLookup(clDebugEvent& event)
{
    JSONRoot root(event.GetString());
    JSONElement body = root.toElement().namedObject("body");
    std::vector<PendingLookupT> unresolved;

    wxTreeItemId parent;
    NodeJSOuptutParser p;
    for(size_t i = 0; i < m_pendingLookupRefs.size(); ++i) {
        const PendingLookupT& pl = m_pendingLookupRefs.at(i);
        if(!parent.IsOk()) {
            parent = pl.parent;
        }
        wxString nameID;
        nameID << pl.refID;

        if(!body.hasNamedObject(nameID)) {
            unresolved.push_back(pl);
            continue;
        }

        // Parse and add this ref to the global m_handles map
        JSONElement ref = body.namedObject(nameID);
        NodeJSHandle h = p.ParseRef(ref, m_handles);
        h.name = pl.name;
        if(!h.IsOk()) continue;

        // Add the local
        AddLocal(pl.parent, pl.name, pl.refID);
    }

    if(parent.IsOk() && m_treeCtrl->HasChildren(parent) && !m_treeCtrl->IsExpanded(parent)) {
        m_treeCtrl->Expand(parent);
    }
    m_pendingLookupRefs.clear();
}
