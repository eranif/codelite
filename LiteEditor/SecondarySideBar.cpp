#include "SecondarySideBar.hpp"

#include "SideBar.hpp"
#include "cl_config.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"

#define VIEW_NAME "Secondary Sidebar"

SecondarySideBar::SecondarySideBar(wxWindow* parent, long style)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    m_book = new clSideBarCtrl(this);
    GetSizer()->Add(m_book, wxSizerFlags().Expand().Proportion(1));

    auto direction = EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection();
    m_book->SetButtonPosition(direction);

    GetSizer()->Fit(this);
    m_book->Bind(wxEVT_SIDEBAR_CONTEXT_MENU, &SecondarySideBar::OnContextMenu, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &SecondarySideBar::OnSettingsChanged, this);
}

SecondarySideBar::~SecondarySideBar()
{
    wxArrayString tabs;
    tabs.reserve(m_book->GetPageCount());
    for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
        tabs.Add(m_book->GetPageText(i));
    }

    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &SecondarySideBar::OnSettingsChanged, this);

    // override the values
    clConfig::Get().Write("secondary_side_bar.tabs", tabs);
    clConfig::Get().Write("secondary_side_bar.selection", m_book->GetSelection());
}

void SecondarySideBar::SetSideBar(SideBar* sb) { m_sidebar = sb; }

void SecondarySideBar::AddPage(wxWindow* win, wxBitmap bmp, const wxString& label)
{
    if(m_book->GetPageCount() == 0) {
        clGetManager()->ShowPane(VIEW_NAME, true);
    }

    m_book->AddPage(win, label, bmp, true);
    m_book->GetSizer()->Layout();

    auto secondary_tabs = clConfig::Get().Read("secondary_side_bar.tabs", wxArrayString{});
    if(secondary_tabs.Index(label) == wxNOT_FOUND) {
        secondary_tabs.Add(label);
        clConfig::Get().Write("secondary_side_bar.tabs", secondary_tabs);
    }
}

void SecondarySideBar::OnContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("sidebar-move-to-sidebar"), _("Move to primary sidebar"));

    int pos = event.GetSelection();
    menu.Bind(
        wxEVT_MENU,
        [pos, this](wxCommandEvent& e) {
            wxUnusedVar(e);
            MoveToPrimarySideBar(pos);
        },
        XRCID("sidebar-move-to-sidebar"));
    m_book->PopupMenu(&menu);
}

void SecondarySideBar::MoveToPrimarySideBar(int pos)
{
    wxString label = m_book->GetPageText(pos);
    wxBitmap bmp = m_book->GetPageBitmap(pos);
    wxWindow* win = m_book->GetPage(pos);

    m_book->RemovePage(pos);
    m_book->GetSizer()->Layout();

    // add it to the right side bar
    m_sidebar->AddPage(win, bmp, label);

    if(m_book->GetPageCount() == 0) {
        // No more pages -> hide this view
        clGetManager()->ShowPane(VIEW_NAME, false);
    }
}

void SecondarySideBar::SetSelection(int selection)
{
    if((size_t)selection >= m_book->GetPageCount()) {
        return;
    }
    m_book->SetSelection(selection);
}

void SecondarySideBar::OnSettingsChanged(wxCommandEvent& event)
{
    event.Skip();

    auto direction = EditorConfigST::Get()->GetOptions()->GetWorkspaceTabsDirection();
    // on the secondary bar, "left" means: vertical tabs, i.e. place them on the right side
    if(direction == wxLEFT) {
        direction = wxRIGHT;
    }
    m_book->SetButtonPosition(direction);
}
