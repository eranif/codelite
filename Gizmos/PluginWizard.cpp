//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : PluginWizard.cpp
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

#include "PluginWizard.h"
#include "globals.h"
#include <wx/msgdlg.h>
#include <wx/dir.h>

PluginWizard::PluginWizard(wxWindow* parent)
    : PluginWizardBase(parent)
{
}

PluginWizard::~PluginWizard()
{
}

bool PluginWizard::Run(NewPluginData& pd)
{
    bool res = RunWizard( GetFirstPage() );
    if ( res ) {
        pd.SetCodelitePath( m_dirPickerCodeliteDir->GetPath() );
        pd.SetPluginDescription( m_textCtrlDescription->GetValue() );
        pd.SetPluginName( m_textCtrlName->GetValue() );
        pd.SetProjectPath( m_textCtrlPreview->GetValue() );
    }
    return res;
}

void PluginWizard::OnFinish(wxWizardEvent& event)
{
    event.Skip();
}

void PluginWizard::OnPageChanging(wxWizardEvent& event)
{
    if ( event.GetDirection() && event.GetPage() == m_pages.at(0)) {
        wxString pluginName = m_textCtrlName->GetValue();
        pluginName.Trim();
        if ( pluginName.IsEmpty() || !::IsValidCppIdentifier(pluginName) ) {
            ::wxMessageBox(_("Invalid plugin name"), "codelite");
            event.Veto();
            return;
        }
    } else if ( event.GetDirection() && event.GetPage() == m_pages.at(1)) {
        if ( !wxDir::Exists( m_dirPickerCodeliteDir->GetPath() ) ) {
            ::wxMessageBox(_("codelite folder does not exists"), "codelite");
            event.Veto();
            return;
        }

        if ( !wxDir::Exists( m_dirPickerPluginPath->GetPath() ) ) {
            ::wxMessageBox(_("The selected plugin folder does not exist"), "codelite");
            event.Veto();
            return;
        }
    }
}
void PluginWizard::OnProjectPathChanged(wxFileDirPickerEvent& event)
{
    wxFileName project(event.GetPath(), m_textCtrlName->GetValue());
    project.SetExt("project");
    project.AppendDir(m_textCtrlName->GetValue());
    m_textCtrlPreview->ChangeValue( project.GetFullPath() );
}
