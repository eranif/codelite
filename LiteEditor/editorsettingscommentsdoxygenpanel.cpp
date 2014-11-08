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

#include "editorsettingscommentsdoxygenpanel.h"
#include "commentconfigdata.h"

EditorSettingsCommentsDoxygenPanel::EditorSettingsCommentsDoxygenPanel(wxWindow* parent)
    : EditorSettingsCommentsDoxygenPanelBase(parent)
    , TreeBookNode<EditorSettingsCommentsDoxygenPanel>()
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);
    m_pgPropDoxyClassPrefix->SetValueFromString(data.GetClassPattern());
    m_pgPropDoxyFunctionPrefix->SetValueFromString(data.GetFunctionPattern());
    m_pgPropAutoGen->SetValue(data.IsAutoInsertAfterSlash2Stars());
}

void EditorSettingsCommentsDoxygenPanel::Save(OptionsConfigPtr)
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    wxString classPattern = m_pgPropDoxyClassPrefix->GetValue().GetString();
    wxString funcPattern = m_pgPropDoxyFunctionPrefix->GetValue().GetString();
    classPattern.Replace("\\n", "\n");
    funcPattern.Replace("\\n", "\n");
    
    data.SetClassPattern(classPattern);
    data.SetFunctionPattern(funcPattern);
    data.SetAutoInsertAfterSlash2Stars(m_pgPropAutoGen->GetValue().GetBool());
    EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
}
