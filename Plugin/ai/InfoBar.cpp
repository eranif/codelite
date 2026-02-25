#include "ai/InfoBar.hpp"

#include "ai/ChatAIWindowFrame.hpp"
#include "file_logger.h"

InfoBar::InfoBar(wxWindow* parent, std::shared_ptr<std::promise<llm::UserAnswer>> promise_ptr)
    : wxInfoBar(parent)
    , m_promise_ptr{promise_ptr}
{
    SetShowHideEffects(wxSHOW_EFFECT_NONE, wxSHOW_EFFECT_NONE);
    Hide();
    AddButton(wxID_YES, _("Yes"));
    AddButton(wxID_NO, _("No"));
    AddButton(wxID_YESTOALL, _("Trust"));

    Bind(wxEVT_BUTTON, &InfoBar::OnYes, this, wxID_YES);
    Bind(wxEVT_BUTTON, &InfoBar::OnNo, this, wxID_NO);
    Bind(wxEVT_BUTTON, &InfoBar::OnTrust, this, wxID_YESTOALL);
}

void InfoBar::Dismiss()
{
    wxInfoBar::Dismiss();
    ChatAIWindowFrame* parent = dynamic_cast<ChatAIWindowFrame*>(GetParent());
    CHECK_PTR_RET(parent);

    parent->CallAfter(&ChatAIWindowFrame::DeleteInfoBar, this);
}

void InfoBar::OnYes(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Dismiss();
    m_promise_ptr->set_value(llm::UserAnswer::kYes);
}

void InfoBar::OnNo(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Dismiss();
    m_promise_ptr->set_value(llm::UserAnswer::kNo);
}

void InfoBar::OnTrust(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Dismiss();
    m_promise_ptr->set_value(llm::UserAnswer::kTrust);
}