#include "DefaultWorkspacePage.h"

#include "SelectDropTargetDlg.h"
#include "clFileOrFolderDropTarget.h"
#include "clSystemSettings.h"
#include "clWorkspaceView.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

#include <algorithm>
#include <imanager.h>
#include <wx/dcbuffer.h>
#include <wx/simplebook.h>

DefaultWorkspacePage::DefaultWorkspacePage(wxWindow* parent)
    : DefaultWorkspacePageBase(parent)
{
    wxColour bg = clSystemSettings::clSystemSettings::GetDefaultPanelColour();
    m_colours.InitFromColour(bg);
    if(clConfig::Get().Read("UseCustomBaseColour", false)) {
        bg = clConfig::Get().Read("BaseColour", bg);
        m_colours.InitFromColour(bg);
    }

    SetDropTarget(new clFileOrFolderDropTarget(this));
    SetBackgroundColour(clSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_staticBitmap521->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultWorkspacePage::OnColoursChanged, this);
}

DefaultWorkspacePage::~DefaultWorkspacePage()
{
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
    m_colours.InitDefaults();
}
