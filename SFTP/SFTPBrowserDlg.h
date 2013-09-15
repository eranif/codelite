#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#include "sftp_ui.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"

class SFTPBrowserEntryClientData;
class SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
    clSFTP::Ptr_t m_sftp;
    BitmapLoader::BitmapMap_t m_bitmaps;
    wxString m_filter;

protected:
    virtual void OnItemSelected(wxDataViewEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    void DoCloseSession();
    void DoDisplayEntriesForSelectedPath();
    SFTPBrowserEntryClientData* DoGetItemData(const wxDataViewItem& item) const;

public:
    SFTPBrowserDlg(wxWindow* parent, const wxString &title, const wxString& filter);
    virtual ~SFTPBrowserDlg();
    wxString GetPath() const;
    wxString GetAccount() const;

protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // SFTPBROWSERDLG_H
