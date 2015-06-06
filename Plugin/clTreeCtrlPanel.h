#ifndef CLTREECTRLPANEL_H
#define CLTREECTRLPANEL_H

#include "wxcrafter_plugin.h"
#include "bitmap_loader.h"
#include "cl_command_event.h"
#include "clFileViwerTreeCtrl.h"
#include <imanager.h>
#include "cl_config.h"

class WXDLLIMPEXP_SDK clTreeCtrlPanel : public clTreeCtrlPanelBase
{
    BitmapLoader m_bmpLoader;
    clConfig* m_config;

public:
    clTreeCtrlPanel(wxWindow* parent);
    virtual ~clTreeCtrlPanel();

    clConfig* GetConfig() { return m_config; }
    /**
     * @brief return the configuration tool used for storing information about
     * this tree. Override it to provide a custom configuration tool
     */
    void SetConfig(clConfig* config) { this->m_config = config; }
    
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

    /**
     * @brief select a given filename in the tree. Expand the tree if needed
     */
    bool ExpandToFile(const wxFileName& filename);

protected:
    virtual void OnContextMenu(wxTreeEvent& event);
    virtual void OnItemActivated(wxTreeEvent& event);
    virtual void OnItemExpanding(wxTreeEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);

    void UpdateItemDeleted(const wxTreeItemId& item);
    void GetTopLevelFolders(wxArrayString& paths, wxArrayTreeItemIds& items);

    /**
     * @brief ensure that item is selected (single selection)
     */
    void SelectItem(const wxTreeItemId& item);
    /**
     * @brief return list of selected files and folders. In addition return the
     * tree ctrl items. You can always assume that the folders and the folderItems are of the same
     * size. Same for the file arrays
     */
    void GetSelections(wxArrayString& folders,
                       wxArrayTreeItemIds& folderItems,
                       wxArrayString& files,
                       wxArrayTreeItemIds& fileItems);

    void OnCloseFolder(wxCommandEvent& event);
    void OnNewFolder(wxCommandEvent& event);
    void OnNewFile(wxCommandEvent& event);
    void OnDeleteFolder(wxCommandEvent& event);
    void OnOpenFile(wxCommandEvent& event);
    void OnRenameFile(wxCommandEvent& event);
    void OnDeleteFile(wxCommandEvent& event);

    void OnFindInFilesFolder(wxCommandEvent& event);
    void OnOpenContainingFolder(wxCommandEvent& event);
    void OnOpenShellFolder(wxCommandEvent& event);
    void OnPinFolder(wxCommandEvent& event);

    void OnFolderDropped(clCommandEvent& event);

    // Helpers
    void DoExpandItem(const wxTreeItemId& parent, bool expand);
    void DoRenameItem(const wxTreeItemId& item, const wxString& oldname, const wxString& newname);

    bool IsTopLevelFolder(const wxTreeItemId& item);

    clTreeCtrlData* GetItemData(const wxTreeItemId& item);
    wxTreeItemId DoAddFolder(const wxTreeItemId& parent, const wxString& path);
    wxTreeItemId DoAddFile(const wxTreeItemId& parent, const wxString& path);
};
#endif // CLTREECTRLPANEL_H
