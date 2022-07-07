#include "DAPTooltip.hpp"

#include "clResizableTooltip.h" // wxEVT_TOOLTIP_DESTROY
#include "clSystemSettings.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "macros.h"

#include <wx/dcclient.h>
#include <wx/sizer.h>

#ifdef __WXMSW__
#define BORDER_STYLE wxBORDER_SIMPLE
#else
#define BORDER_STYLE wxBORDER_THEME
#endif

DAPTooltip::DAPTooltip(dap::Client* client, const wxString& expression, const wxString& result, const wxString& type,
                       int variableReference)
    : wxPopupWindow(EventNotifier::Get()->TopFrame())
    , m_client(client)
{
    wxClientDC dc(this);
    dc.SetFont(DrawingUtils::GetDefaultGuiFont());
    wxSize sz = dc.GetTextExtent("Tp");
    sz.SetWidth(sz.GetWidth() * 50);   // 50 chars width
    sz.SetHeight(sz.GetHeight() * 15); // 10 lines

    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_ctrl = new clThemedTreeCtrl(this, wxID_ANY, wxDefaultPosition, sz, BORDER_STYLE);
    GetSizer()->Add(m_ctrl, 1, wxEXPAND);

    m_ctrl->AddHeader(_("Result"));
    m_ctrl->AddHeader(_("Type"));
    m_ctrl->SetShowHeader(false);

    wxString root_text;
    root_text << expression << " = " << result;
    auto root = m_ctrl->AddRoot(root_text, -1, -1, new TooltipItemData(variableReference));
    m_ctrl->SetItemText(root, type, 1);
    m_ctrl->Bind(wxEVT_TREE_ITEM_EXPANDING, &DAPTooltip::OnItemExpanding, this);
    m_ctrl->Bind(wxEVT_KEY_DOWN, &DAPTooltip::OnKeyDown, this);

    if(variableReference > 0) {
        // we have children
        m_ctrl->AppendItem(m_ctrl->GetRootItem(), "<dummy>");
    }
    GetSizer()->Fit(this);
    m_ctrl->CallAfter(&clThemedTreeCtrl::SelectItem, m_ctrl->GetRootItem(), true);
}

DAPTooltip::~DAPTooltip() {}

void DAPTooltip::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    auto item = event.GetItem();
    CHECK_ITEM_RET(item);
    CHECK_COND_RET(m_ctrl->ItemHasChildren(item));

    wxTreeItemIdValue cookie;
    auto child = m_ctrl->GetFirstChild(item, cookie);
    CHECK_ITEM_RET(child);

    wxString text = m_ctrl->GetItemText(child);
    if(text != "<dummy>") {
        // already evaluated
        return;
    }

    // remove the fake children
    m_ctrl->DeleteChildren(item);

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

    auto ptr = m_ctrl->GetItemData(item);
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
        auto child = m_ctrl->AppendItem(item, display_text, -1, -1, new TooltipItemData(var.variablesReference));
        m_ctrl->SetItemText(child, var.type, 1);

        if(var.variablesReference > 0) {
            m_ctrl->AppendItem(child, "<dummy>");
        }
    }
    m_ctrl->Expand(item);
}

void DAPTooltip::OnKeyDown(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        //  Cancel this tip
        clCommandEvent destroyEvent(wxEVT_TOOLTIP_DESTROY);
        EventNotifier::Get()->AddPendingEvent(destroyEvent);
    }
}
