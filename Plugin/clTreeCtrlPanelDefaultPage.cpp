#include "clFileOrFolderDropTarget.h"
#include "clTreeCtrlPanelDefaultPage.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include "event_notifier.h"
#include "codelite_events.h"
#include "clSystemSettings.h"
#include <wx/dcbuffer.h>
#include "cl_config.h"

clTreeCtrlPanelDefaultPage::clTreeCtrlPanelDefaultPage(wxWindow* parent)
    : clTreeCtrlPanelDefaultPageBase(parent)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    wxColour bg = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_colours.InitFromColour(bg);
    if(clConfig::Get().Read("UseCustomBaseColour", false)) {
        bg = clConfig::Get().Read("BaseColour", bg);
        m_colours.InitFromColour(bg);
    }
    
    SetDropTarget(new clFileOrFolderDropTarget(parent));
    m_staticText177->SetBackgroundColour(m_colours.GetBgColour());
    m_staticText177->SetForegroundColour(m_colours.GetItemTextColour());
    GetPanel169()->SetBackgroundColour(m_colours.GetBgColour());
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clTreeCtrlPanelDefaultPage::OnColoursChanged, this);
    Bind(wxEVT_PAINT, &clTreeCtrlPanelDefaultPage::OnPaint, this);
}

clTreeCtrlPanelDefaultPage::~clTreeCtrlPanelDefaultPage()
{
    Unbind(wxEVT_PAINT, &clTreeCtrlPanelDefaultPage::OnPaint, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &clTreeCtrlPanelDefaultPage::OnColoursChanged, this);
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
    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(GetClientRect());
}

void clTreeCtrlPanelDefaultPage::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    wxColour bg = clSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    m_colours.InitFromColour(bg);
    bool useCustom = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustom) {
        bg = clConfig::Get().Read("BaseColour", bg);
        m_colours.InitFromColour(bg);
    }
    m_staticText177->SetForegroundColour(m_colours.GetBgColour());
    GetPanel169()->SetBackgroundColour(m_colours.GetBgColour());
    m_staticText177->SetForegroundColour(m_colours.GetItemTextColour());
}
