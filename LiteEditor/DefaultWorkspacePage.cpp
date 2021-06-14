#include "DefaultWorkspacePage.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "globals.h"
#include <imanager.h>
#include "clWorkspaceView.h"
#include <wx/simplebook.h>
#include "event_notifier.h"
#include <algorithm>
#include "SelectDropTargetDlg.h"
#include "cl_config.h"
#include "clSystemSettings.h"
#include <wx/dcbuffer.h>

DefaultWorkspacePage::DefaultWorkspacePage(wxWindow* parent)
    : DefaultWorkspacePageBase(parent)
{
    // Allow the PHP view to accepts folders
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    
    wxColour bg = clSystemSettings::clSystemSettings::GetDefaultPanelColour();
    m_colours.InitFromColour(bg);
    if(clConfig::Get().Read("UseCustomBaseColour", false)) {
        bg = clConfig::Get().Read("BaseColour", bg);
        m_colours.InitFromColour(bg);
    }
    m_staticText523->SetBackgroundColour(m_colours.GetBgColour());
    m_staticText523->SetForegroundColour(m_colours.GetItemTextColour());
    
    
    SetDropTarget(new clFileOrFolderDropTarget(this));
    m_staticText523->SetBackgroundColour(m_colours.GetBgColour());
    m_staticText523->SetForegroundColour(m_colours.GetItemTextColour());
    SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_staticBitmap521->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultWorkspacePage::OnColoursChanged, this);
    Bind(wxEVT_PAINT, &DefaultWorkspacePage::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent& e) { wxUnusedVar(e); });
}

DefaultWorkspacePage::~DefaultWorkspacePage()
{
    Unbind(wxEVT_PAINT, &DefaultWorkspacePage::OnPaint, this);
    Unbind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultWorkspacePage::OnColoursChanged, this);
}

void DefaultWorkspacePage::OnFolderDropped(clCommandEvent& event)
{
    const wxArrayString& folders = event.GetStrings();
    CallAfter(&DefaultWorkspacePage::DoDropFolders, folders);
}

void DefaultWorkspacePage::DoDropFolders(const wxArrayString& folders)
{
    SelectDropTargetDlg dropTargetDlg(EventNotifier::Get()->TopFrame(), folders);
    dropTargetDlg.ShowModal();
}

void DefaultWorkspacePage::OnColoursChanged(clCommandEvent& event)
{
    event.Skip();
    wxColour bg = clSystemSettings::GetDefaultPanelColour();
    m_colours.InitFromColour(bg);
    bool useCustom = clConfig::Get().Read("UseCustomBaseColour", false);
    if(useCustom) {
        bg = clConfig::Get().Read("BaseColour", bg);
        m_colours.InitFromColour(bg);
    }
    m_staticText523->SetBackgroundColour(m_colours.GetBgColour());
    m_staticText523->SetForegroundColour(m_colours.GetItemTextColour());
}

void DefaultWorkspacePage::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBrush(m_colours.GetBgColour());
    dc.SetPen(m_colours.GetBgColour());
    dc.DrawRectangle(GetClientRect());
}
