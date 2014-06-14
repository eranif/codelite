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

#include "windowattrmanager.h"
#include "macros.h"
#include "buildsettingstab.h"
#include "advanced_settings.h"
#include "manager.h"
#include "editor_config.h"
#include <wx/xrc/xmlres.h>
#include "build_settings_config.h"
#include "build_page.h"
#include "globals.h"
#include "frame.h"
#include <wx/textdlg.h>
#include "advance_settings_base.h"
#include "NewCompilerDlg.h"
#include <CompilersDetectorManager.h>
#include "CompilersFoundDlg.h"

BEGIN_EVENT_TABLE(AdvancedDlg, AdvancedDlgBase)
    EVT_MENU(XRCID("delete_compiler"), AdvancedDlg::OnDeleteCompiler)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////

AdvancedDlg::AdvancedDlg( wxWindow* parent, size_t selected_page, int id, wxString title, wxPoint pos, wxSize size, int style )
    : AdvancedDlgBase( parent )
    , m_rightclickMenu(NULL)
{
    m_compilersMainPanel = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxHORIZONTAL );

    m_staticText1 = new wxStaticText( m_compilersMainPanel, wxID_ANY, _("Create New Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_buttonNewCompiler = new wxButton( m_compilersMainPanel, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_buttonNewCompiler, 0, wxALL|wxALIGN_RIGHT, 5 );

    m_buttonAutoDetect = new wxButton( m_compilersMainPanel, wxID_ANY, _("Auto Detect Compilers..."), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_buttonAutoDetect, 0, wxALL|wxALIGN_RIGHT, 5 );

    bSizer5->Add( bSizer4, 0, wxALIGN_RIGHT|wxEXPAND, 5 );

    m_compilersPage = new CompilerMainPage(m_compilersMainPanel);
    bSizer5->Add( m_compilersPage, 1, wxALL|wxEXPAND, 5 );
    
    m_compilersMainPanel->SetSizer( bSizer5 );
    m_compilersMainPanel->Layout();
    
    m_notebook->AddPage( m_compilersMainPanel, _("Compilers"), true );
    m_buildSettings = new BuildTabSetting(m_notebook);
    m_notebook->AddPage(m_buildSettings, _("Build Output Appearance"), false);

    m_buildPage = new BuildPage(m_notebook);
    m_notebook->AddPage(m_buildPage, _("Build Systems"), false);

    m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));

    LoadCompilers();
    
    m_buttonNewCompiler->Bind(wxEVT_BUTTON, &AdvancedDlg::OnButtonNewClicked, this);
    m_buttonAutoDetect->Bind(wxEVT_BUTTON, &AdvancedDlg::OnAutoDetectCompilers, this);
    
    // center the dialog
    Centre();
    this->Layout();
    
    WindowAttrManager::Load(this, wxT("BuildSettingsDlg"), NULL);
}

void AdvancedDlg::LoadCompilers()
{
    m_compilersPage->LoadCompilers();
}

AdvancedDlg::~AdvancedDlg()
{
    wxDELETE(m_rightclickMenu);
    WindowAttrManager::Save(this, wxT("BuildSettingsDlg"), NULL);
}

void AdvancedDlg::OnButtonNewClicked(wxCommandEvent &event)
{
    wxUnusedVar(event);
    NewCompilerDlg dlg(this);
    if ( dlg.ShowModal() == wxID_OK ) {
        CreateNewCompiler(dlg.GetCompilerName(), dlg.GetMasterCompiler());
        LoadCompilers();
    }
}

void AdvancedDlg::OnButtonOKClicked(wxCommandEvent &event)
{
    wxUnusedVar(event);
    //save all compiler pages
    SaveCompilers();

    //save the build page
    m_buildPage->Save();
    m_buildSettings->Save();

    //mark all the projects as dirty
    wxArrayString projects;
    WorkspaceST::Get()->GetProjectList( projects );
    for ( size_t i=0; i< projects.size(); i++ ) {
        ProjectPtr proj = ManagerST::Get()->GetProject( projects.Item(i) );
        if ( proj ) {
            proj->SetModified( true );
        }
    }
    EndModal(wxID_OK);
}

