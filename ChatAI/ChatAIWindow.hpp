#pragma once

#include "CustomControls/IndicatorPanel.hpp"
#include "MarkdownStyler.hpp"
#include "OllamaClient.hpp"
#include "UI.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "wxTerminalCtrl/wxTerminalOutputCtrl.hpp"

#include <wx/activityindicator.h>
#include <wx/stattext.h>
#include <wx/timer.h>

class ChatAI;
class ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent, ChatAI* plugin);
    virtual ~ChatAIWindow();
    wxString GetActiveModel() const { return m_activeModel->GetStringSelection(); }

protected:
    void OnStop(wxCommandEvent& event);
    void OnAutoScroll(wxCommandEvent& event);
    void OnAutoScrollUI(wxUpdateUIEvent& event);
    void OnStopUI(wxUpdateUIEvent& event);
    void OnInputUI(wxUpdateUIEvent& event) override;
    void OnSend(wxCommandEvent& event);
    void OnSendUI(wxUpdateUIEvent& event);
    void OnUpdateTheme(wxCommandEvent& event);
    void OnModelChanged(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnNewSession(wxCommandEvent& event);
    void OnRefreshModelList(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnLog(LLMEvent& event);
    void UpdateTheme();
    void DoSendPrompt();
    void OnChatStarted(LLMEvent& event);
    void OnChatAIOutput(LLMEvent& event);
    void OnChatAIOutputDone(LLMEvent& event);
    void OnModels(LLMEvent& event);
    void OnThinking(LLMEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void PopulateModels();
    void SetFocusToActiveEditor();
    void StyleOutput();
    void AppendOutput(const wxString& text);
    void AppendMarker();
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void LoadGlobalConfig();
    void RestoreUI();

    /// Clears the output view, does not change the model history.
    void DoClearOutputView();
    /// Clear the view (input & output) and reset the client.
    void DoReset();
    bool DoCreateWorkspaceSettings();
    void DoLogMessage(const wxString& message, LLMLogLevel log_level);

    /// Return the relevant configuration file. If a workspace file is opened, we use the workspace specific
    /// configuration file. If no workspace is opened, we use the global settings.
    wxString GetConfigurationFilePath() const;

    void ShowIndicator(bool show);
    void NotifyThinking(bool thinking);
    void OnTimer(wxTimerEvent& event);

private:
    ChatAI* m_plugin{nullptr};
    wxChoice* m_activeModel{nullptr};
    wxTerminalOutputCtrl* m_logView{nullptr};
    std::unique_ptr<MarkdownStyler> m_markdownStyler;
    bool m_thinking{false};
    IndicatorPanel* m_statusPanel{nullptr};
    bool m_autoScroll{true};
};
