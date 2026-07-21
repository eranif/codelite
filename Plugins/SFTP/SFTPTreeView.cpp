//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPTreeView.cpp
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

#include "SFTPTreeView.h"

#include "SFTPManageBookmarkDlg.h"
#include "SFTPQuickConnectDlg.h"
#include "SFTPSettingsDialog.h"
#include "SFTPStatusPage.h"
#include "SSHAccountManagerDlg.h"
#include "clSystemSettings.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "sftp.h"
#include "sftp_settings.h"
#include "sftp_worker_thread.h"
#include "ssh/ssh_account_info.h"

#include <wx/busyinfo.h>
#include <wx/fdrepdlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/textdlg.h>
#include <wx/utils.h>

SFTPTreeView::SFTPTreeView(wxWindow* parent, SFTP* plugin)
    : SFTPTreeViewBase(parent)
    , m_plugin(plugin)
{
    SetBackgroundColour(clSystemSettings::GetDefaultPanelColour());
    m_view = new clRemoteDirCtrl(this);
    GetSizer()->Add(m_view, 1, wxEXPAND);

    m_timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &SFTPTreeView::OnKeepAliveTimer, this, m_timer->GetId());
    m_timer->Start(30000); // every 30 seconds send a hearbeat

    SFTPSettings settings;
    settings.Load();

    auto images = clGetManager()->GetStdIcons();
    m_auibar->AddTool(XRCID("ID_OPEN_ACCOUNT_MANAGER"),
                      _("Open account manager..."),
                      images->LoadBitmap("user"),
                      _("Open account manager..."));
    m_auibar->AddTool(XRCID("ID_SFTP_CONNECT"),
                      _("Disconnected. Click to connect"),
                      images->LoadBitmap("disconnected"),
                      wxEmptyString,
                      wxITEM_CHECK);
    m_auibar->AddTool(XRCID("ID_SFTP_GOTO"), _("Goto Folder"), images->LoadBitmap("forward"), _("Goto Folder"));
    m_auibar->AddTool(
        XRCID("ID_SSH_OPEN_TERMINAL"), _("Open Terminal"), images->LoadBitmap("console"), "", wxITEM_DROPDOWN);
    m_auibar->Realize();

    // Bind the toolbar events
    m_auibar->Bind(wxEVT_TOOL, &SFTPTreeView::OnOpenAccountManager, this, XRCID("ID_OPEN_ACCOUNT_MANAGER"));
    m_auibar->Bind(wxEVT_TOOL, &SFTPTreeView::OnConnection, this, XRCID("ID_SFTP_CONNECT"));
    m_auibar->Bind(wxEVT_TOOL, &SFTPTreeView::OnGotoLocation, this, XRCID("ID_SFTP_GOTO"));
    m_auibar->Bind(wxEVT_UPDATE_UI, &SFTPTreeView::OnConnectionUI, this, XRCID("ID_SFTP_CONNECT"));
    m_auibar->Bind(wxEVT_UPDATE_UI, &SFTPTreeView::OnGotoLocationUI, this, XRCID("ID_SFTP_GOTO"));
}

SFTPTreeView::~SFTPTreeView()
{
    m_timer->Stop();
    Unbind(wxEVT_TIMER, &SFTPTreeView::OnKeepAliveTimer, this, m_timer->GetId());
    wxDELETE(m_timer);
}

void SFTPTreeView::OnDisconnect(wxCommandEvent& event) { DoCloseSession(); }
void SFTPTreeView::OnConnect(wxCommandEvent& event) { DoOpenSession(); }

void SFTPTreeView::DoBuildTree(const wxString& initialFolder)
{
    m_view->Close(false);
    m_view->Open(initialFolder, m_account);
}

void SFTPTreeView::OnOpenAccountManager(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SSHAccountManagerDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void SFTPTreeView::DoCloseSession()
{
    // Clear the 'search' view
    m_plugin->GetOutputPane()->ClearSearchOutput();
    m_view->Close(true);
}

void SFTPTreeView::OnDisconnectUI(wxUpdateUIEvent& event) { event.Enable(m_view->IsConnected()); }

void SFTPTreeView::OnGotoLocation(wxCommandEvent& event)
{
    wxString path = ::wxGetTextFromUser(_("Goto Location:"), "CodeLite");
    if (path.empty())
        return;
    DoChangeLocation(path);
}

void SFTPTreeView::OnGotoLocationUI(wxUpdateUIEvent& event) { event.Enable(m_view->IsConnected()); }

void SFTPTreeView::ManageBookmarks()
{
    SFTPManageBookmarkDlg dlg(NULL, m_account.GetBookmarks());
    if (dlg.ShowModal() == wxID_OK) {
        m_account.SetBookmarks(dlg.GetBookmarks());
        SFTPSettings settings;
        settings.Load();
        settings.UpdateAccount(m_account);
        settings.Save();
    }
}

void SFTPTreeView::OnChoiceAccount(wxCommandEvent& event) { event.Skip(); }
void SFTPTreeView::OnChoiceAccountUI(wxUpdateUIEvent& event) { event.Enable(!m_view->IsConnected()); }

void SFTPTreeView::OnConnectionUI(wxUpdateUIEvent& event) { event.Check(m_view->IsConnected()); }

void SFTPTreeView::OnConnection(wxCommandEvent& event)
{
    if (m_view->IsConnected()) {
        // Disconnect
        DoCloseSession();
    } else {
        DoOpenSession();
    }
}

void SFTPTreeView::DoOpenSession()
{
    DoCloseSession();
    if (!GetAccountFromUser(m_account)) {
        return;
    }
    DoBuildTree(m_account.GetDefaultFolder().IsEmpty() ? "/" : m_account.GetDefaultFolder());
    // CallAfter(&SFTPTreeView::DoLoadSession);
}

void SFTPTreeView::OnSftpSettings(wxCommandEvent& event)
{
    // Show the SFTP settings dialog
    SFTPSettingsDialog dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

bool SFTPTreeView::GetAccountFromUser(SSHAccountInfo& account)
{
    SFTPQuickConnectDlg connectDialog(EventNotifier::Get()->TopFrame());
    if (connectDialog.ShowModal() != wxID_OK) {
        return false;
    }

    // Get the selected account
    account = connectDialog.GetSelectedAccount();
    return true;
}

void SFTPTreeView::OnKeepAliveTimer(wxTimerEvent& event) { event.Skip(); }
void SFTPTreeView::DoChangeLocation(const wxString& path) { m_view->SetNewRoot(path); }
