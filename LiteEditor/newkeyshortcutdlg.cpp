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

static const struct wxKeyName
{
    wxKeyCode code;
  // Pre-2.9 wxTRANSLATE returned  a wxChar*, since then it's char*
#if wxVERSION_NUMBER < 2900
    const wxChar *name;
#else
    const char *name;
#endif
} wxKeyNames[] =
{
    { WXK_DELETE, wxTRANSLATE("DEL") },
    { WXK_DELETE, wxTRANSLATE("DELETE") },
    { WXK_BACK, wxTRANSLATE("BACK") },
    { WXK_INSERT, wxTRANSLATE("INS") },
    { WXK_INSERT, wxTRANSLATE("INSERT") },
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
    { WXK_NUMPAD_SPACE, wxTRANSLATE("KP_SPACE") },
    { WXK_NUMPAD_TAB, wxTRANSLATE("KP_TAB") },
    { WXK_NUMPAD_ENTER, wxTRANSLATE("KP_ENTER") },
    { WXK_NUMPAD_HOME, wxTRANSLATE("KP_HOME") },
    { WXK_NUMPAD_LEFT, wxTRANSLATE("KP_LEFT") },
    { WXK_NUMPAD_UP, wxTRANSLATE("KP_UP") },
    { WXK_NUMPAD_RIGHT, wxTRANSLATE("KP_RIGHT") },
    { WXK_NUMPAD_DOWN, wxTRANSLATE("KP_DOWN") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PRIOR") },
    { WXK_NUMPAD_PAGEUP, wxTRANSLATE("KP_PAGEUP") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_NEXT") },
    { WXK_NUMPAD_PAGEDOWN, wxTRANSLATE("KP_PAGEDOWN") },
    { WXK_NUMPAD_END, wxTRANSLATE("KP_END") },
    { WXK_NUMPAD_BEGIN, wxTRANSLATE("KP_BEGIN") },
    { WXK_NUMPAD_INSERT, wxTRANSLATE("KP_INSERT") },
    { WXK_NUMPAD_DELETE, wxTRANSLATE("KP_DELETE") },
    { WXK_NUMPAD_EQUAL, wxTRANSLATE("KP_EQUAL") },
    { WXK_NUMPAD_MULTIPLY, wxTRANSLATE("KP_MULTIPLY") },
    { WXK_NUMPAD_ADD, wxTRANSLATE("KP_ADD") },
    { WXK_NUMPAD_SEPARATOR, wxTRANSLATE("KP_SEPARATOR") },
    { WXK_NUMPAD_SUBTRACT, wxTRANSLATE("KP_SUBTRACT") },
    { WXK_NUMPAD_DECIMAL, wxTRANSLATE("KP_DECIMAL") },
    { WXK_NUMPAD_DIVIDE, wxTRANSLATE("KP_DIVIDE") },
    { WXK_WINDOWS_LEFT, wxTRANSLATE("WINDOWS_LEFT") },
    { WXK_WINDOWS_RIGHT, wxTRANSLATE("WINDOWS_RIGHT") },
    { WXK_WINDOWS_MENU, wxTRANSLATE("WINDOWS_MENU") },
    { WXK_COMMAND, wxTRANSLATE("COMMAND") },
};

NewKeyShortcutDlg::NewKeyShortcutDlg( wxWindow* parent, const MenuItemData & mid )
		:
		NewKeyShortcutBaseDlg( parent )
		, m_mid(mid)
{
	m_staticTextAction->SetLabel( wxT("Action:\t") + m_mid.action );
	m_textCtrl1->SetValue( m_mid.accel );
	m_textCtrl1->SetFocus();
	Centre();
}

void NewKeyShortcutDlg::OnKeyDown( wxKeyEvent& event )
{
	wxString text = ToString(event);
	if(text.IsEmpty()) { return; }
	m_textCtrl1->SetValue( text );
}

wxString NewKeyShortcutDlg::ToString(wxKeyEvent &e)
{
	wxString text;

	int flags = e.GetModifiers();
	if ( flags & wxACCEL_ALT )
		text += wxT("Alt-");
	if ( flags & wxACCEL_CTRL )
		text += wxT("Ctrl-");
	if ( flags & wxACCEL_SHIFT )
		text += wxT("Shift-");

	const int code = e.GetKeyCode();

	if ( code >= WXK_F1 && code <= WXK_F12 )
		text << _("F") << code - WXK_F1 + 1;
	else if ( code >= WXK_NUMPAD0 && code <= WXK_NUMPAD9 )
		text << _("KP_") << code - WXK_NUMPAD0;
	else if ( code >= WXK_SPECIAL1 && code <= WXK_SPECIAL20 )
		text << _("SPECIAL") << code - WXK_SPECIAL1 + 1;
	else { // check the named keys
		size_t n;
		for ( n = 0; n < WXSIZEOF(wxKeyNames); n++ ) {
			const wxKeyName& kn = wxKeyNames[n];
			if ( code == kn.code ) {
				text << wxGetTranslation(kn.name);
				break;
			}
		}

		if ( n == WXSIZEOF(wxKeyNames) ) {
			// must be a simple key
			if (
			    isascii(code) /*&&
			    wxIsalnum(code)*/ ) {
				text << (wxChar)code;
			} else {
				return wxEmptyString;
			}
		}
	}

	return text;
}

void NewKeyShortcutDlg::OnButtonClear(wxCommandEvent &event)
{
	wxUnusedVar(event);
	m_textCtrl1->Clear();
}
