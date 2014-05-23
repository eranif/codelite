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
#include "compilerswitchespage.h"
#include "compilertoolspage.h"
#include "compilerpatternspage.h"
#include "compileradvancepage.h"
#include "compilerfiletypespage.h"
#include "compilercompileroptionspage.h"
#include "compilerlinkeroptionspage.h"
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
    m_compilersPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxHORIZONTAL );

    m_staticText1 = new wxStaticText( m_compilersPage, wxID_ANY, _("Create New Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_buttonNewCompiler = new wxButton( m_compilersPage, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_buttonNewCompiler, 0, wxALL|wxALIGN_RIGHT, 5 );

    m_buttonAutoDetect = new wxButton( m_compilersPage, wxID_ANY, _("Auto Detect Compilers..."), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( m_buttonAutoDetect, 0, wxALL|wxALIGN_RIGHT, 5 );

    bSizer5->Add( bSizer4, 0, wxALIGN_RIGHT|wxEXPAND, 5 );

    m_compilersNotebook = new wxTreebook(m_compilersPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
    bSizer5->Add( m_compilersNotebook, 1, wxALL|wxEXPAND, 5 );
    MSWSetNativeTheme(m_compilersNotebook->GetTreeCtrl());

    m_compilersPage->SetSizer( bSizer5 );
    m_compilersPage->Layout();
    bSizer5->Fit( m_compilersPage );
    m_notebook->AddPage( m_compilersPage, _("Compilers"), true );

    m_buildSettings = new BuildTabSetting(m_notebook);
    m_notebook->AddPage(m_buildSettings, _("Build Output Appearance"), false);

    m_buildPage = new BuildPage(m_notebook);
    m_notebook->AddPage(m_buildPage, _("Build Systems"), false);

    m_compilersNotebook->GetTreeCtrl()->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(AdvancedDlg::OnContextMenu), NULL, this);
    m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));

    LoadCompilers();

    if(m_compilersNotebook->GetPageCount()) {
        if(m_compilersNotebook->GetSelection() != wxNOT_FOUND) {
            m_compilersNotebook->ExpandNode(m_compilersNotebook->GetSelection());
        }
    }
    
    m_buttonNewCompiler->Bind(wxEVT_BUTTON, &AdvancedDlg::OnButtonNewClicked, this);
    m_buttonAutoDetect->Bind(wxEVT_BUTTON, &AdvancedDlg::OnAutoDetectCompilers, this);
    
    // center the dialog
    Centre();
    this->Layout();
    GetSizer()->Fit(this);
    m_compilersNotebook->SetFocus();
    WindowAttrManager::Load(this, wxT("BuildSettingsDlg"), NULL);
}

void AdvancedDlg::LoadCompilers()
{
    m_compilerPagesMap.clear();
    m_compilersNotebook->Freeze();
    m_compilersNotebook->DeleteAllPages();

    BuildSettingsConfigCookie cookie;

    wxString proj, conf;
    ManagerST::Get()->GetActiveProjectAndConf(proj, conf);

    wxString cmpType;
    BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjBuildConf(proj, conf);
    if(bldConf) {
        cmpType = bldConf->GetCompilerType();
    }

    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while (cmp) {
        AddCompiler(cmp, cmpType == cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }
    m_compilersNotebook->Thaw();
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

        if(m_compilersNotebook->GetPageCount() > ((m_compilerPagesMap.size() *6)-1) ) {
            int start_pos = (m_compilerPagesMap.size()-1)*6;
            m_compilersNotebook->ExpandNode(start_pos);
            m_compilersNotebook->SetSelection(start_pos + 1);
        }
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
    int sel = m_compilersNotebook->GetSelection();

    if (sel != wxNOT_FOUND) {
        wxString name = m_compilersNotebook->GetPageText((size_t)sel);
        if (DeleteCompiler(name)) {
            m_compilersNotebook->DeletePage((size_t)sel);

            std::map<wxString, std::vector<ICompilerSubPage*> >::iterator iter = m_compilerPagesMap.find(name);
            if(iter != m_compilerPagesMap.end()) {
                m_compilerPagesMap.erase(iter);
            }

            if(m_compilersNotebook->GetPageCount()) {
                m_compilersNotebook->ExpandNode(0);
                m_compilersNotebook->SetSelection(1);
            }
        }
    }
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

void AdvancedDlg::AddCompiler(CompilerPtr cmp, bool selected)
{
    std::vector<ICompilerSubPage*> pages;

    // add the root node
    m_compilersNotebook->AddPage(0, cmp->GetName(), selected);

    CompilerToolsPage *p3 = new CompilerToolsPage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p3);
    m_compilersNotebook->AddSubPage(p3, _("Tools"), selected);

    CompilerPatternsPage *p2 = new CompilerPatternsPage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p2);
    m_compilersNotebook->AddSubPage(p2, _("Patterns"), false);

    CompilerSwitchesPage *p4 = new CompilerSwitchesPage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p4);
    m_compilersNotebook->AddSubPage(p4, _("Switches"), false);

    CompilerFileTypePage *p5 = new CompilerFileTypePage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p5);
    m_compilersNotebook->AddSubPage(p5, _("File Types"), false);

    CompilerCompilerOptionsPage *p6 = new CompilerCompilerOptionsPage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p6);
    m_compilersNotebook->AddSubPage(p6, _("Compiler options"), false);

    CompilerLinkerOptionsPage *p7 = new CompilerLinkerOptionsPage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p7);
    m_compilersNotebook->AddSubPage(p7, _("Linker options"), false);

    CompilerAdvancePage *p8 = new CompilerAdvancePage(m_compilersNotebook, cmp->GetName());
    pages.push_back(p8);
    m_compilersNotebook->AddSubPage(p8, _("Advanced"), false);

    m_compilerPagesMap[cmp->GetName()] = pages;
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
    wxTreeCtrl *tree = m_compilersNotebook->GetTreeCtrl();
    wxTreeItemId item = tree->GetSelection();

    // only compilers have children
    if(item.IsOk() && tree->HasChildren(item)) {
        PopupMenu(m_rightclickMenu);
    }
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
