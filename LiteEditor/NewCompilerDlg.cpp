//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : NewCompilerDlg.cpp
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

#include "NewCompilerDlg.h"
#include "windowattrmanager.h"
#include "build_settings_config.h"

NewCompilerDlg::NewCompilerDlg(wxWindow* parent)
    : NewCompilerDlgBase(parent)
{
    BuildSettingsConfigCookie cookie;
    m_choiceCompilers->Append("<None>");
    CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
    while ( cmp ) {
        m_choiceCompilers->Append(cmp->GetName());
        cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
    }

    m_choiceCompilers->SetStringSelection("<None>");
    SetName("NewCompilerDlg");
    WindowAttrManager::Load(this);
}

void NewCompilerDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable( !m_textCtrlCompilerName->IsEmpty() );
}

wxString NewCompilerDlg::GetMasterCompiler() const
{
    wxString compilerName = m_choiceCompilers->GetStringSelection();
    if ( compilerName == "<None>" ) {
        return wxEmptyString;
    }
    return compilerName;
}
