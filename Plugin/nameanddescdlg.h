//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : nameanddescdlg.h
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
#ifndef __nameanddescdlg__
#define __nameanddescdlg__

#include "codelite_exports.h"
#include "nameanddescbasedlg.h"
class IManager;

class WXDLLIMPEXP_SDK NameAndDescDlg : public NameAndDescBaseDlg
{
public:
    /** Constructor */
    NameAndDescDlg(wxWindow* parent, IManager* manager, const wxString& name);
    wxString GetName() const { return m_textCtrlName->GetValue(); }
    wxString GetDescription() const { return m_textCtrlDescription->GetValue(); }
    wxString GetType() const { return m_choiceType->GetValue(); }
};

#endif // __nameanddescdlg__
