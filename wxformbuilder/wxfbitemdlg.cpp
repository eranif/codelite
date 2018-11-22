//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : wxfbitemdlg.cpp
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

#include "VirtualDirectorySelectorDlg.h"
#include "globals.h"
#include "imanager.h"
#include "project.h"
#include "wxfbitemdlg.h"
#include <wx/app.h>
#include <wx/msgdlg.h>

wxFBItemDlg::wxFBItemDlg(wxWindow* parent, IManager* mgr)
    : wxFBItemBaseDlg(parent)
    , m_mgr(mgr)
{
    TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
    if(item.m_item.IsOk() && item.m_itemType == ProjectItem::TypeVirtualDirectory) {
        m_textCtrlVD->SetValue(VirtualDirectorySelectorDlg::DoGetPath(m_mgr->GetWorkspaceTree(), item.m_item, false));
    }
    m_textCtrlClassName->SetFocus();
    GetSizer()->Fit(this);
}

void wxFBItemDlg::OnGenerate(wxCommandEvent& event)
{
    wxUnusedVar(event);

    if(!IsValidCppIndetifier(m_textCtrlClassName->GetValue())) {
        wxMessageBox(_("Invalid C++ class name"), _("CodeLite"), wxOK | wxICON_WARNING);
        return;
    }

    EndModal(wxID_OK);
}

void wxFBItemDlg::OnGenerateUI(wxUpdateUIEvent& event)
{
    bool titleCondition;
    if(m_textCtrlTitle->IsEnabled()) {
        titleCondition = !m_textCtrlTitle->IsEmpty();
    } else {
        titleCondition = true;
    }

    event.Enable(!m_textCtrlClassName->IsEmpty() && titleCondition && !m_textCtrlVD->IsEmpty());
}

void wxFBItemDlg::OnCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

void wxFBItemDlg::OnBrowseVD(wxCommandEvent& event)
{
    VirtualDirectorySelectorDlg dlg(this, m_mgr->GetWorkspace(), m_textCtrlVD->GetValue());
    if(dlg.ShowModal() == wxID_OK) {
        m_textCtrlVD->SetValue(dlg.GetVirtualDirectoryPath());
    }
}

wxFBItemInfo wxFBItemDlg::GetData()
{
    wxFBItemInfo info;
    info.className = m_textCtrlClassName->GetValue();
    info.kind = wxFBItemKind_Unknown;
    info.title = m_textCtrlTitle->GetValue();
    info.virtualFolder = m_textCtrlVD->GetValue();
    info.file = m_textCtrlClassName->GetValue().Lower();

    return info;
}

void wxFBItemDlg::DisableTitleField() { m_textCtrlTitle->Enable(false); }
