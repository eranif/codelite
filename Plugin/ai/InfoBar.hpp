#pragma once
#include "ai/ChatAIWindow.hpp"
#include "ai/Common.hpp"
#include "codelite_exports.h"

#include <future>
#include <vector>
#include <wx/infobar.h>

class WXDLLIMPEXP_SDK InfoBar : public wxInfoBar
{
public:
    explicit InfoBar(wxWindow* parent, std::shared_ptr<std::promise<llm::UserAnswer>> promise_ptr);
    ~InfoBar() override = default;

    /**
     * @brief Dismisses the current message and shows the next pending message if available.
     *
     * This method overrides the base class Dismiss() behavior. If there are pending messages
     * in the queue, it removes and displays the next message instead of hiding the info bar.
     * If no pending messages exist, it delegates to the base class to hide the info bar.
     *
     * @return void This method does not return a value.
     */
    void Dismiss() override;

protected:
    void OnYes(wxCommandEvent& e);
    void OnNo(wxCommandEvent& e);
    void OnTrust(wxCommandEvent& e);

private:
    std::vector<std::pair<wxString, int>> m_pendingMessages;
    std::shared_ptr<std::promise<llm::UserAnswer>> m_promise_ptr;
};
