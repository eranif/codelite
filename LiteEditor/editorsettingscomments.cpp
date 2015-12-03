//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingscomments.cpp
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

#include "editorsettingscomments.h"
#include "commentconfigdata.h"

EditorSettingsComments::EditorSettingsComments( wxWindow* parent )
    : EditorSettingsCommentsBase( parent )
    , TreeBookNode<EditorSettingsComments>()
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    m_checkBoxContCComment->SetValue( data.GetAddStarOnCComment() );
    m_checkBoxContinueCppComment->SetValue( data.GetContinueCppComment() );
    m_checkBoxSmartAddFiles->SetValue( EditorConfigST::Get()->GetOptions()->GetOptions() & OptionsConfig::Opt_SmartAddFiles );
    
    size_t flags = EditorConfigST::Get()->GetOptions()->GetOptions();
    
    if ( !(flags & (OptionsConfig::Opt_NavKey_Alt|OptionsConfig::Opt_NavKey_Control)) ) {
        flags = OptionsConfig::Opt_NavKey_Alt|OptionsConfig::Opt_NavKey_Control; // force the least-instrusive meta key default
    }
    
    m_checkBoxAlt->SetValue( flags & OptionsConfig::Opt_NavKey_Alt );
    m_checkBoxCtrl->SetValue( flags & OptionsConfig::Opt_NavKey_Control );
}

void EditorSettingsComments::Save(OptionsConfigPtr options)
{
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    data.SetAddStarOnCComment(m_checkBoxContCComment->IsChecked());
    data.SetContinueCppComment(m_checkBoxContinueCppComment->IsChecked());

    EditorConfigST::Get()->WriteObject(wxT("CommentConfigData"), &data);
    size_t flags = options->GetOptions();
    if( m_checkBoxSmartAddFiles->IsChecked() )
        flags |= OptionsConfig::Opt_SmartAddFiles;
    else
        flags &= ~OptionsConfig::Opt_SmartAddFiles;
    
    // clear the navigation key code
    flags &= ~(OptionsConfig::Opt_NavKey_Alt|OptionsConfig::Opt_NavKey_Control|OptionsConfig::Opt_NavKey_Shift);
   
    if( m_checkBoxCtrl->IsChecked() )
        flags |= OptionsConfig::Opt_NavKey_Control;
        
    if( m_checkBoxAlt->IsChecked() )
        flags |= OptionsConfig::Opt_NavKey_Alt;
    
    if ( !(flags & (OptionsConfig::Opt_NavKey_Alt|OptionsConfig::Opt_NavKey_Control)) ) {
        flags |= OptionsConfig::Opt_NavKey_Alt|OptionsConfig::Opt_NavKey_Control; // force the least-instrusive meta key default
    }
    
    options->SetOptions(flags);
}
