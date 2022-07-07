#include "DAPTooltip.hpp"

#include "macros.h"

DAPTooltip::DAPTooltip(wxWindow* parent, dap::Client* client, const wxString& expression, const wxString& result,
                       const wxString& type, int variableReference)
    : clResizableTooltip(parent)
    , m_client(client)
{
    GetTreeCtrl()->AddHeader(_("Result"));
    GetTreeCtrl()->AddHeader(_("Type"));
    GetTreeCtrl()->SetShowHeader(false);

    wxString root_text;
    root_text << expression << " = " << result;
    auto root = GetTreeCtrl()->AddRoot(root_text, -1, -1, new TooltipItemData(variableReference));
    GetTreeCtrl()->SetItemText(root, type, 1);

    if(variableReference > 0) {
        // we have children
        GetTreeCtrl()->AppendItem(GetTreeCtrl()->GetRootItem(), "<dummy>");
    }
}

DAPTooltip::~DAPTooltip() {}

void DAPTooltip::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    CHECK_COND_RET(GetTreeCtrl()->ItemHasChildren(item));

    wxTreeItemIdValue cookie;
    auto child = GetTreeCtrl()->GetFirstChild(item, cookie);
    CHECK_ITEM_RET(child);

    wxString text = GetTreeCtrl()->GetItemText(child);
    if(text != "<dummy>") {
        // already evaluated
        return;
    }

    // remove the fake children
    GetTreeCtrl()->DeleteChildren(item);

    auto cd = GetItemData(item);
    CHECK_COND_RET(cd->refId != wxNOT_FOUND);

    m_client->GetChildrenVariables(cd->refId, dap::EvaluateContext::HOVER);
    m_pending_items.insert({ cd->refId, item });
}

TooltipItemData* DAPTooltip::GetItemData(const wxTreeItemId& item)
{
    if(!item) {
        return nullptr;
    }

    auto ptr = GetTreeCtrl()->GetItemData(item);
    CHECK_PTR_RET_NULL(ptr);

    return dynamic_cast<TooltipItemData*>(ptr);
}

void DAPTooltip::UpdateChildren(int varId, dap::VariablesResponse* response)
{
    if(m_pending_items.count(varId) == 0) {
        return;
    }

    wxTreeItemId item = m_pending_items[varId];
    m_pending_items.erase(varId);

    // update the tree
    for(auto var : response->variables) {
        wxString display_text;
        display_text << var.name << " = " << var.value;
        auto child = GetTreeCtrl()->AppendItem(item, display_text, -1, -1, new TooltipItemData(var.variablesReference));
        GetTreeCtrl()->SetItemText(child, var.type, 1);

        if(var.variablesReference > 0) {
            GetTreeCtrl()->AppendItem(child, "<dummy>");
        }
    }
    GetTreeCtrl()->Expand(item);
}
