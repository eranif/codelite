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

DefaultWorkspacePage::DefaultWorkspacePage(wxWindow* parent)
    : DefaultWorkspacePageBase(parent)
{
    // Allow the PHP view to accepts folders
    SetDropTarget(new clFileOrFolderDropTarget(this));
    m_staticText523->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    m_staticBitmap521->SetDropTarget(new clFileOrFolderDropTarget(this));
    Bind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
}

DefaultWorkspacePage::~DefaultWorkspacePage()
{
    Unbind(wxEVT_DND_FOLDER_DROPPED, &DefaultWorkspacePage::OnFolderDropped, this);
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
