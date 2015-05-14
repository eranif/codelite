#ifndef CLTREECTRLPANEL_H
#define CLTREECTRLPANEL_H

#include "wxcrafter_plugin.h"
#include "bitmap_loader.h"
#include "cl_command_event.h"
#include "clFileViwerTreeCtrl.h"
#include <imanager.h>

class WXDLLIMPEXP_SDK clTreeCtrlPanel : public clTreeCtrlPanelBase
{
    BitmapLoader m_bmpLoader;

public:
    clTreeCtrlPanel(wxWindow* parent);
    virtual ~clTreeCtrlPanel();

    /**
     * @brief add top level folder
     */
    void AddFolder(const wxString& path);

    /**
     * @brief return an info about the selected items in the tree
     * @return
     */
    TreeItemInfo GetSelectedItemInfo();

    /**
     * @brief return 2 arrays of the selected items
     * @param folders [output]
     * @param files [output]
     */
    void GetSelections(wxArrayString& folders, wxArrayString& files);

protected:
    virtual void OnContextMenu(wxTreeEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);

    void OnCloseFolder(wxCommandEvent& event);

    void OnFolderDropped(clCommandEvent& event);

    // Helpers
    void DoExpandItem(const wxTreeItemId& parent);
    bool IsTopLevelFolder(const wxTreeItemId& item);
    
    clTreeCtrlData* GetItemData(const wxTreeItemId& item);
    wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
    wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);
};
#endif // CLTREECTRLPANEL_H
