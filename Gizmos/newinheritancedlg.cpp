//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : newinheritancedlg.cpp
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
#include "newinheritancedlg.h"
#include "open_resource_dialog.h"
#include "windowattrmanager.h"
#include "imanager.h"

NewIneritanceDlg::NewIneritanceDlg(wxWindow* parent, IManager* mgr, const wxString& parentName, const wxString& access)
    : NewIneritanceBaseDlg(parent, wxID_ANY, _("New Inheritance"))
    , m_mgr(mgr)
{
    // Do this the hard way, rather than letting wxFB localise these particular strings :p
    const wxString AccessChoices[] = { wxT("public"), wxT("private"), wxT("protected") };
    wxArrayString choices(3, AccessChoices);
    m_choiceAccess->Clear();
    m_choiceAccess->Append(choices);
    // by default select 0
    m_choiceAccess->Select(0);
    if(access.IsEmpty() == false) {
        m_choiceAccess->SetStringSelection(access);
    }
    m_textCtrlInhertiance->SetValue(parentName);
    GetSizer()->Fit(this);
    CentreOnParent();
}

NewIneritanceDlg::~NewIneritanceDlg() {}

void NewIneritanceDlg::OnButtonMore(wxCommandEvent& event)
{
    m_textCtrlInhertiance->SetFocus();
    OpenResourceDialog dlg(this, m_mgr, "");

    // Open the resource dialog with 'class' and 'struct' only
    dlg.GetFilters().Add(KIND_CLASS);
    dlg.GetFilters().Add(KIND_STRUCT);

    if((dlg.ShowModal() == wxID_OK) && !dlg.GetSelections().empty()) {
        OpenResourceDialogItemData* item = dlg.GetSelections().at(0);
        wxString parentName;
        if(item->m_scope.IsEmpty() == false && item->m_scope != wxT("<global>")) {
            parentName << item->m_scope << wxT("::");
        }
        parentName << item->m_name;
        m_textCtrlInhertiance->SetValue(parentName);

        m_fileName = item->m_file;
    }
}
