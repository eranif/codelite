//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SvnInfoDialog.h
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

#ifndef SVNINFODIALOG_H
#define SVNINFODIALOG_H
#include "subversion2_ui.h"

class SvnInfoDialog : public SvnInfoDialogBase
{
public:
    SvnInfoDialog(wxWindow* parent);
    virtual ~SvnInfoDialog();
    wxTextCtrl* GetTextCtrlAuthor() {
        return m_textCtrlAuthor;
    }
    wxTextCtrl* GetTextCtrlDate() {
        return m_textCtrlDate;
    }
    wxTextCtrl* GetTextCtrlRevision() {
        return m_textCtrlRevision;
    }
    wxTextCtrl* GetTextCtrlRootURL() {
        return m_textCtrlRootURL;
    }
    wxTextCtrl* GetTextCtrlURL() {
        return m_textCtrlURL;
    }
};
#endif // SVNINFODIALOG_H
