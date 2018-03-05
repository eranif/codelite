//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : build_page.cpp
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

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif // WX_PRECOMP

#include "build_page.h"
#include "build_settings_config.h"
#include "editor_config.h"
#include "buildmanager.h"

///////////////////////////////////////////////////////////////////////////

BuildPage::BuildPage(wxWindow* parent, int id, wxPoint pos, wxSize size, int style)
    : wxPanel(parent, id, pos, size, style)
{
    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    m_staticText = new wxStaticText(this, wxID_ANY, _("Available Build Systems:"), wxDefaultPosition, wxDefaultSize, 0);
    mainSizer->Add(m_staticText, 0, wxALL, 5);

    m_bookBuildSystems = new wxChoicebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT);
    mainSizer->Add(m_bookBuildSystems, 0, wxEXPAND | wxALL, 5);

    m_fixOnStartup = new wxCheckBox(this, wxID_ANY, _("Fix build tool path on startup"));
    mainSizer->Add(m_fixOnStartup, 0, wxEXPAND | wxALL, 5);

    // m_generateAsteriskCleanTarget =
    //    new wxCheckBox(this, wxID_ANY, _("Use asterisk (*) for the clean target (e.g. rm -f *.o)"));
    // mainSizer->Add(m_generateAsteriskCleanTarget, 0, wxEXPAND | wxALL, 5);

    long fix = EditorConfigST::Get()->GetInteger(wxT("FixBuildToolOnStartup"), 1);
    m_fixOnStartup->SetValue(fix ? true : false);

    // long asterisk = EditorConfigST::Get()->GetInteger(wxT("CleanTragetWithAsterisk"), 0);
    // m_generateAsteriskCleanTarget->SetValue(asterisk ? true : false);

    this->SetSizer(mainSizer);
    this->Layout();
    CustomInit();
}

void BuildPage::CustomInit()
{
    // rest of builders list
    std::list<wxString> builders;
    BuildManagerST::Get()->GetBuilders(builders);

    std::list<wxString>::iterator iter = builders.begin();
    for(; iter != builders.end(); iter++) {
        BuilderPtr builder = BuildManagerST::Get()->GetBuilder(*iter);
        m_bookBuildSystems->AddPage(CreateBuildSystemPage(*iter), *iter, builder->IsActive());
    }
}

wxPanel* BuildPage::CreateBuildSystemPage(const wxString& name)
{
    return new BuildSystemPage(m_bookBuildSystems, name);
}

void BuildPage::Save()
{
    EditorConfigST::Get()->SetInteger(wxT("FixBuildToolOnStartup"), m_fixOnStartup->IsChecked() ? 1 : 0);
    // EditorConfigST::Get()->SetInteger(wxT("CleanTragetWithAsterisk"),
    //                                  m_generateAsteriskCleanTarget->IsChecked() ? 1 : 0);

    // Save current page displayed as 'selected' builder
    int sel = (int)m_bookBuildSystems->GetSelection();

    // clLogMessage(wxString::Format( wxT("selection:%d"), sel ));
    BuildSystemPage* page = dynamic_cast<BuildSystemPage*>(m_bookBuildSystems->GetPage(sel));
    if(page) {
        page->SetSelected();
    }

    int count = (int)m_bookBuildSystems->GetPageCount();
    for(int i = 0; i < count; i++) {
        BuildSystemPage* page = dynamic_cast<BuildSystemPage*>(m_bookBuildSystems->GetPage(i));
        if(page) {
            page->Save();
        }
    }
}

//---------------------------------------------------------------
// Build system page
//---------------------------------------------------------------
BuildSystemPage::BuildSystemPage(wxWindow* parent, wxString name)
    : wxPanel(parent)
    , m_name(name)
{
    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(bSizer6);
    this->Layout();
}

void BuildSystemPage::Save()
{
    // update cached builders
    BuilderPtr builder = BuildManagerST::Get()->GetBuilder(m_name);
    BuildManagerST::Get()->AddBuilder(builder);

    // Save the configuration
    BuildSettingsConfigST::Get()->SaveBuilderConfig(builder);
}

void BuildSystemPage::SetSelected()
{
    BuilderPtr builder = BuildManagerST::Get()->GetBuilder(m_name);
    builder->SetActive();
}
