#pragma once

#include "UI.hpp"

class ChatAIWindow : public AssistanceAIChatWindowBase
{
public:
    ChatAIWindow(wxWindow* parent);
    virtual ~ChatAIWindow();

protected:
    void OnSend(wxCommandEvent& event) override;
    void OnSendUI(wxUpdateUIEvent& event) override;
    void OnUpdateTheme(wxCommandEvent& event);
    void UpdateTheme();
};

wxDECLARE_EVENT(wxEVT_CHATAI_SEND, wxCommandEvent);
