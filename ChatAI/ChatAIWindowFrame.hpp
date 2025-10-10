#ifndef CHATAIWINDOWFRAME_HPP
#define CHATAIWINDOWFRAME_HPP

#include "ChatAIWindow.hpp"
#include "UI.hpp"

class ChatAI;
class ChatAIWindowFrame : public ChatAIWindowFrameBase
{
public:
    ChatAIWindowFrame(wxWindow* parent, ChatAI* plugin);
    ~ChatAIWindowFrame() override;

    bool Show(bool show = true) override;

protected:
    void OnClose(wxCloseEvent& event) override;
    void AdjustSize();

    ChatAIWindow* m_view{nullptr};
    bool m_showFirstTime{true};
};
#endif // CHATAIWINDOWFRAME_HPP
