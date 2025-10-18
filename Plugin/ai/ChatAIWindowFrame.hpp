#ifndef CHATAIWINDOWFRAME_HPP
#define CHATAIWINDOWFRAME_HPP

#include "ChatAIWindow.hpp"
#include "UI.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ChatAI;
class WXDLLIMPEXP_SDK ChatAIWindowFrame : public ChatAIWindowFrameBase
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
