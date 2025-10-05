//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : diff_dialog.cpp
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

#include "diff_dialog.h"
#include "windowattrmanager.h"
#include "imanager.h"

DiffDialog::DiffDialog( wxWindow* parent, IManager *manager )
    : DiffDialogBase( parent )
    , m_manager(manager)
{
    m_textCtrlFromRev->SetFocus();
    SetName("DiffDialog");
    WindowAttrManager::Load(this);
}

wxString DiffDialog::GetFromRevision() const
{
    wxString from = m_textCtrlFromRev->GetValue();
    from.Trim().Trim(false);

    if(from.IsEmpty()) {
        return wxT("BASE");
    } else {
        return from;
    }
}

wxString DiffDialog::GetToRevision() const
{
    wxString to = m_textCtrlToRev->GetValue();
    to.Trim().Trim(false);
    return to;
}

bool DiffDialog::IgnoreWhitespaces() const
{
    return m_checkBoxIgnoreWhitespace->IsChecked();
}
