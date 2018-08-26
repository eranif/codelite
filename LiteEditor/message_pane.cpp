//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : message_pane.cpp
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

#include "cl_config.h"
#include "editor_config.h"
#include "message_pane.h"
#include "pluginmanager.h"
#include <wx/dcbuffer.h>
#include <wx/msgdlg.h>

MessagePane::MessagePane(wxWindow* parent)
    : MessagePaneBase(parent)
{
    BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();
    m_bitmap1->SetBitmap(bmpLoader->LoadBitmap(wxT("messages/48/info")));
    m_buttonAction->Hide();
    Hide();
}

void MessagePane::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_ESCAPE) {
        DoHide();

    } else
        event.Skip();
}

void MessagePane::OnButtonClose(wxCommandEvent& event)
{
    wxUnusedVar(event);
    MessageDetails msg = m_messages.CurrentMessage();
    SavePreferenceIfNeeded(msg, wxNOT_FOUND); // -1 for Hide
    DoShowNextMessage();
}

void MessagePane::DoHide()
{
    if(IsShown()) {
        // Hide all default controls
        m_buttonAction->Hide();
        m_buttonAction1->Hide();
        m_buttonAction2->Hide();
        m_DontAnnoyMeCheck->Hide();

        m_messages.Clear();
        Hide();
        GetParent()->GetSizer()->Layout();
    }
}

void MessagePane::DoShowCurrentMessage()
{
    MessageDetails msg = m_messages.CurrentMessage();
    wxString txt = msg.message;

    m_buttonAction->Hide();
    m_buttonAction1->Hide();
    m_buttonAction2->Hide();
    m_DontAnnoyMeCheck->Hide();

    bool hasDefaultButton(false);
    if(msg.bmp.IsOk() == false) {
        BitmapLoader* bmpLoader = PluginManager::Get()->GetStdIcons();
        m_bitmap1->SetBitmap(bmpLoader->LoadBitmap(wxT("messages/48/info")));

    } else {
        m_bitmap1->SetBitmap(msg.bmp);
    }

    // display the buttons
    if(msg.btn1.buttonLabel.IsEmpty() == false) {
        m_buttonAction->SetLabel(msg.btn1.buttonLabel);
        m_buttonAction->Show();
        if(msg.btn1.isDefault) {
            m_buttonAction->SetDefault();
            hasDefaultButton = true;
        }
    }

    if(msg.btn2.buttonLabel.IsEmpty() == false) {
        m_buttonAction1->SetLabel(msg.btn2.buttonLabel);
        m_buttonAction1->Show();
        if(msg.btn2.isDefault) {
            m_buttonAction1->SetDefault();
            hasDefaultButton = true;
        }
    }

    if(msg.btn3.buttonLabel.IsEmpty() == false) {
        m_buttonAction2->SetLabel(msg.btn3.buttonLabel);
        m_buttonAction2->Show();
        if(msg.btn3.isDefault) {
            m_buttonAction2->SetDefault();
            hasDefaultButton = true;
        }
    }

    // Show hide button if needed and make the default if there is
    // no default button
    if(msg.showHideButton) {
        m_buttonClose->Show();
        if(!hasDefaultButton) {
            m_buttonClose->SetDefault();
        }
    } else {
        m_buttonClose->Hide();
    }

    m_staticTextMessage->SetLabel(txt);
    if(IsShown() == false) {
        Show();
    }

    // Show the NoSpam checkbox if needed
    m_DontAnnoyMeCheck->Show(msg.check.GetShowCheckbox());
    // Uncheck it, otherwise it will remember a stale 'tick' that the user subsequently reverted
    m_DontAnnoyMeCheck->SetValue(0);

    m_staticTextMessage->Fit();
    GetSizer()->Fit(this);
    GetParent()->GetSizer()->Layout();
    GetParent()->Refresh();
}

