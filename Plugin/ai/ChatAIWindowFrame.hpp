#ifndef CHATAIWINDOWFRAME_HPP
#define CHATAIWINDOWFRAME_HPP

#include "ChatAIWindow.hpp"
#include "UI.hpp"
#include "codelite_exports.h"

class InfoBar;
class ChatAI;
class WXDLLIMPEXP_SDK ChatAIWindowFrame : public ChatAIWindowFrameBase
{
public:
    ChatAIWindowFrame(wxWindow* parent, ChatAI* plugin);
    ~ChatAIWindowFrame() override;

    bool Show(bool show = true) override;
    ChatAIWindow* GetChatWindow() { return m_view; }

    void DeleteInfoBar(InfoBar* bar);

protected:
    void OnClose(wxCloseEvent& event) override;
    void OnLLMRestarted(clLLMEvent& event);

    void AdjustSize();
    void UpdateLabel();

    ChatAIWindow* m_view{nullptr};
    bool m_showFirstTime{true};
};
#endif // CHATAIWINDOWFRAME_HPP
