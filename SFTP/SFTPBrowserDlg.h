#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#include "sftp_ui.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"

class SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
    clSFTP::Ptr_t m_sftp;
    BitmapLoader::BitmapMap_t m_bitmaps;
protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    void DoCloseSession();
    void DoDisplayEntriesForSelectedPath();

public:
    SFTPBrowserDlg(wxWindow* parent);
    virtual ~SFTPBrowserDlg();

protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // SFTPBROWSERDLG_H
