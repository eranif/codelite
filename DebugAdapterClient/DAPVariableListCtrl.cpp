#include "DAPVariableListCtrl.hpp"

#include "globals.h"
#include "macros.h"

#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

#if defined(__WXMSW__) || defined(__WXMAC__)
#define BORDER_STYLE wxBORDER_SIMPLE
#else
#define BORDER_STYLE wxBORDER_THEME
#endif

DAPVariableListCtrl::DAPVariableListCtrl(wxWindow* parent, dap::Client* client, dap::EvaluateContext dapContext,
                                         wxWindowID id, const wxPoint& pos, const wxSize& size)
    : clThemedTreeCtrl(parent, id, pos, size, BORDER_STYLE | wxTR_HIDE_ROOT | wxTR_MULTIPLE)
    , m_client(client)
    , m_dapContext(dapContext)
{
    SetShowHeader(false);
    AddHeader(_("Expression"));
    AddHeader(_("Value"));
    AddHeader(_("Type"));
    AddRoot(_("Variables"));

    Bind(wxEVT_TREE_ITEM_EXPANDING, &DAPVariableListCtrl::OnItemExpanding, this);
    Bind(wxEVT_TREE_ITEM_MENU, &DAPVariableListCtrl::OnMenu, this);
}

DAPVariableListCtrl::~DAPVariableListCtrl() {}

void DAPVariableListCtrl::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    CHECK_COND_RET(ItemHasChildren(item));

    wxTreeItemIdValue cookie;
    auto child = GetFirstChild(item, cookie);
    CHECK_ITEM_RET(child);

    wxString text = GetItemText(child);
    if(text != "<dummy>") {
        // already evaluated
        return;
    }

    // remove the fake children
    DeleteChildren(item);

    auto cd = GetItemData(item);
    CHECK_COND_RET(cd->refId != wxNOT_FOUND);

    m_client->GetChildrenVariables(cd->refId, m_dapContext);
    m_pending_items.insert({ cd->refId, item });
}

void DAPVariableListCtrl::OnMenu(wxTreeEvent& event)
{
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);

    wxMenu menu;
    menu.Append(XRCID("dap_copy_var_value"), _("Copy"));
    menu.Bind(
        wxEVT_MENU,
        [this, item](wxCommandEvent& e) {
            wxUnusedVar(e);
            auto cd = GetItemData(item);
            CHECK_PTR_RET(cd);
            ::CopyToClipboard(cd->value);
        },
        XRCID("dap_copy_var_value"));
    PopupMenu(&menu);
}

DAPVariableListCtrlItemData* DAPVariableListCtrl::GetItemData(const wxTreeItemId& item)
{
    if(!item) {
        return nullptr;
    }

    auto ptr = clThemedTreeCtrl::GetItemData(item);
    CHECK_PTR_RET_NULL(ptr);

    return dynamic_cast<DAPVariableListCtrlItemData*>(ptr);
}

void DAPVariableListCtrl::AddWatch(const wxString& expression, const wxString& value, const wxString& type, int varRef)
{
    wxString root_text;
    auto root = GetRootItem();
    auto item = AppendItem(root, expression, -1, -1, new DAPVariableListCtrlItemData(varRef, value));
    SetItemText(item, value, 1);
    SetItemText(item, type, 2);
    if(varRef > 0) {
        AppendItem(item, "<dummy>");
    }
}

void DAPVariableListCtrl::UpdateChildren(int varId, dap::VariablesResponse* response)
{
    if(m_pending_items.count(varId) == 0) {
        return;
    }

    wxTreeItemId item = m_pending_items[varId];
    m_pending_items.erase(varId);

    // update the tree
    for(auto var : response->variables) {
        auto child =
            AppendItem(item, var.name, -1, -1, new DAPVariableListCtrlItemData(var.variablesReference, var.value));
        SetItemText(child, var.value, 1);
        SetItemText(child, var.type, 2);

        if(var.variablesReference > 0) {
            AppendItem(child, "<dummy>");
        }
    }
    Expand(item);
}
