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
    void OnClear(wxCommandEvent& event);
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
    void StyleAndPrintOutput(bool allow_partial_line = false);
    void OnWorkspaceLoaded(clWorkspaceEvent& event);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

private:
    ChatAI* m_plugin = nullptr;
    wxChoice* m_activeModel = nullptr;
    bool m_autoRestart = false;
    wxTerminalOutputCtrl* m_logView{ nullptr };
    std::unique_ptr<MarkdownStyler> m_markdownStyler;
    wxString m_bufferedLine;
};

wxDECLARE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_START, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_INTERRUPT, clCommandEvent);
