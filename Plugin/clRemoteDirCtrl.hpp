#ifndef CLREMOTEDIRCTRL_HPP
#define CLREMOTEDIRCTRL_HPP
#if USE_SFTP

#include "clThemedTreeCtrl.h"
#include "cl_sftp.h"
#include "cl_ssh.h"
#include "codelite_events.h"
#include "ssh_account_info.h"

#include <wx/panel.h>

class clRemoteDirCtrlItemData;
class WXDLLIMPEXP_SDK clRemoteDirCtrl : public wxPanel
{
    clThemedTreeCtrl* m_treeCtrl = nullptr;
    SSHAccountInfo m_account;

protected:
    enum eDownloadAction {
        kOpenInCodeLite = 0,
        kOpenInExplorer,
    };

protected:
    void OnItemActivated(wxTreeEvent& event);
    void OnItemExpanding(wxTreeEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);

protected:
    clRemoteDirCtrlItemData* GetItemData(const wxTreeItemId& item) const;
    void DoExpandItem(const wxTreeItemId& item);
    wxArrayTreeItemIds GetSelections() const;
    void DoOpenItem(const wxTreeItemId& item, eDownloadAction action);
    void DoCreateFolder(const wxTreeItemId& item, const wxString& name);
    void DoCreateFile(const wxTreeItemId& item, const wxString& name);
    void DoRename(const wxTreeItemId& item);
    void DoDelete(const wxTreeItemId& item);

public:
    clRemoteDirCtrl(wxWindow* parent);
    virtual ~clRemoteDirCtrl();

    // Public API
    clThemedTreeCtrl* GetTree() { return m_treeCtrl; }
    /**
     * @brief Open the tree view for a given path + ssh
     */
    bool Open(const wxString& path, const SSHAccountInfo& account);
    /**
     * @brief close the view and free all resources
     */
    bool Close(bool promptUser);

    /**
     * @brief return true if the tree is "connected" to an account
     * @return
     */
    bool IsConnected() const;

    /**
     * @brief return the selected path
     */
    wxString GetSelectedFolder() const;

    /**
     * @brief return the selected folders
     */
    size_t GetSelectedFolders(wxArrayString& paths) const;

    /**
     * @brief change the new root folder
     */
    bool SetNewRoot(const wxString& remotePath);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEDIR_DIR_CONTEXT_MENU_SHOWING, clContextMenuEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_REMOTEDIR_FILE_CONTEXT_MENU_SHOWING, clContextMenuEvent);
#endif
#endif // CLREMOTEDIRCTRL_HPP
