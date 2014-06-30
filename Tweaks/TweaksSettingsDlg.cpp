//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : TweaksSettingsDlg.cpp
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

#include "TweaksSettingsDlg.h"
#include "tweaks_settings.h"
#include "workspace.h"
#include <wx/choicdlg.h>
#include "windowattrmanager.h"
#include "macros.h"
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include "cl_defs.h"

TweaksSettingsDlg::TweaksSettingsDlg(wxWindow* parent)
    : TweaksSettingsDlgBase(parent)
{
    m_settings.Load();
    m_checkBoxEnableTweaks->SetValue( m_settings.IsEnableTweaks() );

    DoPopulateList();
    WindowAttrManager::Load(this, "TweaksSettingsDlg", NULL);
}

TweaksSettingsDlg::~TweaksSettingsDlg()
{
    WindowAttrManager::Save(this, "TweaksSettingsDlg", NULL);
    m_colourProperties.clear();
}

void TweaksSettingsDlg::OnWorkspaceOpenUI(wxUpdateUIEvent& event)
{
    event.Enable( WorkspaceST::Get()->IsOpen() );
}

void TweaksSettingsDlg::DoPopulateList()
{
    m_colourProperties.clear();
    // Get list of projects
    wxArrayString projects;
    WorkspaceST::Get()->GetProjectList( projects );

    // ----------------------------------------------------------------------
    // Add tab colours properties
    // ----------------------------------------------------------------------
    if ( m_settings.GetGlobalBgColour().IsOk() ) {
        wxVariant value;
        value << m_settings.GetGlobalBgColour();
        m_pgPropGlobalTabBG->SetValue( value );
    }

    if ( m_settings.GetGlobalFgColour().IsOk() ) {
        wxVariant value;
        value << m_settings.GetGlobalFgColour();
        m_pgPropGlobalTabFG->SetValue( value );
    }

    for(size_t i=0; i<projects.GetCount(); ++i) {
        const ProjectTweaks& pt = m_settings.GetProjectTweaks(projects.Item(i));
        // Image table
        m_pgMgr->Append( new wxFileProperty(projects.Item(i), wxPG_LABEL, pt.GetBitmapFilename()) );

        // Colours table
        wxPGProperty *parentProject = m_pgMgrTabColours->AppendIn(m_pgPropProjectsColours, new wxPropertyCategory(projects.Item(i)));
        wxString labelBG, labelFG;
        labelBG << projects.Item(i) << " tab label background colour";
        labelFG << projects.Item(i) << " tab label text colour";
        if ( pt.IsOk() ) {
            m_colourProperties.push_back( m_pgMgrTabColours->AppendIn(parentProject, new wxSystemColourProperty(labelBG, wxPG_LABEL, pt.GetTabBgColour() )) );
            m_colourProperties.push_back( m_pgMgrTabColours->AppendIn(parentProject, new wxSystemColourProperty(labelFG, wxPG_LABEL, pt.GetTabFgColour() )) );
            
        } else {
            wxPGProperty* prop(NULL);

            prop = m_pgMgrTabColours->AppendIn(parentProject, new wxSystemColourProperty(labelBG));
            prop->SetValueToUnspecified();
            m_colourProperties.push_back( prop );

            prop = m_pgMgrTabColours->AppendIn(parentProject, new wxSystemColourProperty(labelFG));
            prop->SetValueToUnspecified();
            m_colourProperties.push_back( prop );
        }
    }
}

void TweaksSettingsDlg::OnEnableTweaks(wxCommandEvent& event)
{
    m_settings.SetEnableTweaks( event.IsChecked() );
}

void TweaksSettingsDlg::OnEnableTweaksUI(wxUpdateUIEvent& event)
{
    event.Enable( m_checkBoxEnableTweaks->IsChecked() && WorkspaceST::Get()->IsOpen() );
}

void TweaksSettingsDlg::OnEnableTweaksCheckboxUI(wxUpdateUIEvent& event)
{
    event.Enable( WorkspaceST::Get()->IsOpen() );
}

void TweaksSettingsDlg::OnColourChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    wxPGProperty* prop = event.GetProperty();
    CHECK_PTR_RET(prop);

    if ( prop == m_pgPropGlobalTabBG ) {
        // Global tab bg colour was modified
        wxColourPropertyValue cpv;
        cpv << prop->GetValue();
        m_settings.SetGlobalBgColour( cpv.m_colour );

    } else if ( prop == m_pgPropGlobalTabFG ) {
        // Global tab colour was modified
        wxColourPropertyValue cpv;
        cpv << prop->GetValue();
        m_settings.SetGlobalFgColour( cpv.m_colour );

    } else if ( prop->GetParent() ) {
        // project specific colour was changed
        wxColourPropertyValue cpv;
        cpv << prop->GetValue();

        if ( prop->GetLabel().Contains("text colour") ) {
            m_settings.GetProjectTweaks(prop->GetParent()->GetLabel()).SetTabFgColour( cpv.m_colour );

        } else if ( prop->GetLabel().Contains("background colour") ) {
            m_settings.GetProjectTweaks(prop->GetParent()->GetLabel()).SetTabBgColour( cpv.m_colour );

        }
    }
}
void TweaksSettingsDlg::OnImageSelected(wxPropertyGridEvent& event)
{
    wxPGProperty* prop = event.GetProperty();
    CHECK_PTR_RET(prop);

    wxString projectName = prop->GetLabel();
    m_settings.GetProjectTweaks(projectName).SetBitmapFilename( prop->GetValueAsString() );

    if ( !m_settings.HasFlag( TweaksSettings::kDontPromptForProjectReload ) ) {
        wxRichMessageDialog dlg(this, _("Icon changes require a workspace reload"), "CodeLite", wxOK|wxOK_DEFAULT|wxCANCEL|wxICON_INFORMATION);
        dlg.ShowCheckBox(_("Remember my answer"));
        if ( dlg.ShowModal() == wxID_OK ) {
            if ( dlg.IsCheckBoxChecked() ) {
                m_settings.EnableFlag( TweaksSettings::kDontPromptForProjectReload, true );
            }
        }
    }
}
void TweaksSettingsDlg::OnResetColours(wxCommandEvent& event)
{
    wxUnusedVar( event );
    PropPtrList_t::iterator iter = m_colourProperties.begin();
    for( ; iter != m_colourProperties.end(); ++iter ) {
        (*iter)->SetValueToUnspecified();
    }
    m_pgPropGlobalTabBG->SetValueToUnspecified();
    m_pgPropGlobalTabFG->SetValueToUnspecified();
    m_settings.ResetColours();
}

void TweaksSettingsDlg::OnEnableColoursTableUI(wxUpdateUIEvent& event)
{
#if defined(__WXGTK__)
#if CL_USE_NATIVEBOOK
    // Under GTK, when native books are enabled, the tab coloring is not
    // available
    event.Enable(false);
    return;
#endif
#endif

    event.Enable( m_checkBoxEnableTweaks->IsChecked() && WorkspaceST::Get()->IsOpen() );
}
