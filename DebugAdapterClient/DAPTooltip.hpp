#ifndef DAPTOOLTIP_HPP
#define DAPTOOLTIP_HPP

#include "clThemedTreeCtrl.h"
#include "dap/Client.hpp"

#include <unordered_map>
#include <wx/popupwin.h>

struct TooltipItemData : public wxTreeItemData {
    int refId = wxNOT_FOUND;
    wxString value;
    TooltipItemData(int id, const wxString& v)
        : refId(id)
        , value(v)
    {
    }
    virtual ~TooltipItemData() {}
};

class DAPTooltip : public wxPopupWindow
{
    dap::Client* m_client = nullptr;
    std::unordered_map<int, wxTreeItemId> m_pending_items;
    clThemedTreeCtrl* m_ctrl = nullptr;

protected:
    void OnItemExpanding(wxTreeEvent& event);
    void OnMenu(wxTreeEvent& event);
    void OnKeyDown(wxKeyEvent& event);

private:
    TooltipItemData* GetItemData(const wxTreeItemId& item);

public:
    DAPTooltip(dap::Client* client, const wxString& expression, const wxString& result, const wxString& type,
               int variableReference);
    virtual ~DAPTooltip();
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};

#endif // DAPTOOLTIP_HPP
