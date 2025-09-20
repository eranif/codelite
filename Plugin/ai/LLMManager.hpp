#pragma once

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/timer.h>

class WXDLLIMPEXP_SDK LLMManager : public wxEvtHandler
{
public:
    using Callback = std::function<void(const std::string& message, bool is_completed, bool is_error)>;
    static LLMManager& GetInstance();

    /// Send prompt to the available LLM. Return a unique ID to be used when querying for the response.
    /// If no LLM is available, return `std::nullopt`.
    std::optional<uint64_t> Chat(const wxString& prompt, Callback cb);

    /// Return true if the LLM is available.
    inline bool IsAvailable() const { return m_isAvailable; }

    bool IsProcessing(uint64_t request_id) const
    {
        auto iter = std::find_if(m_requetstQueue.begin(), m_requetstQueue.end(), [request_id](const auto& p) {
            return p.first == request_id;
        });

        return iter != m_requetstQueue.end();
    }

private:
    LLMManager();
    ~LLMManager();

    void OnResponse(clCommandEvent& event);
    void OnResponseError(clCommandEvent& event);

    void OnTimer(wxTimerEvent& e);

    std::unordered_map<uint64_t, std::string> m_responses;
    bool m_isAvailable{false};
    wxTimer m_timer;
    std::vector<std::pair<uint64_t, Callback>> m_requetstQueue;
};
