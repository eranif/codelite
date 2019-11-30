//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : advanced_settings.cpp
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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CompilersFoundDlg.h"
#include "NewCompilerDlg.h"
#include "advance_settings_base.h"
#include "advanced_settings.h"
#include "build_page.h"
#include "build_settings_config.h"
#include "buildsettingstab.h"
#include "editor_config.h"
#include "frame.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "windowattrmanager.h"
#include <CompilersDetectorManager.h>
#include <cl_aui_notebook_art.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

///////////////////////////////////////////////////////////////////////////
BuildSettingsDialog::BuildSettingsDialog(wxWindow* parent, size_t selected_page, int id, wxString title, wxPoint pos,
                                         wxSize size, int style)
    : AdvancedDlgBase(parent)
    , m_rightclickMenu(NULL)
{
    m_compilersPage = new CompilerMainPage(m_notebook);
    m_notebook->AddPage(m_compilersPage, _("Compilers"), true);
    m_buildSettings = new BuildTabSetting(m_notebook);
    m_notebook->AddPage(m_buildSettings, _("Build Output Appearance"), false);

    m_buildPage = new BuildPage(m_notebook);
    m_notebook->AddPage(m_buildPage, _("Build Systems"), false);
    m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));
    LoadCompilers();
    clSetDialogBestSizeAndPosition(this);
}

void BuildSettingsDialog::LoadCompilers() { m_compilersPage->LoadCompilers(); }

BuildSettingsDialog::~BuildSettingsDialog() { wxDELETE(m_rightclickMenu); }

void BuildSettingsDialog::OnButtonNewClicked()
{
    NewCompilerDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        CreateNewCompiler(dlg.GetCompilerName(), dlg.GetMasterCompiler());
        LoadCompilers();
    }
}

void BuildSettingsDialog::OnButtonOKClicked(wxCommandEvent& event)
{
    OnApply(event);
    EndModal(wxID_OK);
}

void BuildSettingsDialog::SaveCompilers()
{
    std::map<wxString, std::vector<ICompilerSubPage*>>::iterator iter = m_compilerPagesMap.begin();
    for(; iter != m_compilerPagesMap.end(); iter++) {
        std::vector<ICompilerSubPage*> items = iter->second;
        wxString cmpname = iter->first;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpname);
        if(cmp) {
            for(size_t i = 0; i < items.size(); i++) {
                ICompilerSubPage* p = items.at(i);
                p->Save(cmp);
            }
            BuildSettingsConfigST::Get()->SetCompiler(cmp); // save changes
        }
    }
}

bool BuildSettingsDialog::CreateNewCompiler(const wxString& name, const wxString& copyFrom)
{
    if(BuildSettingsConfigST::Get()->IsCompilerExist(name)) {
        wxMessageBox(_("A compiler with this name already exists"), _("Error"), wxOK | wxICON_HAND);
        return false;
    }

    CompilerPtr cmp;
    if(!copyFrom.IsEmpty()) {
        cmp = BuildSettingsConfigST::Get()->GetCompiler(copyFrom);
    } else {
        cmp = BuildSettingsConfigST::Get()->GetCompiler(name);
    }
    cmp->SetName(name);
    BuildSettingsConfigST::Get()->SetCompiler(cmp);
    return true;
}

bool BuildSettingsDialog::DeleteCompiler(const wxString& name)
{
    if(wxMessageBox(_("Remove Compiler?"), _("Confirm"), wxYES_NO | wxICON_QUESTION) == wxYES) {
        BuildSettingsConfigST::Get()->DeleteCompiler(name);
        return true;
    }
    return false;
}

void BuildSettingsDialog::OnContextMenu(wxContextMenuEvent& e)
{
    //    wxTreeCtrl *tree = m_compilersNotebook->GetTreeCtrl();
    //    wxTreeItemId item = tree->GetSelection();
    //
    //    // only compilers have children
    //    if(item.IsOk() && tree->HasChildren(item)) {
    //        PopupMenu(m_rightclickMenu);
    //    }
}

