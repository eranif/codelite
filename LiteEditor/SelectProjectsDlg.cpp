//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SelectProjectsDlg.cpp
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

#include "SelectProjectsDlg.h"
#include "windowattrmanager.h"
#include "manager.h"

SelectProjectsDlg::SelectProjectsDlg(wxWindow* parent)
    : SelectProjectsDlgBase(parent)
{
    // Populate the project list and check them all
    m_checkListBoxProjects->Clear();
    wxArrayString projects;
    ManagerST::Get()->GetProjectList( projects );
    
    for(size_t i=0; i<projects.GetCount(); ++i) {
        unsigned int pos = m_checkListBoxProjects->Append(projects.Item(i));
        m_checkListBoxProjects->Check(pos, true);
    }
    
    SetName("SelectProjectsDlg");
    WindowAttrManager::Load(this);
    ::clSetSmallDialogBestSizeAndPosition(this);
}

SelectProjectsDlg::~SelectProjectsDlg()
{
    
}

void SelectProjectsDlg::OnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i);
    }
}

void SelectProjectsDlg::OnUnSelectAll(wxCommandEvent& event)
{
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        m_checkListBoxProjects->Check(i, false);
    }
}

wxArrayString SelectProjectsDlg::GetProjects() const
{
    wxArrayString projects;
    for(unsigned int i=0; i<m_checkListBoxProjects->GetCount(); ++i) {
        if ( m_checkListBoxProjects->IsChecked(i) ) {
            projects.Add( m_checkListBoxProjects->GetString(i) );
        }
    }
    return projects;
}
