#include "clSTCContainerStylerBase.hpp"

#include "ColoursAndFontsManager.h"
#include "codelite_events.h"
#include "event_notifier.h"

clSTCContainerStylerBase::clSTCContainerStylerBase(wxStyledTextCtrl* ctrl)
    : m_ctrl(ctrl)
{
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    m_ctrl->SetIdleStyling(wxSTC_IDLESTYLING_AFTERVISIBLE); // style all the visible text at once
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &clSTCContainerStylerBase::OnThemChanged, this);
}

clSTCContainerStylerBase::~clSTCContainerStylerBase()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &clSTCContainerStylerBase::OnThemChanged, this);
}

void clSTCContainerStylerBase::OnThemChanged(wxCommandEvent& event)
{
    event.Skip();
    InitInternal();
}

void clSTCContainerStylerBase::StyleText()
{
    CHECK_PTR_RET(m_ctrl);
    CHECK_COND_RET(m_on_style_callback != nullptr);

    int startPos = m_ctrl->GetEndStyled();
    int endPos = m_ctrl->GetLastPosition();
    wxString text = m_ctrl->GetTextRange(startPos, endPos);
    // The scintilla syntax in wx3.1.1 changed
    m_ctrl->StartStyling(startPos);
    clSTCAccessor accessor{ m_ctrl, startPos, endPos };
    m_on_style_callback(accessor);
}

void clSTCContainerStylerBase::InitInternal()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_PTR_RET(lexer);

    lexer->Apply(m_ctrl);
    m_ctrl->SetLexer(wxSTC_LEX_CONTAINER);
    InitStyles();
}
