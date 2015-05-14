#include "DefaultWorkspacePage.h"
#include "clFileOrFolderDropTarget.h"
#include "codelite_events.h"
#include "globals.h"
#include <imanager.h>
#include "clWorkspaceView.h"
#include <wx/simplebook.h>
#include "event_notifier.h"

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
    std::map<wxString, wxWindow*> pages;
    wxSimplebook* book = clGetManager()->GetWorkspaceView()->GetSimpleBook();
    wxArrayString options;
    int selection = wxNOT_FOUND;
    for(size_t i = 0; i < book->GetPageCount(); ++i) {
        if(book->GetPageText(i) == "Default") continue;

        wxString title;
        title << book->GetPageText(i);
        pages.insert(std::make_pair(title, book->GetPage(i)));
        options.Add(title);
        if(selection == wxNOT_FOUND) {
            selection = options.size() - 1;
        }
    }

    wxString userSelection =
        ::wxGetSingleChoice(_("Choose a View"), _("Open folder"), options, selection, EventNotifier::Get()->TopFrame());

    if(pages.count(userSelection) == 0) return; // user cancelled
    wxWindow* page = pages.find(userSelection)->second;

    // Propogate the event to the correct handler
    clCommandEvent event(wxEVT_DND_FOLDER_DROPPED);
    event.SetStrings(folders);
    page->GetEventHandler()->AddPendingEvent(event);
}
