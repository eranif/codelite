#ifndef FSCONFIGPAGE_H
#define FSCONFIGPAGE_H
#include "clFileSystemWorkspaceDlgBase.h"
#include "clFileSystemWorkspaceConfig.hpp"

class FSConfigPage : public FSConfigPageBase
{
    clFileSystemWorkspaceConfig::Ptr_t m_config;

protected:
    virtual void OnEnableRemoteUI(wxUpdateUIEvent& event);
    virtual void OnSSHAccountChoice(wxCommandEvent& event);
    virtual void OnSSHBrowse(wxCommandEvent& event);
    virtual void OnRemoteEnabledUI(wxUpdateUIEvent& event);

protected:
    void DoTargetActivated();
    void DoUpdateSSHAcounts();

public:
    FSConfigPage(wxWindow* parent, clFileSystemWorkspaceConfig::Ptr_t config);
    virtual ~FSConfigPage();
    void Save();

protected:
    virtual void OnTargetActivated(wxDataViewEvent& event);
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
    virtual void OnEditTarget(wxCommandEvent& event);
    virtual void OnEditTargetUI(wxUpdateUIEvent& event);
    virtual void OnNewTarget(wxCommandEvent& event);
};
#endif // FSCONFIGPAGE_H
