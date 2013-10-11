//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : SFTPTreeView.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef SFTPTREEVIEW_H
#define SFTPTREEVIEW_H

#include "UI.h"
#include "cl_sftp.h"
#include "bitmap_loader.h"
#include "ssh_account_info.h"

class MyClientData;
class SFTP;
class SFTPTreeView : public SFTPTreeViewBase
{
    clSFTP::Ptr_t  m_sftp;
    BitmapLoader   m_bmpLoader;
    SSHAccountInfo m_account;
    SFTP*          m_plugin;
    
public:
    SFTPTreeView(wxWindow* parent, SFTP* plugin);
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
