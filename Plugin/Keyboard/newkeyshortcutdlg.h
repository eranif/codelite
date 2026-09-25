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

#include "NewKeyShortcutBaseDlg.hpp"
#include "clKeyboardManager.h"

class WXDLLIMPEXP_SDK NewKeyShortcutDlg : public NewKeyShortcutBaseDlg
{
    MenuItemData m_mid;

protected:
    void OnSuggest(wxCommandEvent& event) override;
    void OnClear(wxCommandEvent& event) override;
    void OnClearUI(wxUpdateUIEvent& event) override;
    void OnKeyDown(wxKeyEvent& event) override;
    wxString ToString(wxKeyEvent& e) const;
    wxArrayString GetSuggestions() const;

    void Initialise(const clKeyboardShortcut& accel);

public:
    NewKeyShortcutDlg(wxWindow* parent, const MenuItemData& mid);
    ~NewKeyShortcutDlg() override;
    clKeyboardShortcut GetAccel() const;
};

#endif // __newkeyshortcutdlg__
