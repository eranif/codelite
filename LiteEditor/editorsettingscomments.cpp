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
	options->SetOptions(flags);
}
