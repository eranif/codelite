#ifndef DAPVARIABLELISTCTRL_HPP
#define DAPVARIABLELISTCTRL_HPP

#include "clThemedTreeCtrl.h"
#include "dap/Client.hpp"

#include <unordered_map>
#include <vector>

struct DAPVariableListCtrlItemData : public wxTreeItemData {
    int refId = wxNOT_FOUND;
    wxString value;
    DAPVariableListCtrlItemData(int id, const wxString& v)
        : refId(id)
        , value(v)
    {
    }
    virtual ~DAPVariableListCtrlItemData() {}
};

class DAPVariableListCtrl : public clThemedTreeCtrl
{
protected:
    std::unordered_map<int, wxTreeItemId> m_pending_items;
    dap::Client* m_client = nullptr;
    dap::EvaluateContext m_dapContext = dap::EvaluateContext::HOVER;

protected:
    void OnItemExpanding(wxTreeEvent& event);
    void OnMenu(wxTreeEvent& event);
    DAPVariableListCtrlItemData* GetItemData(const wxTreeItemId& item);

public:
    DAPVariableListCtrl(wxWindow* parent, dap::Client* client, dap::EvaluateContext dapContext,
                        wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize);
    virtual ~DAPVariableListCtrl();
    /**
     * @brief add a watch to the list of watches
     */
    void AddWatch(const wxString& expression, const wxString& value, const wxString& type, int varRef);
    /**
     * @brief update the ui with the children of `varId`
     * @param varId
     * @param response
     */
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};

#endif // DAPVARIABLELISTCTRL_HPP
