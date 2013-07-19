#ifndef FILEEXPLORERTAB_H
#define FILEEXPLORERTAB_H

#include "wxcrafter.h"
#include <wx/menu.h>
#include "imanager.h"

class FileExplorerTab : public FileExplorerBase
{
protected:
    wxMenu* m_rclickMenu;
    
public:
    FileExplorerTab(wxWindow* parent);
    virtual ~FileExplorerTab();
    wxGenericDirCtrl *Tree() {
        return m_genericDirCtrl;
    }
    TreeItemInfo GetSelectedItemInfo();
protected:
    virtual void OnContextMenu(wxTreeEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnOpenFile(wxCommandEvent &event);
    virtual void OnOpenFileInTextEditor(wxCommandEvent &event);
    virtual void OnRefreshNode(wxCommandEvent &event);
    virtual void OnDeleteNode(wxCommandEvent &event);
    virtual void OnSearchNode(wxCommandEvent &event);
    virtual void OnTagNode(wxCommandEvent &event);
    virtual void OnKeyDown( wxTreeEvent &event );
    virtual void OnOpenShell(wxCommandEvent &event);
    virtual void OnOpenWidthDefaultApp(wxCommandEvent &e);
    
    void DoOpenItem(const wxString& path);
};
#endif // FILEEXPLORERTAB_H
