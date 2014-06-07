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

#include "manager.h"
#include "NewKeyShortcutBaseDlg.h"

class NewKeyShortcutDlg : public NewKeyShortcutBaseDlg
{
    MenuItemData m_mid;
    enum {
        kAlt   = 0x00000001,
        kCtrl  = 0x00000002,
        kShift = 0x00000004,
    };
    
    struct KeyboardShortcut {
        size_t modifiers;
        wxString key;
        
        KeyboardShortcut() : modifiers(0) {}
    };
    
protected:
    virtual void OnClear(wxCommandEvent& event);
    virtual void OnClearUI(wxUpdateUIEvent& event);
    void OnKeyDown( wxKeyEvent& event );
    wxString ToString(wxKeyEvent &e);
    NewKeyShortcutDlg::KeyboardShortcut FromString( const wxString &accelString );
    
public:
    /** Constructor */
    NewKeyShortcutDlg( wxWindow* parent, const MenuItemData &mid );
    wxString GetAccel() const;
};

#endif // __newkeyshortcutdlg__
