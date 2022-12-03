//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newkeyshortcutdlg.cpp
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
#include "newkeyshortcutdlg.h"

#include "clSingleChoiceDialog.h"
#include "globals.h"
#include "windowattrmanager.h"

static const struct wxKeyName {
    wxKeyCode code;
    const char* name;
} wxKeyNames[] = {
    { WXK_DELETE, wxTRANSLATE("DEL") },
    { WXK_DELETE, wxTRANSLATE("DELETE") },
    { WXK_BACK, wxTRANSLATE("BACK") },
    { WXK_INSERT, wxTRANSLATE("INS") },
    { WXK_RETURN, wxTRANSLATE("ENTER") },
    { WXK_RETURN, wxTRANSLATE("RETURN") },
    { WXK_PAGEUP, wxTRANSLATE("PGUP") },
    { WXK_PAGEDOWN, wxTRANSLATE("PGDN") },
    { WXK_LEFT, wxTRANSLATE("LEFT") },
    { WXK_RIGHT, wxTRANSLATE("RIGHT") },
    { WXK_UP, wxTRANSLATE("UP") },
    { WXK_DOWN, wxTRANSLATE("DOWN") },
    { WXK_HOME, wxTRANSLATE("HOME") },
    { WXK_END, wxTRANSLATE("END") },
    { WXK_SPACE, wxTRANSLATE("SPACE") },
    { WXK_TAB, wxTRANSLATE("TAB") },
    { WXK_ESCAPE, wxTRANSLATE("ESC") },
    { WXK_ESCAPE, wxTRANSLATE("ESCAPE") },
    { WXK_CANCEL, wxTRANSLATE("CANCEL") },
    { WXK_CLEAR, wxTRANSLATE("CLEAR") },
    { WXK_MENU, wxTRANSLATE("MENU") },
    { WXK_PAUSE, wxTRANSLATE("PAUSE") },
    { WXK_CAPITAL, wxTRANSLATE("CAPITAL") },
    { WXK_SELECT, wxTRANSLATE("SELECT") },
    { WXK_PRINT, wxTRANSLATE("PRINT") },
    { WXK_EXECUTE, wxTRANSLATE("EXECUTE") },
    { WXK_SNAPSHOT, wxTRANSLATE("SNAPSHOT") },
    { WXK_HELP, wxTRANSLATE("HELP") },
    { WXK_ADD, wxTRANSLATE("ADD") },
    { WXK_SEPARATOR, wxTRANSLATE("SEPARATOR") },
    { WXK_SUBTRACT, wxTRANSLATE("SUBTRACT") },
    { WXK_DECIMAL, wxTRANSLATE("DECIMAL") },
    { WXK_DIVIDE, wxTRANSLATE("DIVIDE") },
    { WXK_NUMLOCK, wxTRANSLATE("NUM_LOCK") },
    { WXK_SCROLL, wxTRANSLATE("SCROLL_LOCK") },
    { WXK_PAGEUP, wxTRANSLATE("PAGEUP") },
    { WXK_PAGEDOWN, wxTRANSLATE("PAGEDOWN") },
    { WXK_NUMPAD_SPACE, wxTRANSLATE("SPACE") },
    { WXK_NUMPAD_TAB, wxTRANSLATE("TAB") },
    { WXK_NUMPAD_ENTER, wxTRANSLATE("ENTER") },
    { WXK_NUMPAD_HOME, wxTRANSLATE("HOME") },
    { WXK_NUMPAD_LEFT, wxTRANSLATE("LEFT") },
    { WXK_NUMPAD_UP, wxTRANSLATE("UP") },
    { WXK_NUMPAD_RIGHT, wxTRANSLATE("RIGHT") },
    { WXK_NUMPAD_DOWN, wxTRANSLATE("DOWN") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("PRIOR") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("PGUP") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("NEXT") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("PGDN") },
    { WXK_NUMPAD_END, wxTRANSLATE("END") },
    { WXK_NUMPAD_BEGIN, wxTRANSLATE("BEGIN") },
    { WXK_NUMPAD_INSERT, wxTRANSLATE("INS") },
    { WXK_NUMPAD_DELETE, wxTRANSLATE("DEL") },
    { WXK_NUMPAD_EQUAL, wxTRANSLATE("=") },
    { WXK_NUMPAD_MULTIPLY, wxTRANSLATE("*") },
    { WXK_NUMPAD_ADD, wxTRANSLATE("+") },
    { WXK_NUMPAD_SEPARATOR, wxTRANSLATE("KP_SEPARATOR") },
    { WXK_NUMPAD_SUBTRACT, wxTRANSLATE("-") },
    { WXK_NUMPAD_DECIMAL, wxTRANSLATE(".") },
    { WXK_NUMPAD_DIVIDE, wxTRANSLATE("/") },
    { WXK_WINDOWS_LEFT, wxTRANSLATE("WINDOWS_LEFT") },
    { WXK_WINDOWS_RIGHT, wxTRANSLATE("WINDOWS_RIGHT") },
    { WXK_WINDOWS_MENU, wxTRANSLATE("WINDOWS_MENU") },
    { WXK_COMMAND, wxTRANSLATE("COMMAND") },
};

NewKeyShortcutDlg::NewKeyShortcutDlg(wxWindow* parent, const MenuItemData& mid)
    : NewKeyShortcutBaseDlg(parent)
    , m_mid(mid)
{
    CentreOnParent();
    GetSizer()->Fit(this);
    Initialise(mid.accel);
}

