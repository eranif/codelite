#include "LanguageServerLogView.h"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "codelite_events.h"
#include "event_notifier.h"

#include <wx/menu.h>

LanguageServerLogView::LanguageServerLogView(wxWindow* parent)
    : LanguageServerLogViewBase(parent)
{
    m_stcLog->SetReadOnly(true);

    m_stcLog->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                // clear the view
                m_stcLog->SetReadOnly(false);
                m_stcLog->ClearAll();
                m_stcLog->SetReadOnly(true);
            },
            wxID_CLEAR);
        m_stcLog->PopupMenu(&menu);
    });
    DoColourChanged();
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &LanguageServerLogView::OnColoursChanged, this);
}

LanguageServerLogView::~LanguageServerLogView()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &LanguageServerLogView::OnColoursChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &LanguageServerLogView::OnWorkspaceClosed, this);
}

void LanguageServerLogView::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    m_stcLog->SetReadOnly(false);
    m_stcLog->ClearAll();
    m_stcLog->SetReadOnly(true);
}

void LanguageServerLogView::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    DoColourChanged();
}

void LanguageServerLogView::DoColourChanged()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    CHECK_PTR_RET(lexer);

    lexer->Apply(m_stcLog);
}