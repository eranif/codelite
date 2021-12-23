#ifndef CLFILESYSTEMWORKSPACEDLG_H
#define CLFILESYSTEMWORKSPACEDLG_H
#include "clFileSystemWorkspaceConfig.hpp"
#include "clFileSystemWorkspaceDlgBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clFileSystemWorkspaceDlg : public clFileSystemWorkspaceDlgBase
{
    clFileSystemWorkspaceSettings* m_settings = nullptr;
    bool m_usingGlobalSettings = true;
    bool m_useRemoteBrowsing = false;
    wxString m_sshAccount;

public:
    clFileSystemWorkspaceDlg(wxWindow* parent, clFileSystemWorkspaceSettings* settings = nullptr);
    virtual ~clFileSystemWorkspaceDlg();
    void SetUseRemoteBrowsing(bool useRemoteBrowsing, const wxString& account);
    bool IsUseRemoteBrowsing() const { return m_useRemoteBrowsing; }

protected:
    virtual void OnDeleteConfig(wxCommandEvent& event);
    virtual void OnDeleteConfigUI(wxUpdateUIEvent& event);
    virtual void OnNewConfig(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
};
#endif // CLFILESYSTEMWORKSPACEDLG_H
