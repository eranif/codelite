//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : AddFunctionsImpDlg.h
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

#ifndef ADDFUNCTIONSIMPDLG_H
#define ADDFUNCTIONSIMPDLG_H

#include <wx/treebase.h>
#include "entry.h"
#include "wxcrafter.h"
#include <wx/arrstr.h>

class AddFunctionsImpDlg : public AddFunctionsImplBaseDlg
{
    TagEntryPtrVector_t m_tags;
    wxArrayString m_implArr;

protected:
    void DoCheckAll(bool checked);

public:
    AddFunctionsImpDlg(wxWindow* parent, const TagEntryPtrVector_t& tags, const wxString& targetFile);
    virtual ~AddFunctionsImpDlg();

    wxString GetText() const;
    wxString GetFileName() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnCheckAll(wxCommandEvent& event);
    virtual void OnUncheckAll(wxCommandEvent& event);
};

#endif // ADDFUNCTIONSIMPDLG_H
