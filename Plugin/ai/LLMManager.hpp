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
    enum ResponseFlags {
        kNone = 0,
        kThinking = (1 << 0),
        kError = (1 << 1),
        kCompleted = (1 << 2),
    };

    enum ChatOptions {
        kDefault = 0,
        kNoTools = (1 << 0),
    };

    using Callback = std::function<void(const std::string& message, size_t flags)>;
    static LLMManager& GetInstance();

    /// Send prompt to the available LLM. Return a unique ID to be used when querying for the response.
    /// If no LLM is available, return `std::nullopt`.
    std::optional<uint64_t>
    Chat(const wxString& prompt, Callback cb, size_t options, const wxString& model = wxEmptyString);

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

    void OnResponse(clLLMEvent& event);
    void OnResponseError(clLLMEvent& event);

    void OnTimer(wxTimerEvent& e);

    std::unordered_map<uint64_t, std::string> m_responses;
    bool m_isAvailable{false};
    wxTimer m_timer;
    std::vector<std::pair<uint64_t, Callback>> m_requetstQueue;
};
