//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : testclassdlg.cpp
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

#include <wx/app.h>
#include <wx/choicdlg.h>
#include "unittestpp.h"
#include "open_resource_dialog.h"
#include <wx/msgdlg.h>
#include "testclassdlg.h"
#include "imanager.h"
#include "ctags_manager.h"
#include "windowattrmanager.h"

TestClassDlg::TestClassDlg(wxWindow* parent, IManager* mgr, UnitTestPP* plugin)
    : TestClassBaseDlg(parent)
    , m_manager(mgr)
    , m_plugin(plugin)
{
    m_manager->GetTagsManager()->GetClasses(m_tags);

    // populate the unit tests project list
    std::vector<ProjectPtr> projects = m_plugin->GetUnitTestProjects();
    for(size_t i = 0; i < projects.size(); i++) {
        m_choiceProjects->Append(projects.at(i)->GetName());
    }

    if(m_choiceProjects->IsEmpty() == false) {
        m_choiceProjects->SetSelection(0);
    }
    SetName("TestClassDlg");
    WindowAttrManager::Load(this);
}

TestClassDlg::~TestClassDlg() {  }

void TestClassDlg::OnRefreshFunctions(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoRefreshFunctions();
}

void TestClassDlg::OnUseActiveEditor(wxCommandEvent& event)
{
    if(event.IsChecked()) {
        IEditor* editor = m_manager->GetActiveEditor();
        if(editor) {
            m_textCtrlFileName->SetValue(editor->GetFileName().GetFullPath());
        }
        m_textCtrlFileName->Enable(true);
    } else {
        m_textCtrlFileName->Enable(false);
    }
}

void TestClassDlg::OnRefreshButtonUI(wxUpdateUIEvent& e)
{
    e.Enable(m_textCtrlClassName->GetValue().IsEmpty() ? false : true);
}

void TestClassDlg::OnCheckAll(wxCommandEvent& e)
{
    // check all items
    for(unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
        m_checkListMethods->Check(idx, true);
    }
}

void TestClassDlg::OnUnCheckAll(wxCommandEvent& e)
{
    // check all items
    for(unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
        m_checkListMethods->Check(idx, false);
    }
}
wxArrayString TestClassDlg::GetTestsList()
{
    wxArrayString results;
    for(unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
        if(m_checkListMethods->IsChecked(idx)) {
            wxString str = m_checkListMethods->GetString(idx);

            str = str.BeforeFirst(wxT('('));
            EscapeName(str);
            str.Prepend(m_textCtrlClassName->GetValue() + wxT("_"));
            results.Add(str);
        }
    }
    return results;
}

void TestClassDlg::OnUseFixture(wxCommandEvent& e) { m_textCtrlFixtureName->Enable(e.IsChecked()); }

void TestClassDlg::OnButtonOk(wxCommandEvent& e)
{
    // validate the class name
    if(m_checkListMethods->GetCount() == 0) {
        wxMessageBox(_("There are no tests to generate"), _("CodeLite"), wxICON_WARNING | wxOK);
        return;
    }
    EndModal(wxID_OK);
}

void TestClassDlg::OnShowClassListDialog(wxCommandEvent& e)
{
    m_textCtrlClassName->SetFocus();
    OpenResourceDialog dlg(m_manager->GetTheApp()->GetTopWindow(), m_manager, "");
    if(dlg.ShowModal() == wxID_OK && !dlg.GetSelections().empty()) {
        OpenResourceDialogItemData* item = dlg.GetSelections().at(0);
        // do something with the selected text
        m_textCtrlClassName->SetValue(item->m_name);

        // display the class methods
        DoRefreshFunctions();
    }
}

void TestClassDlg::DoRefreshFunctions(bool repportError)
{
    std::vector<TagEntryPtr> matches;

    // search m_tags for suitable name
    for(size_t i = 0; i < m_tags.size(); i++) {
        TagEntryPtr tag = m_tags.at(i);
        if(tag->GetName() == m_textCtrlClassName->GetValue()) {
            matches.push_back(tag);
        }
    }

    if(matches.empty()) {
        if(repportError) {
            wxMessageBox(_("Could not find match for class '") + m_textCtrlClassName->GetValue() + wxT("'"),
                         _("CodeLite"),
                         wxICON_WARNING | wxOK);
        }
        return;
    }

    wxString theClass;
    if(matches.size() == 1) {
        // single match we are good
        theClass = matches.at(0)->GetPath();
    } else {
        // suggest the user a multiple choice
        wxArrayString choices;

        for(size_t i = 0; i < matches.size(); i++) {
            wxString option;
            TagEntryPtr t = matches.at(i);
            choices.Add(t->GetPath());
        }

        theClass = wxGetSingleChoice(_("Select class:"), _("Select class:"), choices, this);
    }

    if(theClass.empty()) { // user clicked 'Cancel'
        return;
    }

    // get list of methods for the given path
    matches.clear();
    m_manager->GetTagsManager()->TagsByScope(theClass, wxT("prototype"), matches, false, true);

    // populate the list control
    wxArrayString methods;
    for(size_t i = 0; i < matches.size(); i++) {
        TagEntryPtr t = matches.at(i);
        methods.Add(t->GetName() + t->GetSignature());
    }
    m_checkListMethods->Clear();
    m_checkListMethods->Append(methods);

    // check all items
    for(unsigned int idx = 0; idx < m_checkListMethods->GetCount(); idx++) {
        m_checkListMethods->Check(idx, true);
    }
}

void TestClassDlg::SetClassName(const wxString& clsName)
{
    m_textCtrlClassName->SetValue(clsName);
    DoRefreshFunctions(false);
}

void TestClassDlg::EscapeName(wxString& name)
{
    name.Replace(wxT(" "), wxEmptyString);
    name.Replace(wxT("~"), wxT("Tilda"));
    name.Replace(wxT("="), wxT("Shave"));
    name.Replace(wxT(">"), wxT("Gadol"));
    name.Replace(wxT("<"), wxT("Katan"));
}