void NewKeyShortcutDlg::Initialise(const clKeyboardShortcut& accel)
{
    m_staticTextAction->SetLabel(m_mid.action);
    m_textCtrl1->ChangeValue(accel.GetKeyCode());

#ifdef __WXMAC__
    m_checkBoxAlt->SetValue(accel.GetAlt());
    m_checkBoxCtrl->SetValue(accel.GetControl() == WXK_RAW_CONTROL); // Control, always control
    m_checkBoxCmd->SetValue(accel.GetControl() == WXK_CONTROL);      // CMD or Control
    m_checkBoxShift->SetValue(accel.GetShift());
#else
    m_checkBoxCmd->SetValue(false);
    m_checkBoxCmd->Enable(false);
    m_checkBoxAlt->SetValue(accel.GetAlt());
    m_checkBoxCtrl->SetValue(accel.GetControl() == WXK_CONTROL); // Control, always control
    m_checkBoxShift->SetValue(accel.GetShift());
#endif
}

void NewKeyShortcutDlg::OnKeyDown(wxKeyEvent& event)
{
    wxString text = ToString(event);
    if(text.IsEmpty()) {
        return;
    }
    m_textCtrl1->ChangeValue(text);
}

wxString NewKeyShortcutDlg::ToString(wxKeyEvent& e) const
{
    wxString text;

    // int flags = e.GetModifiers();
    // if ( flags & wxMOD_ALT )
    //     text += wxT("Alt-");
    // if ( flags & wxMOD_CONTROL )
    //     text += wxT("Ctrl-");
    // if ( flags & wxMOD_SHIFT )
    //     text += wxT("Shift-");

    const int code = e.GetKeyCode();

    if(code >= WXK_F1 && code <= WXK_F12) {
        text << _("F") << code - WXK_F1 + 1;
    } else if(code >= WXK_NUMPAD0 && code <= WXK_NUMPAD9) {
        text << code - WXK_NUMPAD0;
    } else if(code >= WXK_SPECIAL1 && code <= WXK_SPECIAL20) {
        text << _("SPECIAL") << code - WXK_SPECIAL1 + 1;
    } else { // check the named keys
        size_t n;
        for(n = 0; n < WXSIZEOF(wxKeyNames); n++) {
            const wxKeyName& kn = wxKeyNames[n];
            if(code == kn.code && kn.code != WXK_COMMAND) {
                text << wxGetTranslation(kn.name);
                break;
            }
        }

        if(n == WXSIZEOF(wxKeyNames)) {
            // must be a simple key
            if(isascii(code)) {
                text << (wxChar)code;
            } else {
                return wxEmptyString;
            }
        }
    }

    return text;
}

clKeyboardShortcut NewKeyShortcutDlg::GetAccel() const
{
#ifdef __WXMAC__
    wxKeyCode ctrl = WXK_NONE;
    if(m_checkBoxCmd->IsChecked()) {
        ctrl = WXK_CONTROL;
    } else if(m_checkBoxCtrl->IsChecked()) {
        ctrl = WXK_RAW_CONTROL;
    }
    clKeyboardShortcut shortcut(ctrl, m_checkBoxAlt->IsChecked(), m_checkBoxShift->IsChecked(),
                                m_textCtrl1->GetValue());
    return shortcut;
#else
    wxKeyCode ctrl = m_checkBoxCtrl->IsChecked() ? WXK_CONTROL : WXK_NONE;
    clKeyboardShortcut shortcut(ctrl, m_checkBoxAlt->IsChecked(), m_checkBoxShift->IsChecked(),
                                m_textCtrl1->GetValue());
    return shortcut;
#endif
}

void NewKeyShortcutDlg::OnClear(wxCommandEvent& event)
{
    // Clear the modifiers checkboxs
    m_checkBoxAlt->SetValue(false);
    m_checkBoxCtrl->SetValue(false);
    m_checkBoxShift->SetValue(false);

    m_textCtrl1->ChangeValue("");
}

void NewKeyShortcutDlg::OnClearUI(wxUpdateUIEvent& event)
{
    event.Enable(m_checkBoxAlt->IsChecked() || m_checkBoxCtrl->IsChecked() || m_checkBoxShift->IsChecked() ||
                 !m_textCtrl1->IsEmpty());
}

NewKeyShortcutDlg::~NewKeyShortcutDlg() {}

wxArrayString NewKeyShortcutDlg::GetSuggestions() const
{
    clKeyboardShortcut::Vec_t unassignedShortcuts = clKeyboardManager::Get()->GetAllUnassignedKeyboardShortcuts();

    wxArrayString suggestions;
    suggestions.Alloc(unassignedShortcuts.size());
    for(const clKeyboardShortcut& shortcut : unassignedShortcuts) {
        suggestions.Add(shortcut.ToString());
    }
    return suggestions;
}

void NewKeyShortcutDlg::OnSuggest(wxCommandEvent& event)
{
    clSingleChoiceDialog dlg(this, GetSuggestions(), 0);
    ::clSetDialogSizeAndPosition(&dlg, 1.2); // give it a reasonable size
    dlg.SetLabel(_("Select a Keyboard Shortcut"));
    if(dlg.ShowModal() == wxID_OK) {
        Initialise(dlg.GetSelection());
    }
}
