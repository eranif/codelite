//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : EditDlg.h
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

#ifndef EDITDLG_H
#define EDITDLG_H
#include "wxcrafter_plugin.h"

class WXDLLIMPEXP_SDK EditDlg : public EditDlgBase
{
public:
    EditDlg(wxWindow* parent, const wxString& text);
    virtual ~EditDlg();

    wxString GetText() const { return m_stc10->GetText(); }
};

WXDLLIMPEXP_SDK wxString clGetStringFromUser(const wxString& initialValue, wxWindow* parent = NULL);
#endif // EDITDLG_H
