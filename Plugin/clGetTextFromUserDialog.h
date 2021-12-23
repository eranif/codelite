//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clGetTextFromUserDialog.h
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

#ifndef CLGETTEXTFROMUSERDIALOG_H
#define CLGETTEXTFROMUSERDIALOG_H
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK clGetTextFromUserDialog : public clGetTextFromUserBaseDialog
{
public:
    clGetTextFromUserDialog(wxWindow* parent, const wxString& title, const wxString& message,
                            const wxString& initialValue, int charsToSelect = wxNOT_FOUND);
    virtual ~clGetTextFromUserDialog();

    wxString GetValue() const { return m_textCtrl->GetValue(); }
};
#endif // CLGETTEXTFROMUSERDIALOG_H