void AdvancedDlg::OnDeleteCompiler(wxCommandEvent & event)
{
    wxUnusedVar(event);

//    int sel = m_compilersNotebook->GetSelection();
//
//    if (sel != wxNOT_FOUND) {
//        wxString name = m_compilersNotebook->GetPageText((size_t)sel);
//        if (DeleteCompiler(name)) {
//            m_compilersNotebook->DeletePage((size_t)sel);
//
//            std::map<wxString, std::vector<ICompilerSubPage*> >::iterator iter = m_compilerPagesMap.find(name);
//            if(iter != m_compilerPagesMap.end()) {
//                m_compilerPagesMap.erase(iter);
//            }
//
//            if(m_compilersNotebook->GetPageCount()) {
//                m_compilersNotebook->ExpandNode(0);
//                m_compilersNotebook->SetSelection(1);
//            }
//        }
//    }
}

void AdvancedDlg::SaveCompilers()
{
    std::map<wxString, std::vector<ICompilerSubPage*> >::iterator iter = m_compilerPagesMap.begin();
    for(; iter != m_compilerPagesMap.end(); iter ++) {
        std::vector<ICompilerSubPage*> items = iter->second;
        wxString cmpname = iter->first;
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpname);
        if(cmp) {
            for(size_t i=0; i<items.size(); i++) {
                ICompilerSubPage* p = items.at(i);
                p->Save(cmp);
            }
            BuildSettingsConfigST::Get()->SetCompiler(cmp);//save changes
        }
    }
}

bool AdvancedDlg::CreateNewCompiler (const wxString& name, const wxString& copyFrom)
{
    if ( BuildSettingsConfigST::Get()->IsCompilerExist ( name ) ) {
        wxMessageBox ( _( "A compiler with this name already exists" ), _( "Error" ), wxOK | wxICON_HAND );
        return false;
    }
    
    CompilerPtr cmp;
    if ( !copyFrom.IsEmpty() ) {
        cmp = BuildSettingsConfigST::Get()->GetCompiler ( copyFrom );
    } else {
        cmp = BuildSettingsConfigST::Get()->GetCompiler ( name );
    }
    cmp->SetName ( name );
    BuildSettingsConfigST::Get()->SetCompiler ( cmp );
    return true;
}

bool AdvancedDlg::DeleteCompiler ( const wxString &name )
{
    if ( wxMessageBox ( _( "Remove Compiler?" ), _( "Confirm" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
        BuildSettingsConfigST::Get()->DeleteCompiler ( name );
        return true;
    }
    return false;
}

void AdvancedDlg::OnContextMenu(wxContextMenuEvent& e)
{
//    wxTreeCtrl *tree = m_compilersNotebook->GetTreeCtrl();
//    wxTreeItemId item = tree->GetSelection();
//
//    // only compilers have children
//    if(item.IsOk() && tree->HasChildren(item)) {
//        PopupMenu(m_rightclickMenu);
//    }
}

void AdvancedDlg::OnRestoreDefaults(wxCommandEvent&)
{
    if(wxMessageBox(_("Are you sure you want to revert to the default settings?"),
                    wxT("CodeLite"),
                    wxYES_NO|wxCANCEL|wxCENTRE|wxICON_WARNING,
                    this) == wxYES) {
        // restore the default settings of the build configuration
        BuildSettingsConfigST::Get()->RestoreDefaults();

        // Dismiss this dialog and reload it
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("advance_settings"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
        EndModal(wxID_OK);
    }
}

void AdvancedDlg::OnAutoDetectCompilers(wxCommandEvent&)
{
    // Launch the auto detect compilers code
    if ( m_compilersDetector.Locate() ) {
        CallAfter( &AdvancedDlg::OnCompilersDetected, m_compilersDetector.GetCompilersFound() );
    }
}

void AdvancedDlg::OnCompilersDetected(const ICompilerLocator::CompilerVec_t& compilers)
{
    CompilersFoundDlg dlg(this, compilers);
    if ( dlg.ShowModal() == wxID_OK ) {
        // Replace the current compilers with a new one
        BuildSettingsConfigST::Get()->SetCompilers( compilers );
        
        // Dismiss this dialog and reload it
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("advance_settings"));
        clMainFrame::Get()->GetEventHandler()->AddPendingEvent(event);
        EndModal(wxID_OK);
    }
}

void AdvancedDlg::OnApply(wxCommandEvent& event)
{
    m_compilersPage->Save();
}

void AdvancedDlg::OnApplyUI(wxUpdateUIEvent& event)
{
    event.Enable( m_compilersPage->IsDirty() );
}
