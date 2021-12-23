#include "clTreeCtrlPanelDefaultPage.h"

#include "clFileOrFolderDropTarget.h"
#include "clSystemSettings.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/menu.h>
#include <wx/settings.h>

clTreeCtrlPanelDefaultPage::clTreeCtrlPanelDefaultPage(wxWindow* parent)
    : clTreeCtrlPanelDefaultPageBase(parent)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetDropTarget(new clFileOrFolderDropTarget(parent));

    clCommandEvent dummy;
    OnColoursChanged(dummy);

    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clTreeCtrlPanelDefaultPage::OnColoursChanged, this);
    Bind(wxEVT_PAINT, &clTreeCtrlPanelDefaultPage::OnPaint, this);
}

clTreeCtrlPanelDefaultPage::~clTreeCtrlPanelDefaultPage()
{
    Unbind(wxEVT_PAINT, &clTreeCtrlPanelDefaultPage::OnPaint, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clTreeCtrlPanelDefaultPage::OnColoursChanged, this);
}

void clTreeCtrlPanelDefaultPage::OnDefaultPageContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(XRCID("open_folder_default_page"), _("Open Folder..."));
    menu.Bind(wxEVT_MENU, &clTreeCtrlPanelDefaultPage::OnOpenFolder, this, XRCID("open_folder_default_page"));
    PopupMenu(&menu);
}

void clTreeCtrlPanelDefaultPage::OnOpenFolder(wxCommandEvent& event)
{
    wxCommandEvent eventOpenFolder(wxEVT_MENU, XRCID("open_folder"));
    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(eventOpenFolder);
}

void clTreeCtrlPanelDefaultPage::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBrush(clSystemSettings::GetDefaultPanelColour());
    dc.SetPen(clSystemSettings::GetDefaultPanelColour());
    dc.DrawRectangle(GetClientRect());
}

void clTreeCtrlPanelDefaultPage::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    m_staticText177->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    m_staticText177->SetForegroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    GetPanel169()->SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
}
