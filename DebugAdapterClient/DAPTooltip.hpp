#ifndef DAPTOOLTIP_HPP
#define DAPTOOLTIP_HPP

#include "dap/Client.hpp"

#include <wx/popupwin.h>

class DAPVariableListCtrl;
class DAPTooltip : public wxPopupWindow
{
    DAPVariableListCtrl* m_list = nullptr;

protected:
    void OnKeyDown(wxKeyEvent& event);

public:
    DAPTooltip(dap::Client* client,
               const wxString& expression,
               const wxString& result,
               const wxString& type,
               int variableReference);
    virtual ~DAPTooltip() = default;
    void UpdateChildren(int varId, dap::VariablesResponse* response);
};

#endif // DAPTOOLTIP_HPP
