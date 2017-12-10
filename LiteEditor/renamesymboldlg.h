//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : renamesymboldlg.h
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
#ifndef __renamesymboldlg__
#define __renamesymboldlg__

#include "cpptoken.h"
#include <list>
#include <vector>
#include "rename_symbool_dlg.h"

class RenameSymbol : public RenameSymbolBase
{
    CppToken::Vec_t m_tokens;
    wxString m_filename;

protected:
    virtual void OnCheckAll(wxCommandEvent& event);
    virtual void OnUncheckAll(wxCommandEvent& event);
    virtual void OnSelection(wxDataViewEvent& event);
    void OnButtonOK(wxCommandEvent& e);
    void AddMatch(const CppToken& token, bool check);
    void DoSelectFile(const CppToken& token);

public:
    RenameSymbol(wxWindow* parent, const CppToken::Vec_t& candidates, const CppToken::Vec_t& possCandidates,
                 const wxString& oldname = wxEmptyString);
    virtual ~RenameSymbol();
    void GetMatches(CppToken::Vec_t& matches);
    wxString GetWord() const { return m_textCtrlNewName->GetValue(); }
};

#endif // __renamesymboldlg__
