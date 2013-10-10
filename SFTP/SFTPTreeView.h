#ifndef SFTPTREEVIEW_H
#define SFTPTREEVIEW_H

#include "UI.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"

class MyClientData;
class SFTPTreeView : public SFTPTreeViewBase
{
    clSFTP::Ptr_t m_sftp;
    BitmapLoader  m_bmpLoader;

public:
    SFTPTreeView(wxWindow* parent);
    virtual ~SFTPTreeView();

protected:
    virtual void OnDisconnect(wxCommandEvent& event);
    virtual void OnDisconnectUI(wxUpdateUIEvent& event);
    virtual void OnConnectUI(wxUpdateUIEvent& event);
    virtual void OnConnect(wxCommandEvent& event);
    
    void DoCloseSession();
    void DoExpandItem(const wxTreeListItem& item);
    MyClientData* GetItemData(const wxTreeListItem& item);

protected:
    virtual void OnItemActivated(wxTreeListEvent& event);
    virtual void OnItemExpanding(wxTreeListEvent& event);
    virtual void OnOpenAccountManager(wxCommandEvent& event);
};
#endif // SFTPTREEVIEW_H
