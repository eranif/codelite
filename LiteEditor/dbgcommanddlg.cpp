//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dbgcommanddlg.cpp
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
#include "dbgcommanddlg.h"
#include "globals.h"
#include "windowattrmanager.h"

DbgCommandDlg::DbgCommandDlg(wxWindow* parent)
    : DbgCommandBaseDlg(parent)
{
    m_textCtrlName->ChangeValue("");
    m_textCtrlName->CallAfter(&wxTextCtrl::SetFocus);
    ::clSetSmallDialogBestSizeAndPosition(this);
    CentreOnParent();
}

DbgCommandDlg::~DbgCommandDlg() {}
