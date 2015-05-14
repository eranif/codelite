#include "DefaultWorkspacePage.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "globals.h"
#include <imanager.h>
#include "clWorkspaceView.h"
#include <wx/simplebook.h>
#include "event_notifier.h"
#include <algorithm>

DefaultWorkspacePage::DefaultWorkspacePage(wxWindow* parent)
    : DefaultWorkspacePageBase(parent)
{
    // Allow the PHP view to accepts folders
    SetDropTarget(new clFileOrFolderDropTarget(this));
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
    wxArrayString options;
    std::map<wxString, wxWindow*> pages = clGetManager()->GetWorkspaceView()->GetAllPages();
    std::for_each(pages.begin(), pages.end(), [&](const std::pair<wxString, wxWindow*>& p){
        options.Add(p.first);
    });
    
    wxString userSelection =
        ::wxGetSingleChoice(_("Choose a View"), _("Open folder"), options, 0, EventNotifier::Get()->TopFrame());

    if(pages.count(userSelection) == 0) return; // user cancelled
    wxWindow* page = pages.find(userSelection)->second;

    // Propogate the event to the correct handler
    clCommandEvent event(wxEVT_DND_FOLDER_DROPPED);
    event.SetStrings(folders);
    page->GetEventHandler()->AddPendingEvent(event);
}
