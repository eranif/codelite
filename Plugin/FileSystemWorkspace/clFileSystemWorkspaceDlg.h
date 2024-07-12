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
    void OnDeleteConfig(wxCommandEvent& event) override;
    void OnDeleteConfigUI(wxUpdateUIEvent& event) override;
    void OnNewConfig(wxCommandEvent& event) override;
    void OnOK(wxCommandEvent& event) override;
    void SelectConfig(const wxString& config);
};
#endif // CLFILESYSTEMWORKSPACEDLG_H
