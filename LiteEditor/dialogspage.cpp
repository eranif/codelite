//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dialogspage.cpp              
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

#include "editor_config.h"
#include "dialogspage.h"

DialogsPage::DialogsPage( wxWindow* parent )
:
DialogsPageBase( parent )
{
	Initialize();
}

void DialogsPage::Initialize()
{
	long adjustCpuNumber(0);
	long replaceWrapAround(0);
	long findNextWrapAround(0);
	long buildBeforeDebug(0);
	
	EditorConfigST::Get()->GetLongValue(wxT("AdjustCPUNumber"), adjustCpuNumber);
	EditorConfigST::Get()->GetLongValue(wxT("ReplaceWrapAroundAnswer"), replaceWrapAround);
	EditorConfigST::Get()->GetLongValue(wxT("FindNextWrapAroundAnswer"), findNextWrapAround);
	EditorConfigST::Get()->GetLongValue(wxT("BuildBeforeDebug"), buildBeforeDebug);
	
	int idx;
	
	// the value stored is 0 / 1
	idx = m_checkListAnswers->Append(wxT("Adjust number of build processes according to CPU"));
	m_checkListAnswers->Check((unsigned int)idx, adjustCpuNumber ? true : false);
	
	// the value stored is wxID_OK / wxID_NO
	idx = m_checkListAnswers->Append(wxT("Wrap around 'Replace' operation"));
	m_checkListAnswers->Check((unsigned int)idx, replaceWrapAround == wxID_OK);
	
	// the value stored is wxID_OK / wxID_NO
	idx = m_checkListAnswers->Append(wxT("Wrap around 'Find' operation"));
	m_checkListAnswers->Check((unsigned int)idx, findNextWrapAround == wxID_OK);
	
	idx = m_checkListAnswers->Append(wxT("Always Build before debugging"));
	m_checkListAnswers->Check((unsigned int)idx, buildBeforeDebug == wxID_OK);
}

void DialogsPage::Save()
{
	EditorConfigST::Get()->SaveLongValue(wxT("AdjustCPUNumber"), m_checkListAnswers->IsChecked(0) ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), m_checkListAnswers->IsChecked(1) ? wxID_OK : wxID_NO);
	EditorConfigST::Get()->SaveLongValue(wxT("FindNextWrapAroundAnswer"), m_checkListAnswers->IsChecked(2) ? wxID_OK : wxID_NO);
	EditorConfigST::Get()->SaveLongValue(wxT("BuildBeforeDebug"), m_checkListAnswers->IsChecked(3) ? wxID_OK : wxID_NO);
}
