//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_resources_page.cpp
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

#include "ps_resources_page.h"
#include "macros.h"
#include "globals.h"

PSResourcesPage::PSResourcesPage( wxWindow* parent, ProjectSettingsDlg *dlg )
    : PSResourcesPageBase( parent )
    , m_dlg(dlg)
{
    ::wxPGPropertyBooleanUseCheckbox(m_pgMgr->GetGrid());
}

void PSResourcesPage::Load(BuildConfigPtr buildConf)
{
    m_pgPropResCmpOptions->SetValue(buildConf->GetResCompileOptions());
    m_pgPropResCmpSearchPath->SetValue(buildConf->GetResCmpIncludePath());
    SelectChoiceWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings, buildConf->GetBuildResWithGlobalSettings());
}

void PSResourcesPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetResCmpIncludePath( m_pgPropResCmpSearchPath->GetValueAsString() );
    buildConf->SetResCmpOptions( m_pgPropResCmpOptions->GetValueAsString() );
    buildConf->SetBuildResWithGlobalSettings(m_pgPropBehaviorWithGlobalSettings->GetValueAsString());
}

void PSResourcesPage::Clear()
{
    wxPropertyGridIterator iter = m_pgMgr->GetGrid()->GetIterator();
    for( ; !iter.AtEnd(); ++iter ) {
        if ( iter.GetProperty() && !iter.GetProperty()->IsCategory() ) {
            iter.GetProperty()->SetValueToUnspecified();
        }
    }
}

void PSResourcesPage::OnCustomEditorClicked(wxCommandEvent& event)
{
    m_dlg->SetIsDirty(true);
    wxPGProperty* prop = m_pgMgr->GetSelectedProperty();
    CHECK_PTR_RET(prop);

    if ( prop == m_pgPropResCmpOptions || prop == m_pgPropResCmpSearchPath ) {
        wxString v = prop->GetValueAsString();
        if ( PopupAddOptionDlg( v ) ) {
            prop->SetValue( v );
        }
    }
}

void PSResourcesPage::OnValueChanged(wxPropertyGridEvent& event)
{
    m_dlg->SetIsDirty(true);
}

void PSResourcesPage::OnResourcesEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_dlg->IsCustomBuildEnabled() );
}

void PSResourcesPage::OnProjectEnabledUI(wxUpdateUIEvent& event)
{
    event.Enable( m_dlg->IsProjectEnabled() );
}
