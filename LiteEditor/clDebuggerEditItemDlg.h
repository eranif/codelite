//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clDebuggerEditItemDlg.h
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

#ifndef CLDEBUGGEREDITITEMDLG_H
#define CLDEBUGGEREDITITEMDLG_H

#include "newquickwatch.h"

class clDebuggerEditItemDlg : public clDebuggerEditItemDlgBase
{
public:
    clDebuggerEditItemDlg(wxWindow* parent, const wxString &initialValue);
    virtual ~clDebuggerEditItemDlg() = default;
    
    wxString GetValue() const;
    void SetValue(const wxString &value) {
        m_textCtrl38->ChangeValue( value );
    }
};
#endif // CLDEBUGGEREDITITEMDLG_H
