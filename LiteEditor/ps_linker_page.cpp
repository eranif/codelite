//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : ps_linker_page.cpp
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

#include "ps_linker_page.h"
#include "compiler.h"
#include "build_settings_config.h"
#include "ps_general_page.h"

PSLinkerPage::PSLinkerPage( wxWindow* parent, ProjectSettingsDlg *dlg, PSGeneralPage *gp )
    : PSLinkPageBase( parent )
    , m_dlg(dlg)
    , m_gp(gp)
{
    m_pgMgr->GetGrid()->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);
}

void PSLinkerPage::OnLinkerNotNeededUI( wxUpdateUIEvent& event )
{
    event.Enable( !m_checkLinkerNeeded->IsChecked() && !m_dlg->IsCustomBuildEnabled() );
}

void PSLinkerPage::Load(BuildConfigPtr buildConf)
{
    m_checkLinkerNeeded->SetValue( !buildConf->IsLinkerRequired() );
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildLnkWithGlobalSettings());
    m_pgPropLibraries->SetValue( buildConf->GetLibraries() );
    m_pgPropLibraryPaths->SetValue( buildConf->GetLibPath() );
    m_pgPropOptions->SetValue( buildConf->GetLinkOptions() );
}

void PSLinkerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetLinkerRequired(!m_checkLinkerNeeded->IsChecked());
    buildConf->SetLibPath( m_pgPropLibraryPaths->GetValueAsString() );
    buildConf->SetLibraries( m_pgPropLibraries->GetValueAsString() );
    buildConf->SetLinkOptions( m_pgPropOptions->GetValueAsString() );
    buildConf->SetBuildLnkWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
}

void PSLinkerPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
    m_checkLinkerNeeded->SetValue(false);
}

void PSLinkerPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);
    m_dlg->SetIsDirty(true);

    if ( prop == m_pgPropLibraries || prop == m_pgPropLibraryPaths ) {
        wxString value = prop->GetValueAsString();
        if ( PopupAddOptionDlg(value) ) {
            prop->SetValueFromString( value );
        }

    } else if ( prop == m_pgPropOptions ) {
        wxString value = prop->GetValueAsString();
        wxString cmpName = m_gp->GetCompiler();
        CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpName);
        if (PopupAddOptionCheckDlg(value, _("Linker options"), cmp->GetLinkerOptions())) {
            prop->SetValueFromString( value );
        }
    }
}
void PSLinkerPage::OnCheckLinkerNeeded(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSLinkerPage::OnProjectCustumBuildUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}

void PSLinkerPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}

void PSLinkerPage::OnPropertyChanged(wxPropertyGridEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}
