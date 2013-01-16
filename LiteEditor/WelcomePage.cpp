#include "WelcomePage.h"
#include "manager.h"
#include "frame.h"
#include <wx/arrstr.h>
#include "bitmap_loader.h"
#include "fileextmanager.h"
#include "window_locker.h"
#include <wx/clntdata.h>

WelcomePage::WelcomePage(wxWindow* parent)
    : WelcomePageBase(parent)
{
}

WelcomePage::~WelcomePage()
{
}

void WelcomePage::OnNewProject(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("new_project"));
    e.SetEventObject( clMainFrame::Get() );

    clMainFrame::Get()->GetEventHandler()->AddPendingEvent( e );
}

void WelcomePage::OnOpenForums(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("http://forums.codelite.org/");
}

void WelcomePage::OnOpenWiki(wxCommandEvent& event)
{
    wxUnusedVar(event);
    ::wxLaunchDefaultBrowser("http://codelite.org/LiteEditor/Documentation");
}

void WelcomePage::OnSize(wxSizeEvent& event)
{
    event.Skip();
    m_staticBitmap161->Refresh();
}

void WelcomePage::OnOpenWorkspace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
    e.SetEventObject( clMainFrame::Get() );

    clMainFrame::Get()->GetEventHandler()->AddPendingEvent( e );
}

void WelcomePage::OnShowFileseMenu(wxCommandEvent& event)
{
    wxArrayString recentFiles;
    clMainFrame::Get()->GetMainBook()->GetRecentlyOpenedFiles(recentFiles);
    recentFiles.Sort();
    
    int id = DoGetPopupMenuSelection(m_cmdLnkBtnFilesMenu, recentFiles, _("Open file"));
    if ( id != wxID_NONE ) {
        wxString filename = m_idToName[id];
        clMainFrame::Get()->GetMainBook()->OpenFile( filename );
    }
}

void WelcomePage::OnShowWorkspaceMenu(wxCommandEvent& event)
{
    wxArrayString files;
    ManagerST::Get()->GetRecentlyOpenedWorkspaces(files);
    files.Sort();
    
    wxArrayString recentWorkspaces;
    for(size_t i=0; i<files.GetCount(); i++) {
        wxFileName fn(files.Item(i));
        recentWorkspaces.Add( fn.GetName() + " - " + fn.GetFullPath() );
    }

    int id = DoGetPopupMenuSelection(m_cmdLnkBtnWorkspaces, recentWorkspaces, _("Open workspace"));
    if ( id != wxID_NONE ) {
        wxString filename = m_idToName[id];
        filename = filename.AfterFirst('-');
        filename.Trim().Trim(false);
        clWindowUpdateLocker locker(clMainFrame::Get());
        ManagerST::Get()->OpenWorkspace( filename );
    }
}

int WelcomePage::DoGetPopupMenuSelection(wxCommandLinkButton* btn, const wxArrayString& strings, const wxString &menuTitle)
{
    BitmapLoader bl;
    BitmapLoader::BitmapMap_t bmps = bl.MakeStandardMimeMap();
    
    m_idToName.clear();
    wxMenu menu( menuTitle );
    
    for(size_t i=0; i<strings.GetCount(); i++) {
        
        wxBitmap bmp = bmps[FileExtManager::TypeText];
        FileExtManager::FileType type = FileExtManager::GetType( strings.Item(i) );
        if( bmps.count(type) ) {
            bmp = bmps[type];
        }

        wxMenuItem* item = new wxMenuItem(&menu, wxID_ANY, strings.Item(i));
        item->SetBitmap(bmp);
        m_idToName.insert(std::make_pair(item->GetId(), strings.Item(i)));
        menu.Append(item);
    }
    
    // get the best position to show the menu
    wxPoint pos = btn->GetPosition();
    pos.y += btn->GetSize().y;
    pos.y += 5;
    pos.x += 5;
    return GetPopupMenuSelectionFromUser(menu, pos);
}
