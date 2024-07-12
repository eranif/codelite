//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newkeyshortcutdlg.h
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
#ifndef __newkeyshortcutdlg__
#define __newkeyshortcutdlg__

#include "NewKeyShortcutBaseDlg.h"
#include "clKeyboardManager.h"

class WXDLLIMPEXP_SDK NewKeyShortcutDlg : public NewKeyShortcutBaseDlg
{
    MenuItemData m_mid;

protected:
    virtual void OnSuggest(wxCommandEvent& event);
    virtual void OnClear(wxCommandEvent& event);
    virtual void OnClearUI(wxUpdateUIEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    wxString ToString(wxKeyEvent& e) const;
    wxArrayString GetSuggestions() const;

    void Initialise(const clKeyboardShortcut& accel);

public:
    NewKeyShortcutDlg(wxWindow* parent, const MenuItemData& mid);
    virtual ~NewKeyShortcutDlg();
    clKeyboardShortcut GetAccel() const;
};

#endif // __newkeyshortcutdlg__