#define ID_MENU_AUTO_DETECT_COMPILERS 1001
#define ID_MENU_ADD_COMPILER_BY_PATH 1002
#define ID_MENU_CLONE_COMPILER 1003

void BuildSettingsDialog::OnAutoDetectCompilers(wxButton* btn)
{
    // Launch the auto detect compilers code

    wxMenu menu;
    menu.Append(ID_MENU_ADD_COMPILER_BY_PATH, _("Add an existing compiler"));
    menu.Append(ID_MENU_CLONE_COMPILER, _("Clone a compiler"));
    menu.AppendSeparator();
    menu.Append(ID_MENU_AUTO_DETECT_COMPILERS, _("Scan computer for installed compilers"));

    // Menu will be shown in client coordinates
    wxRect size = btn->GetSize();
    wxPoint menuPos(0, size.GetHeight());

    int res = btn->GetPopupMenuSelectionFromUser(menu, menuPos);
    if(res == ID_MENU_AUTO_DETECT_COMPILERS) {
        if(m_compilersDetector.Locate()) {
            CallAfter(&BuildSettingsDialog::OnCompilersDetected, m_compilersDetector.GetCompilersFound());
        }

    } else if(res == ID_MENU_CLONE_COMPILER) {
        CallAfter(&BuildSettingsDialog::OnButtonNewClicked);

    } else if(res == ID_MENU_ADD_COMPILER_BY_PATH) {
        CallAfter(&BuildSettingsDialog::OnAddExistingCompiler);
    }
}

void BuildSettingsDialog::OnCompilersDetected(const ICompilerLocator::CompilerVec_t& compilers)
{
    CompilersFoundDlg dlg(this, compilers);
    if(dlg.ShowModal() == wxID_OK) {
        // Replace the current compilers with a new one
        BuildSettingsConfigST::Get()->SetCompilers(compilers);

        // update the code completion search paths
        clMainFrame::Get()->CallAfter(&clMainFrame::UpdateParserSearchPathsFromDefaultCompiler);

        // Dismiss this dialog and reload it
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("advance_settings"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
        EndModal(wxID_OK);
    }
}

void BuildSettingsDialog::OnApply(wxCommandEvent& event)
{
    // save the build page
    m_compilersPage->Save();
    m_buildPage->Save();
    m_buildSettings->Save();

    // mark all the projects as dirty
    wxArrayString projects;
    clCxxWorkspaceST::Get()->GetProjectList(projects);
    for(size_t i = 0; i < projects.size(); i++) {
        ProjectPtr proj = ManagerST::Get()->GetProject(projects.Item(i));
        if(proj) { proj->SetModified(true); }
    }
}

void BuildSettingsDialog::OnApplyUI(wxUpdateUIEvent& event)
{
    event.Enable(m_compilersPage->IsDirty() || m_buildSettings->IsModified());
}

void BuildSettingsDialog::OnAddExistingCompiler()
{
    wxString folder = ::wxDirSelector(_("Select the compiler folder"));
    if(folder.IsEmpty()) { return; }

    CompilerPtr cmp = m_compilersDetector.Locate(folder);
    if(cmp) {
        // We found new compiler
        // Prompt the user to give it a name
        while(true) {
            wxString name =
                ::wxGetTextFromUser(_("Set a name to the compiler"), _("New compiler found!"), cmp->GetName());
            if(name.IsEmpty()) { return; }
            // Add the compiler
            if(BuildSettingsConfigST::Get()->IsCompilerExist(name)) { continue; }
            cmp->SetName(name);
            break;
        }

        // Save the new compiler
        BuildSettingsConfigST::Get()->SetCompiler(cmp);

        // Reload the dialog
        LoadCompilers();
    }
}

void BuildSettingsDialog::OnScanAndSuggestCompilers()
{
    if(m_compilersDetector.Locate()) {
        CallAfter(&BuildSettingsDialog::OnCompilersDetected, m_compilersDetector.GetCompilersFound());
    }
}
