#ifndef FSCONFIGPAGE_H
#define FSCONFIGPAGE_H
#include "clFileSystemWorkspaceConfig.hpp"
#include "clFileSystemWorkspaceDlgBase.h"

class FSConfigPage : public FSConfigPageBase
{
    clFileSystemWorkspaceConfig::Ptr_t m_config;
    bool m_enableRemotePage = true;
    bool m_useRemoteBrowsing = false;
    wxString m_sshAccount;

protected:
    virtual void OnBrowseForGDB(wxCommandEvent& event);
    virtual void OnBrowseExec(wxCommandEvent& event);
    virtual void OnBrowseWD(wxCommandEvent& event);
    virtual void OnEditExcludePaths(wxCommandEvent& event);
    virtual void OnEnableRemoteUI(wxUpdateUIEvent& event);
    virtual void OnSSHAccountChoice(wxCommandEvent& event);
    virtual void OnSSHBrowse(wxCommandEvent& event);
    virtual void OnRemoteEnabledUI(wxUpdateUIEvent& event);

protected:
    void DoTargetActivated();
    void DoUpdateSSHAcounts();

public:
    FSConfigPage(wxWindow* parent, clFileSystemWorkspaceConfig::Ptr_t config, bool enableRemotePage = true);
    virtual ~FSConfigPage();
    void Save();
    void SetUseRemoteBrowsing(bool useRemoteBrowsing, const wxString& account)
    {
        this->m_useRemoteBrowsing = useRemoteBrowsing;
        this->m_sshAccount = account;
    }

protected:
    virtual void OnTargetActivated(wxDataViewEvent& event);
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnEditTarget(wxCommandEvent& event);
    virtual void OnEditTargetUI(wxUpdateUIEvent& event);
    virtual void OnNewTarget(wxCommandEvent& event);
};
#endif // FSCONFIGPAGE_H
