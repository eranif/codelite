#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#include "sftp_ui.h"
#include "cl_ssh.h"

class SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
    clSSH* m_ssh;
    
protected:
    void DoCloseSession();
    
public:
    SFTPBrowserDlg(wxWindow* parent);
    virtual ~SFTPBrowserDlg();
    
protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // SFTPBROWSERDLG_H
