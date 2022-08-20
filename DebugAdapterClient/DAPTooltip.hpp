#ifndef DAPTOOLTIP_HPP
#define DAPTOOLTIP_HPP

#include "clThemedTreeCtrl.h"
#include "dap/Client.hpp"

#include <unordered_map>
#include <wx/popupwin.h>

class DAPVariableListCtrl;
class DAPTooltip : public wxPopupWindow
{
    dap::Client* m_client = nullptr;
    DAPVariableListCtrl* m_list = nullptr;

protected:
    void OnKeyDown(wxKeyEvent& event);

public:
    DAPTooltip(dap::Client* client, const wxString& expression, const wxString& result, const wxString& type,
               int variableReference);
    virtual ~DAPTooltip();
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};

#endif // DAPTOOLTIP_HPP
