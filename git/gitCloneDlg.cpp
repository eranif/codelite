//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitCloneDlg.cpp
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

#include "gitCloneDlg.h"
#include "windowattrmanager.h"
#include <wx/uri.h>

gitCloneDlg::gitCloneDlg(wxWindow* parent)
    : gitCloneDlgBaseClass(parent)
{
    m_textCtrlURL->SetFocus();
    SetName("gitCloneDlg");
    WindowAttrManager::Load(this);
}

void gitCloneDlg::OnOKUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlURL->IsEmpty() && !m_dirPickerTargetDir->GetPath().IsEmpty());
}

wxString gitCloneDlg::GetCloneURL() const
{
    wxString urlString = m_textCtrlURL->GetValue();
    return urlString;
}
