#ifndef SFTPTREEVIEW_H
#define SFTPTREEVIEW_H

#include "UI.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"

class SFTPTreeView : public SFTPTreeViewBase
{
    clSFTP::Ptr_t m_sftp;
    BitmapLoader  m_bmpLoader;
    
public:
    SFTPTreeView(wxWindow* parent);
    virtual ~SFTPTreeView();

protected:
    virtual void OnConnectUI(wxUpdateUIEvent& event);
    void DoCloseSession();
    void DoExpandItem(const wxTreeListItem& item);

protected:
    virtual void OnConnect(wxCommandEvent& event);
    virtual void OnItemActivated(wxTreeListEvent& event);
    virtual void OnItemExpanding(wxTreeListEvent& event);
    virtual void OnOpenAccountManager(wxCommandEvent& event);
};
#endif // SFTPTREEVIEW_H
