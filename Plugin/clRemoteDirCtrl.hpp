#ifndef CLREMOTEDIRCTRL_HPP
#define CLREMOTEDIRCTRL_HPP
#if USE_SFTP

#include "clThemedTreeCtrl.h"
#include "cl_sftp.h"
#include "cl_ssh.h"
#include "codelite_events.h"
#include "ssh_account_info.h"
#include <wx/panel.h>

using namespace std;
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
    clRemoteDirCtrlItemData* GetItemData(const wxTreeItemId& item);
    void DoExpandItem(const wxTreeItemId& item);
    wxArrayTreeItemIds GetSelections() const;
    void DoOpenItem(const wxTreeItemId& item, eDownloadAction action);
    void DoCreateFolder(const wxTreeItemId& item, const wxString& name);
    void DoCreateFile(const wxTreeItemId& item, const wxString& name);
    void DoGrepFolder(const wxTreeItemId& item);
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
    void Open(const wxString& path, const SSHAccountInfo& account);
    /**
     * @brief close the view and free all resources
     */
    void Close();
};

#endif
#endif // CLREMOTEDIRCTRL_HPP
