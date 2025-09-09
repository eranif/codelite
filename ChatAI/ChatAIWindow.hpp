#pragma once

#include "ChatAIConfig.hpp"
#include "MarkdownStyler.hpp"
#include "OllamaClient.hpp"
#include "UI.hpp"
#include "clWorkspaceEvent.hpp"
#include "cl_command_event.h"
#include "wxTerminalCtrl/wxTerminalOutputCtrl.hpp"

class ChatAI;
class ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent, ChatAI* plugin);
    virtual ~ChatAIWindow();

protected:
    void OnInputUI(wxUpdateUIEvent& event) override;
    void OnSend(wxCommandEvent& event) override;
    void OnSendUI(wxUpdateUIEvent& event) override;
    void OnUpdateTheme(wxCommandEvent& event);
    void OnModelChanged(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnNewSession(wxCommandEvent& event);
    void OnRefreshModelList(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnLog(OllamaEvent& event);
    void UpdateTheme();
    void DoSendPrompt();
    void OnChatAIOutput(OllamaEvent& event);
    void OnChatAIOutputDone(OllamaEvent& event);
    void OnModels(OllamaEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void PopulateModels();
    void SetFocusToActiveEditor();
    void StyleOutput();
    void AppendOutput(const wxString& text);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

    /// Clears the output view, does not change the model history.
    void DoClearOutputView();
    /// Clear the view (input & output) and reset the client.
    void DoReset();

    /// Return the relevant configuration file. If a workspace file is opened, we use the workspace specific
    /// configuration file. If no workspace is opened, we use the global settings.
    wxString GetConfigurationFilePath() const;

    void ShowIndicator(bool show);

private:
    ChatAI* m_plugin{nullptr};
    wxChoice* m_activeModel{nullptr};
    wxTerminalOutputCtrl* m_logView{nullptr};
    std::unique_ptr<MarkdownStyler> m_markdownStyler;
};

wxDECLARE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_START, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_INTERRUPT, clCommandEvent);
