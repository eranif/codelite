//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editcmpfileinfodlg.cpp
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

#include "windowattrmanager.h"
#include "macrosdlg.h"
#include "editcmpfileinfodlg.h"
#include "manager.h"
#include "pluginmanager.h"

EditCmpFileInfo::EditCmpFileInfo(wxWindow* parent)
    : EditCmpFileInfoBase(parent)
{
    SetName("EditCmpFileInfo");
    WindowAttrManager::Load(this);
}

void EditCmpFileInfo::OnFileTypeText(wxCommandEvent& event) { event.Skip(); }

void EditCmpFileInfo::OnCompilationLine(wxCommandEvent& event) { event.Skip(); }

void EditCmpFileInfo::OnButtonOk(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_OK);
}

void EditCmpFileInfo::OnButtonOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrl2->IsEmpty() && !m_textCtrlFileType->IsEmpty());
}

void EditCmpFileInfo::OnButtonCancel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EndModal(wxID_CANCEL);
}

void EditCmpFileInfo::SetCompilationLine(const wxString& s) { m_textCtrl2->SetValue(s); }

void EditCmpFileInfo::SetExtension(const wxString& s) { m_textCtrlFileType->SetValue(s); }

void EditCmpFileInfo::SetKind(const wxString& s)
{
    int where = m_choiceFileTypeIs->FindString(s);
    if(where != wxNOT_FOUND) {
        m_choiceFileTypeIs->Select(where);
    }
}

void EditCmpFileInfo::OnButtonHelp(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString projectName = ManagerST::Get()->GetActiveProjectName();
    ProjectPtr project = ManagerST::Get()->GetProject(projectName);
    IEditor* editor = PluginManager::Get()->GetActiveEditor();

    MacrosDlg dlg(this, MacrosDlg::MacrosCompiler, project, editor);
    dlg.ShowModal();
}

EditCmpFileInfo::~EditCmpFileInfo() {}
