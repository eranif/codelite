#pragma once

#include "CustomControls/IndicatorPanel.hpp"
#include "MarkdownStyler.hpp"
#include "UI.hpp"
#include "ai/Common.hpp"
#include "ai/ProgressToken.hpp"
#include "clEditorEditEventsHandler.h"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/activityindicator.h>
#include <wx/checkbox.h>
#include <wx/msgqueue.h>
#include <wx/stattext.h>
#include <wx/timer.h>

class ChatAI;
using llm::ChatState;

class WXDLLIMPEXP_SDK ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent, ChatAI* plugin);
    virtual ~ChatAIWindow();
    wxString GetActiveModel() const { return m_choiceEndpoints->GetStringSelection(); }
    bool IsDetached() const;
    /**
     * @brief Sends a chat prompt to the AI window.
     *
     * This method sets the provided prompt as the input text and initiates sending
     * it to the chat system. If the prompt is empty, the function returns without
     * performing any action.
     *
     * @param prompt The prompt text to be sent. Must be a non-empty {@link wxString}.
     */
    void Chat(const wxString& prompt);

    /**
     * @brief Appends text to the chat AI window's output area.
     *
     * This method adds the specified text to the window's output display and optionally
     * applies styling to the newly appended content. The text is first appended via
     * AppendOutput(), and then styling is conditionally applied based on the force_style parameter.
     *
     * @param text The text string to append to the output area.
     * @param force_style If true, immediately applies styling to the output after appending;
     *                    if false, styling is deferred (may be applied later in batch).
     *
     * @return void This method does not return a value.
     *
     * @see AppendOutput()
     * @see StyleOutput()
     */
    void AppendText(const wxString& text, bool force_style = true);

    /**
     * @brief Displays a yes/no trust confirmation bar with the specified message.
     *
     * Shows an information bar (typically at the bottom of the ChatAIWindow)
     * with a question icon and the provided text, prompting the user for a trust decision.
     */
    void ShowYesNoTrustBar(const wxString& text);

protected:
    void OnTrust(wxCommandEvent& event) override;
    void OnNo(wxCommandEvent& event) override;
    void OnYes(wxCommandEvent& event) override;
    void OnCharAdded(wxStyledTextEvent& event);
    void OnAutoScroll(wxCommandEvent& event);
    void OnAutoScrollUI(wxUpdateUIEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnBusyUI(wxUpdateUIEvent& event);
    void OnDetachView(wxCommandEvent& event);
    void OnDetachViewUI(wxUpdateUIEvent& event);
    void OnHistory(wxCommandEvent& event);
    void OnHistoryUI(wxUpdateUIEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnStopUI(wxUpdateUIEvent& event);
    void OnInputUI(wxUpdateUIEvent& event) override;
    void OnSend(wxCommandEvent& event);
    void OnSendUI(wxUpdateUIEvent& event);
    void OnClearOutputViewUI(wxUpdateUIEvent& event);
    void OnUpdateTheme(wxCommandEvent& event);
    void OnEndpointChanged(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnNewSession(wxCommandEvent& event);
    void OnRestartClient(wxCommandEvent& event);
    void UpdateTheme();
    void DoSendPrompt();
    void UpdateChoices();
    void SetFocusToActiveEditor();
    void StyleOutput();
    void AppendOutput(const wxString& text);
    void AppendMarker();
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void LoadGlobalConfig();
    void RestoreUI();

    /// LLM events
    void OnLLMConfigUpdate(clLLMEvent& event);
    void OnLLMUserReplyError(clLLMEvent& event);
    void OnChatStarted(clLLMEvent& event);
    void OnChatAIOutput(clLLMEvent& event);
    void OnChatAIOutputDone(clLLMEvent& event);
    void OnThinkingStart(clLLMEvent& event);
    void OnThinkingEnd(clLLMEvent& event);

    /// Clears the output view, does not change the model history.
    void DoClearOutputView();
    /// Clear the view (input & output) and reset the client.
    void DoRestart();

    void ShowIndicator(bool show);
    void UpdateCostBar();

private:
    wxChoice* m_choiceEndpoints{nullptr};
    wxCheckBox* m_checkboxEnableTools{nullptr};
    std::unique_ptr<MarkdownStyler> m_markdownStyler;
    ChatState m_state{ChatState::kReady};
    IndicatorPanel* m_statusPanel{nullptr};
    bool m_autoScroll{true};
    std::shared_ptr<llm::CancellationToken> m_cancel_token{nullptr};
    ChatAI* m_plugin{nullptr};
    std::unique_ptr<clEditEventsHandler> m_inputEditHelper;
    std::unique_ptr<clEditEventsHandler> m_outputEditHelper;
};