void MessagePane::ShowMessage(const wxString& message, bool showHideButton, const wxBitmap& bmp,
                              const ButtonDetails& btn1, const ButtonDetails& btn2, const ButtonDetails& btn3,
                              const CheckboxDetails& chkbox)
{
    // Dont duplicate messages...
    if(m_messages.IsExists(message)) return;

    MessageDetails msg;
    msg.message = message;
    msg.btn1 = btn1;
    msg.btn2 = btn2;
    msg.btn3 = btn3;
    msg.check = chkbox;
    msg.bmp = bmp;
    msg.showHideButton = showHideButton;
    m_messages.PushMessage(msg);
    DoShowCurrentMessage();
}

void MessagePane::OnActionButton(wxCommandEvent& event)
{
    MessageDetails msg = m_messages.CurrentMessage();
    SavePreferenceIfNeeded(msg, 0);
    DoPostEvent(msg.btn1);
    DoShowNextMessage();
}

void MessagePane::DoShowNextMessage()
{
    m_messages.PopMessage();
    if(m_messages.IsEmpty()) {
        DoHide();
        return;
    }

    DoShowCurrentMessage();
}

/////////////////////////////////////////////////////////////////////////////
//

bool MessagePaneData::IsExists(const wxString& msg)
{
    for(size_t i = 0; i < m_queue.size(); i++) {
        if(msg == m_queue.at(i).message) return true;
    }
    return false;
}

void MessagePaneData::Clear() { m_queue.clear(); }

MessageDetails MessagePaneData::CurrentMessage()
{
    if(m_queue.empty()) return MessageDetails();

    return m_queue.back();
}

bool MessagePaneData::IsEmpty() { return m_queue.empty(); }

void MessagePaneData::PopMessage()
{
    if(m_queue.empty()) return;

    m_queue.pop_back();
}

void MessagePaneData::PushMessage(const MessageDetails& msg) { m_queue.push_back(msg); }

void MessagePane::OnEraseBG(wxEraseEvent& event) { wxUnusedVar(event); }

void MessagePane::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    //	wxRect rr = GetClientRect();
    //
    //	dc.SetPen(  wxPen  (wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    //	dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    //	dc.DrawRectangle(rr);
    //
    //	rr.Deflate(1);
    //	dc.SetPen(wxPen( *wxRED_PEN ));
    //	dc.SetBrush( *wxTRANSPARENT_BRUSH );
    //
    //	dc.DrawRectangle(rr);
}

void MessagePane::OnActionButton1(wxCommandEvent& event)
{
    MessageDetails msg = m_messages.CurrentMessage();
    SavePreferenceIfNeeded(msg, 1);
    DoPostEvent(msg.btn2);
    DoShowNextMessage();
}

void MessagePane::OnActionButton2(wxCommandEvent& event)
{
    MessageDetails msg = m_messages.CurrentMessage();
    SavePreferenceIfNeeded(msg, 2);
    DoPostEvent(msg.btn3);
    DoShowNextMessage();
}

void MessagePane::DoPostEvent(ButtonDetails btn)
{
    if(btn.commandId != wxNOT_FOUND && btn.window) {
        if(btn.menuCommand) {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, btn.commandId);
            btn.window->AddPendingEvent(evt);
        } else {
            wxCommandEvent evt(btn.commandId);
            btn.window->AddPendingEvent(evt);
        }
    }
}

void MessagePane::SavePreferenceIfNeeded(const MessageDetails msg, int choice)
{
    // If the checkbox is both shown (to cater for random ticks) and checked, save the preference
    if(choice != wxNOT_FOUND && m_DontAnnoyMeCheck->IsShown() && m_DontAnnoyMeCheck->IsChecked()) {
        wxString label = msg.check.GetLabel();
        if(!label.IsEmpty()) {
            clConfig::Get().SetAnnoyingDlgAnswer(label, choice + 1); // +1 to skip the Hide button
        }
    }

    if(choice == wxNOT_FOUND && m_DontAnnoyMeCheck->IsShown()) {
        wxString label = msg.check.GetLabel();
        if(!label.IsEmpty()) {
            clConfig::Get().SetAnnoyingDlgAnswer(label, m_DontAnnoyMeCheck->IsChecked() ? 1 : 0);
        }
    }
}
