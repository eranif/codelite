#pragma once

#include "CustomControls/IndicatorPanel.hpp"
#include "MarkdownStyler.hpp"
#include "UI.hpp"
#include "ai/Common.hpp"
#include "ai/ProgressToken.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <wx/activityindicator.h>
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

protected:
    void OnAutoScroll(wxCommandEvent& event);
    void OnAutoScrollUI(wxUpdateUIEvent& event);
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
    void PopulateEndpoints();
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

private:
    wxChoice* m_choiceEndpoints{nullptr};
    std::unique_ptr<MarkdownStyler> m_markdownStyler;
    ChatState m_state{ChatState::kReady};
    IndicatorPanel* m_statusPanel{nullptr};
    bool m_autoScroll{true};
    std::shared_ptr<llm::CancellationToken> m_cancel_token{nullptr};
    ChatAI* m_plugin{nullptr};
};
