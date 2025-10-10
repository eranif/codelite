#include "ChatAIWindowFrame.hpp"

#include "event_notifier.h"
#include "globals.h"

ChatAIWindowFrame::ChatAIWindowFrame(wxWindow* parent, ChatAI* plugin)
    : ChatAIWindowFrameBase(parent)
{
    wxIconBundle app_icons;
    if (clGetManager()->GetStdIcons()->GetIconBundle("codelite-logo", &app_icons)) {
        SetIcons(app_icons);
    }

    m_view = new ChatAIWindow(this, plugin);
    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());
    GetSizer()->Fit(this);
    Hide();
}

ChatAIWindowFrame::~ChatAIWindowFrame() {}

void ChatAIWindowFrame::OnClose(wxCloseEvent& event)
{
    wxUnusedVar(event);
    Hide();
}

bool ChatAIWindowFrame::Show(bool show)
{
    if (show) {
        ChatAIWindowFrameBase::Show(true);
        if (m_showFirstTime) {
            AdjustSize();
            m_showFirstTime = false;
        }
        m_view->GetStcInput()->CallAfter(&wxStyledTextCtrl::SetFocus);
    } else {
        ChatAIWindowFrameBase::Show(false);
    }
    return true;
}

void ChatAIWindowFrame::AdjustSize()
{
    wxFrame* top_level_frame = EventNotifier::Get()->TopFrame();
    wxRect frameSize = top_level_frame->GetSize();
    frameSize.Deflate(100);
    SetSize(frameSize.GetSize());
    CentreOnParent();

#if defined(__WXMAC__) || defined(__WXMSW__)
    Move(wxNOT_FOUND, top_level_frame->GetPosition().y);
#endif
    PostSizeEvent();
}
