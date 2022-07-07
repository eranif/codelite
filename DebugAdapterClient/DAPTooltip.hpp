#ifndef DAPTOOLTIP_HPP
#define DAPTOOLTIP_HPP

#include "clResizableTooltip.h"
#include "dap/Client.hpp"

#include <unordered_map>

struct TooltipItemData : public wxTreeItemData {
    int refId = wxNOT_FOUND;
    TooltipItemData(int id)
        : refId(id)
    {
    }
    virtual ~TooltipItemData() {}
};

class DAPTooltip : public clResizableTooltip
{
    dap::Client* m_client = nullptr;
    std::unordered_map<int, wxTreeItemId> m_pending_items;

protected:
    void OnItemExpanding(wxTreeEvent& event) override;

private:
    TooltipItemData* GetItemData(const wxTreeItemId& item);

public:
    DAPTooltip(wxWindow* parent, dap::Client* client, const wxString& expression, const wxString& result,
               const wxString& type, int variableReference);
    virtual ~DAPTooltip();
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};

#endif // DAPTOOLTIP_HPP
