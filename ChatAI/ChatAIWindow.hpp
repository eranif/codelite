#pragma once

#include "ChatAIConfig.hpp"
#include "OllamaClient.hpp"
#include "UI.hpp"
#include "cl_command_event.h"

class ChatAI;
class ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent, ChatAI* plugin);
    virtual ~ChatAIWindow();
    void ShowSettings();

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
    void UpdateTheme();
    void DoSendPrompt();
    void OnChatAIOutput(OllamaEvent& event);
    void OnChatAIOutputDone(OllamaEvent& event);
    void OnModels(OllamaEvent& event);
    void OnFileSaved(clCommandEvent& event);
    void PopulateModels();
    void SetFocusToActiveEditor();
    void AppendOutputText(const wxString& message);

private:
    ChatAI* m_plugin = nullptr;
    wxChoice* m_activeModel = nullptr;
    bool m_autoRestart = false;
};

wxDECLARE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_START, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_INTERRUPT, clCommandEvent);
