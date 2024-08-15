#pragma once

#include "ChatAIConfig.hpp"
#include "UI.hpp"
#include "cl_command_event.h"

class ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent, ChatAIConfig& config);
    virtual ~ChatAIWindow();
    void ShowSettings();

protected:
    void OnStop(wxCommandEvent& event) override;
    void OnStopUI(wxUpdateUIEvent& event) override;
    void OnInputUI(wxUpdateUIEvent& event) override;
    void OnSend(wxCommandEvent& event) override;
    void OnSendUI(wxUpdateUIEvent& event) override;
    void OnUpdateTheme(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnSettings(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void UpdateTheme();
    void SendPromptEvent();
    void OnChatAIStarted(clCommandEvent& event);
    void OnChatAIOutput(clCommandEvent& event);
    void OnChatAIStderr(clCommandEvent& event);
    void OnChatAITerminated(clCommandEvent& event);
    void PopulateModels();
    void OnActiveModelChanged(wxCommandEvent& event);

private:
    ChatAIConfig& m_config;
    bool m_llamaCliRunning = false;
    wxChoice* m_activeModel = nullptr;
};

wxDECLARE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDECLARE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);
