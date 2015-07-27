#include "clTreeCtrlPanelDefaultPage.h"
#include "clFileOrFolderDropTarget.h"
#include <wx/menu.h>
#include <wx/app.h>

clTreeCtrlPanelDefaultPage::clTreeCtrlPanelDefaultPage(wxWindow* parent)
    : clTreeCtrlPanelDefaultPageBase(parent)
{
    SetDropTarget(new clFileOrFolderDropTarget(parent));
}

clTreeCtrlPanelDefaultPage::~clTreeCtrlPanelDefaultPage() {}

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
