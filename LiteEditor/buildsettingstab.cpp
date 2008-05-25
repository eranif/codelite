//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : buildsettingstab.cpp              
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
 #include "frame.h"
#include "buidltab.h"
#include "buildsettingstab.h"
#include "buildtabsettingsdata.h"
#include "editor_config.h"

BuildTabSetting::BuildTabSetting( wxWindow* parent )
:
BuildTabSettingsBase( parent )
{ 
	BuildTabSettingsData options;
	EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);
	m_checkBoxSkipeWarnings->SetValue( options.GetSkipWarnings() );
	m_colourPickerError->SetColour(options.GetErrorColourBg());
	m_colourPickerWarnings->SetColour(options.GetWarnColourBg());
	m_colourPickerErrorFg->SetColour(options.GetErrorColour());
	m_colourPickerWarningsFg->SetColour(options.GetWarnColour());
	m_checkBoxBoldErrFont->SetValue(options.GetBoldErrFont());
	m_checkBoxBoldWarnFont->SetValue(options.GetBoldWarnFont());
}

void BuildTabSetting::Save()
{
	BuildTabSettingsData options;
	options.SetErrorColourBg(m_colourPickerError->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	options.SetWarnColourBg(m_colourPickerWarnings->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	options.SetErrorColour(m_colourPickerErrorFg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	options.SetWarnColour(m_colourPickerWarningsFg->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
	options.SetSkipWarnings(m_checkBoxSkipeWarnings->IsChecked());
	options.SetBoldErrFont(m_checkBoxBoldErrFont->IsChecked());
	options.SetBoldWarnFont(m_checkBoxBoldWarnFont->IsChecked());
	
	EditorConfigST::Get()->WriteObject(wxT("build_tab_settings"), &options);
	//reload the settings
	Frame::Get()->GetOutputPane()->GetBuildTab()->ReloadSettings();
}
