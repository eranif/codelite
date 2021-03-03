//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_item_dlg.cpp
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

#include "globals.h"
#include "macros.h"
#include "new_item_base_dlg.h"
#include "new_item_dlg.h"
#include "pluginmanager.h"
#include "windowattrmanager.h"
#include "wx/xrc/xmlres.h"
#include <wx/dirdlg.h>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>

///////////////////////////////////////////////////////////////////////////

NewItemDlg::NewItemDlg(wxWindow* parent, const wxString& cwd)
    : NewItemBaseDlg(parent)
{
    m_fileName->ChangeValue("Untitled.cpp");
    m_location->ChangeValue(cwd);
    int charsToSelect = wxStrlen("Untitled");
    m_fileName->SetSelection(0, charsToSelect);
    GetSizer()->Fit(this);
    CentreOnParent();
}

NewItemDlg::~NewItemDlg() {}

void NewItemDlg::OnBrowseButton(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector(_("select a folder"), m_location->GetValue());
    if(path.empty()) {
        return;
    }
    m_location->ChangeValue(path); // we want event here
}

wxFileName NewItemDlg::GetFileName() const
{
    wxFileName fn(m_location->GetValue(), m_fileName->GetValue());
    return fn;
}

void NewItemDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(wxFileName(m_location->GetValue(), "").DirExists() && !m_fileName->GetValue().IsEmpty());
}
