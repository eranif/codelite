//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SvnLogDialog.h
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

#ifndef SVNLOGDIALOG_H
#define SVNLOGDIALOG_H
#include "subversion2_ui.h"

class SvnLogDialog : public SvnLogDialogBase
{
public:
    SvnLogDialog(wxWindow* parent);
    virtual ~SvnLogDialog() = default;

    void SetCompact(wxCheckBox* compact) {
        this->m_compact = compact;
    }
    void SetFrom(wxTextCtrl* from) {
        this->m_from = from;
    }
    void SetTo(wxTextCtrl* to) {
        this->m_to = to;
    }
    wxCheckBox* GetCompact() {
        return m_compact;
    }
    wxTextCtrl* GetFrom() {
        return m_from;
    }
    wxTextCtrl* GetTo() {
        return m_to;
    }
};
#endif // SVNLOGDIALOG_H
