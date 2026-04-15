#pragma once

#include "UI.hpp"
#include "ai/HistoryManager.hpp"
#include "codelite_exports.h"

#include <map>
#include <optional>

class ChatHistoryDialog;
class WXDLLIMPEXP_SDK ChatHistoryPage : public ChatHistoryPageBase
{
public:
    explicit ChatHistoryPage(wxChoicebook* parent, const wxString& endpoint);
    ~ChatHistoryPage() override;

    wxDataViewListCtrl* GetListView() { return m_dvListCtrlPrompts; }
    void Clear();
    void DeleteSelections();
    std::optional<wxString> GetSelection() const;
    const wxString& GetEndpoint() const { return m_endpoint; }

private:
    wxString m_endpoint;
    std::map<wxString, llm::HistoryEntry> m_chats;
};
