#ifndef SFTPBROWSERDLG_H
#define SFTPBROWSERDLG_H

#include "sftp_ui.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"

class SFTPBrowserEntryClientData;
class FloatingTextCtrl;

class SFTPBrowserDlg : public SFTPBrowserBaseDlg
{
    clSFTP::Ptr_t m_sftp;
    BitmapLoader::BitmapMap_t m_bitmaps;
    wxString m_filter;

protected:
    virtual void OnEnter(wxCommandEvent& event);
    virtual void OnFocusLost(wxFocusEvent& event);
    virtual void OnTextUpdated(wxCommandEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
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

    void Initialize(const wxString &account, const wxString &path);
    wxString GetPath() const;
    wxString GetAccount() const;

    void OnInlineSearch();
    void OnInlineSearchEnter();

protected:
    virtual void OnRefresh(wxCommandEvent& event);
    virtual void OnRefreshUI(wxUpdateUIEvent& event);
};
#endif // SFTPBROWSERDLG_H
