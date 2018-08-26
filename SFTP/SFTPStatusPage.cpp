//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SFTPStatusPage.cpp
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

#include "ColoursAndFontsManager.h"
#include "SFTPStatusPage.h"
#include "event_notifier.h"
#include "lexer_configuration.h"
#include "sftp.h"
#include "sftp_item_comparator.h"
#include "sftp_worker_thread.h"
#include <wx/log.h>
#include <wx/menu.h>

SFTPStatusPage::SFTPStatusPage(wxWindow* parent, SFTP* plugin)
    : SFTPStatusPageBase(parent)
    , m_plugin(plugin)
{
    m_stcOutput->Bind(wxEVT_MENU, &SFTPStatusPage::OnClearLog, this, wxID_CLEAR);
    m_stcOutput->Bind(wxEVT_MENU, &SFTPStatusPage::OnCopy, this, wxID_COPY);
    m_stcOutput->Bind(wxEVT_MENU, &SFTPStatusPage::OnSelectAll, this, wxID_SELECTALL);
    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &SFTPStatusPage::OnThemeChanged, this);
    m_stcOutput->SetReadOnly(true);
}

SFTPStatusPage::~SFTPStatusPage()
{
    m_stcOutput->Unbind(wxEVT_MENU, &SFTPStatusPage::OnClearLog, this, wxID_CLEAR);
    m_stcOutput->Unbind(wxEVT_MENU, &SFTPStatusPage::OnCopy, this, wxID_COPY);
    m_stcOutput->Unbind(wxEVT_MENU, &SFTPStatusPage::OnSelectAll, this, wxID_SELECTALL);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &SFTPStatusPage::OnThemeChanged, this);
}

void SFTPStatusPage::OnContentMenu(wxContextMenuEvent& event)
{
    wxUnusedVar(event);
    ShowContextMenu();
}

void SFTPStatusPage::AddLine(SFTPThreadMessage* message)
{
    wxString msg;
    msg << "[ " << wxDateTime::Now().FormatISOTime() << " ]";
    wxBitmap bmp;
    switch(message->GetStatus()) {
    case SFTPThreadMessage::STATUS_ERROR:
        msg << " [ERR]";
        break;
    case SFTPThreadMessage::STATUS_OK:
        msg << " [OK ]";
        break;
    default:
        msg << " [INF]";
        break;
    }

    msg << " [" << message->GetAccount() << "] " << message->GetMessage();
    wxDELETE(message);
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->AppendText(msg + "\n");
    m_stcOutput->SetReadOnly(true);
    m_stcOutput->ScrollToEnd();
}

void SFTPStatusPage::ShowContextMenu()
{
    wxMenu menu;
    menu.Append(wxID_COPY);
    menu.Append(wxID_SELECTALL);
    menu.AppendSeparator();
    menu.Append(wxID_CLEAR);
    menu.Enable(wxID_CLEAR, !m_stcOutput->IsEmpty());
    m_stcOutput->PopupMenu(&menu);
}

void SFTPStatusPage::OnClearLog(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->ClearAll();
    m_stcOutput->SetReadOnly(true);
}

void SFTPStatusPage::SetStatusBarMessage(const wxString& message) { m_plugin->GetManager()->SetStatusMessage(message); }

void SFTPStatusPage::OnThemeChanged(wxCommandEvent& event)
{
    event.Skip();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_stcOutput); }
}

void SFTPStatusPage::OnCopy(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if(m_stcOutput->CanCopy()) { m_stcOutput->Copy(); }
}

void SFTPStatusPage::OnSelectAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_stcOutput->SelectAll();
}
