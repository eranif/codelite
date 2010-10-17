//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsdialogs.cpp
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

#include "editorsettingsdialogs.h"

EditorSettingsDialogs::EditorSettingsDialogs( wxWindow* parent )
		: EditorSettingsDialogsBase( parent )
		, TreeBookNode<EditorSettingsDialogs>()
{
	long adjustCpuNumber(0);
	long replaceWrapAround(0);
	long findNextWrapAround(0);
	long buildBeforeDebug(0);
	long createSwappedFile(0);
	long reloadAlteredWorkspace(0);

	bAdjustCPUNumber = EditorConfigST::Get()->GetLongValue(wxT("AdjustCPUNumber"), adjustCpuNumber);
	bReplaceWrapAroundAnswer = EditorConfigST::Get()->GetLongValue(wxT("ReplaceWrapAroundAnswer"), replaceWrapAround);
	bFindNextWrapAroundAnswer = EditorConfigST::Get()->GetLongValue(wxT("FindNextWrapAroundAnswer"), findNextWrapAround);
	bBuildBeforeDebug = EditorConfigST::Get()->GetLongValue(wxT("BuildBeforeDebug"), buildBeforeDebug);
	bCreateSwappedFile = EditorConfigST::Get()->GetLongValue(wxT("CreateSwappedFile"), createSwappedFile);
	bReloadAlteredWorkspace = EditorConfigST::Get()->GetLongValue(wxT("ReloadWorkspaceWhenAltered"), reloadAlteredWorkspace);

	// the value stored is 0 / 1
	if (bAdjustCPUNumber) {
		AdjustCPUNumber_idx = m_checkListAnswers->Append(wxT("Adjust number of build processes according to CPU"));
		m_checkListAnswers->Check((unsigned int)AdjustCPUNumber_idx, adjustCpuNumber ? true : false);
	}

	// the value stored is wxID_OK / wxID_NO
	if (bReplaceWrapAroundAnswer) {
		ReplaceWrapAroundAnswer_idx = m_checkListAnswers->Append(wxT("Wrap around 'Replace' operation"));
		m_checkListAnswers->Check((unsigned int)ReplaceWrapAroundAnswer_idx, replaceWrapAround == wxID_OK);
	}

	// the value stored is wxID_OK / wxID_NO
	if (bFindNextWrapAroundAnswer) {
		FindNextWrapAroundAnswer_idx = m_checkListAnswers->Append(wxT("Wrap around 'Find' operation"));
		m_checkListAnswers->Check((unsigned int)FindNextWrapAroundAnswer_idx, findNextWrapAround == wxID_OK);
	}

	if (bBuildBeforeDebug) {
		BuildBeforeDebug_idx = m_checkListAnswers->Append(wxT("Always Build before debugging"));
		m_checkListAnswers->Check((unsigned int)BuildBeforeDebug_idx, buildBeforeDebug == wxID_OK);
	}

	if (bCreateSwappedFile) {
		CreateSwappedFile_idx = m_checkListAnswers->Append(wxT("If swapped file does not exist, create one"));
		m_checkListAnswers->Check((unsigned int)CreateSwappedFile_idx, createSwappedFile == wxID_OK);
	}

	// reloadAlteredWorkspace may be 0 (ask each time) 1 (never reload) or 2 (always reload)
	// Only show the item if it's currently set to one of the auto-responses, so it'll always start unticked
	if (bReloadAlteredWorkspace && (reloadAlteredWorkspace > 0)) {
		ReloadAlteredWorkspace_idx = m_checkListAnswers->Append(wxT("Always offer to Reload an externally-modified workspace"));
	} else {
		ReloadAlteredWorkspace_idx = wxNOT_FOUND;
	}

}

EditorSettingsDialogs::~EditorSettingsDialogs()
{

}

void EditorSettingsDialogs::Save(OptionsConfigPtr)
{
	if(bAdjustCPUNumber)
		EditorConfigST::Get()->SaveLongValue(wxT("AdjustCPUNumber"), m_checkListAnswers->IsChecked(AdjustCPUNumber_idx) ? 1 : 0);

	if(bReplaceWrapAroundAnswer)
		EditorConfigST::Get()->SaveLongValue(wxT("ReplaceWrapAroundAnswer"), m_checkListAnswers->IsChecked(ReplaceWrapAroundAnswer_idx) ? wxID_OK : wxID_NO);

	if(bFindNextWrapAroundAnswer)
		EditorConfigST::Get()->SaveLongValue(wxT("FindNextWrapAroundAnswer"), m_checkListAnswers->IsChecked(FindNextWrapAroundAnswer_idx) ? wxID_OK : wxID_NO);

	if(bBuildBeforeDebug)
		EditorConfigST::Get()->SaveLongValue(wxT("BuildBeforeDebug"), m_checkListAnswers->IsChecked(BuildBeforeDebug_idx) ? wxID_OK : wxID_NO);

	if(bCreateSwappedFile)
		EditorConfigST::Get()->SaveLongValue(wxT("CreateSwappedFile"), m_checkListAnswers->IsChecked(CreateSwappedFile_idx) ? wxID_OK : wxID_NO);

	// We only want to change this one if the box became unticked
	if(bReloadAlteredWorkspace && (ReloadAlteredWorkspace_idx != wxNOT_FOUND) && m_checkListAnswers->IsChecked(ReloadAlteredWorkspace_idx))
		EditorConfigST::Get()->SaveLongValue(wxT("ReloadWorkspaceWhenAltered"), 0);
}
