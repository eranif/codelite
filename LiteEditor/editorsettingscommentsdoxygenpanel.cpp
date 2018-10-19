//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingscommentsdoxygenpanel.cpp
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

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"
#include "commentconfigdata.h"
#include "editorsettingscommentsdoxygenpanel.h"
#include "globals.h"
#include "macros.h"

EditorSettingsCommentsDoxygenPanel::EditorSettingsCommentsDoxygenPanel(wxWindow* parent)
    : EditorSettingsCommentsDoxygenPanelBase(parent)
    , TreeBookNode<EditorSettingsCommentsDoxygenPanel>()
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);
    m_checkBoxQtStyle->SetValue(data.IsUseQtStyle());
    m_checkBoxAutoInsert->SetValue(data.IsAutoInsert());
    m_stcClassTemplate->SetText(data.GetClassPattern());
    m_stcFunctionTemplate->SetText(data.GetFunctionPattern());
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(lexer) {
        lexer->Apply(m_stcClassTemplate);
        lexer->Apply(m_stcFunctionTemplate);
    }
}

void EditorSettingsCommentsDoxygenPanel::Save(OptionsConfigPtr)
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    wxString classPattern = m_stcClassTemplate->GetText();
    wxString funcPattern = m_stcFunctionTemplate->GetText();
    classPattern.Replace("\\n", "\n");
    funcPattern.Replace("\\n", "\n");

    data.SetClassPattern(classPattern);
    data.SetFunctionPattern(funcPattern);
    data.SetAutoInsert(m_checkBoxAutoInsert->IsChecked());
    data.SetUseQtStyle(m_checkBoxQtStyle->IsChecked());
    EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
}
