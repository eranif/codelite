#include "LanguageServerLogView.h"

LanguageServerLogView::LanguageServerLogView(wxWindow* parent)
    : LanguageServerLogViewBase(parent)
{
    m_dvListCtrl->Bind(wxEVT_CONTEXT_MENU, [this](wxContextMenuEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                // clear the view
                m_dvListCtrl->DeleteAllItems();
            },
            wxID_CLEAR);
        m_dvListCtrl->PopupMenu(&menu);
    });
}

LanguageServerLogView::~LanguageServerLogView() {}
