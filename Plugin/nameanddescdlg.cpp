//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : nameanddescdlg.cpp
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
#include "nameanddescdlg.h"

#include "imanager.h"

#include <list>
#include <set>

NameAndDescDlg::NameAndDescDlg(wxWindow* parent, IManager* manager, const wxString& name)
    : NameAndDescBaseDlg(parent)
{
    m_choiceType->Clear();
    std::set<wxString> categories;

    // Add the 'All' category
    categories.insert(wxT("All"));
    for (const auto& project : GetProjectTemplateList()) {
        wxString internalType = project->GetProjectInternalType();
        if (internalType.IsEmpty())
            internalType = wxT("Others");
        categories.insert(internalType);
    }

    for (const auto& category : categories) {
        m_choiceType->Append(category);
    }

    // Select the 'Console' to be the default
    int where = m_choiceType->FindString(wxT("Console"));
    if(where == wxNOT_FOUND) {
        where = 0;
    }
    m_choiceType->SetSelection(where);
    m_textCtrlName->SetValue(name);
}
