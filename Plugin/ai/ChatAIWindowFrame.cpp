#include "ChatAIWindowFrame.hpp"

#include "LLMManager.hpp"
#include "event_notifier.h"
#include "globals.h"

ChatAIWindowFrame::ChatAIWindowFrame(wxWindow* parent, ChatAI* plugin)
    : ChatAIWindowFrameBase(parent)
{
    wxIconBundle app_icons;
    if (clGetManager()->GetStdIcons()->GetIconBundle("codelite-logo", &app_icons)) {
        SetIcons(app_icons);
    }

    llm::Manager::GetInstance().Bind(wxEVT_LLM_STARTED, &ChatAIWindowFrame::OnLLMRestarted, this);

    m_view = new ChatAIWindow(this);
    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());
    GetSizer()->Fit(this);
    Hide();
}

ChatAIWindowFrame::~ChatAIWindowFrame()
{
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_STARTED, &ChatAIWindowFrame::OnLLMRestarted, this);
}

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
        UpdateLabel();

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

void ChatAIWindowFrame::OnLLMRestarted(clLLMEvent& event)
{
    event.Skip();
    UpdateLabel();
}

void ChatAIWindowFrame::UpdateLabel()
{
    static const wxString RIGHT_ARROW = wxT(" ▶ ");
    auto& manager = llm::Manager::GetInstance();
    auto endpoint = manager.GetActiveEndpoint();
    wxString label;
    if (endpoint.has_value()) {
        label << _("Assistant") << RIGHT_ARROW << manager.GetModelName().value_or("<unknown>") << RIGHT_ARROW
              << endpoint.value();
    } else {
        label << _("Assistant");
    }
    SetLabel(label);
}
