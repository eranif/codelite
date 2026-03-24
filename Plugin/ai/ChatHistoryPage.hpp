#pragma once

#include "UI.hpp"
#include "codelite_exports.h"

#include <optional>

class ChatHistoryDialog;
class WXDLLIMPEXP_SDK ChatHistoryPage : public ChatHistoryPageBase
{
public:
    explicit ChatHistoryPage(wxChoicebook* parent, ChatHistoryDialog* dlg, const wxString& endpoint);
    ~ChatHistoryPage() override;

    wxDataViewListCtrl* GetListView() { return m_dvListCtrlPrompts; }
    void Clear();
    void DeleteSelections();
    std::optional<wxString> GetSelection() const;
    const wxString& GetEndpoint() const { return m_endpoint; }

protected:
    void OnItemActivated(wxDataViewEvent& event) override;

private:
    wxString m_endpoint;
    ChatHistoryDialog* m_dialog{nullptr};
};
