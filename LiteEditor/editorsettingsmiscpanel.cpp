//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsmiscpanel.cpp
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

#include "editorsettingsmiscpanel.h"
#include "generalinfo.h"
#include "frame.h"
#include "manager.h"
#include "pluginmanager.h"
#include "wx/wxprec.h"
#include <wx/fontmap.h>

EditorSettingsMiscPanel::EditorSettingsMiscPanel( wxWindow* parent )
		: EditorSettingsMiscBasePanel( parent )
		, TreeBookNode<EditorSettingsMiscPanel>()
		, m_restartRequired (false)
{
	GeneralInfo info = clMainFrame::Get()->GetFrameGeneralInfo();
	OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
	if (options->GetIconsSize() == 16) {
		m_toolbarIconSize->SetSelection(0);
	} else {
		m_toolbarIconSize->SetSelection(1);
	}

	m_useSingleToolbar->SetValue(!PluginManager::Get()->AllowToolbar());

	wxArrayString astrEncodings;
	wxFontEncoding fontEnc;
	int iCurrSelId = 0;
	size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
	for (size_t i = 0; i < iEncCnt; i++) {
		fontEnc = wxFontMapper::GetEncoding(i);
		if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
			continue;
		}
		astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
		if (fontEnc == options->GetFileFontEncoding()) {
			iCurrSelId = i;
		}
	}

	m_fileEncoding->Append(astrEncodings);
	m_fileEncoding->SetSelection(iCurrSelId);

	long single_instance(1);
	EditorConfigST::Get()->GetLongValue(wxT("SingleInstance"), single_instance);
	m_singleAppInstance->SetValue(single_instance ? true : false);

	long check(1);
	EditorConfigST::Get()->GetLongValue(wxT("CheckNewVersion"), check);
	m_versionCheckOnStartup->SetValue(check ? true : false);

	check = 1;
	EditorConfigST::Get()->GetLongValue(wxT("ShowFullPathInFrameTitle"), check);
	m_fullFilePath->SetValue(check ? true : false);

	bool showSplash = info.GetFlags() & CL_SHOW_SPLASH ? true : false;
	m_showSplashScreen->SetValue(showSplash);

	long max_items(10);
	EditorConfigST::Get()->GetLongValue(wxT("MaxItemsInFindReplaceDialog"), max_items);
	m_maxItemsFindReplace->SetValue(max_items);

	long maxTabs(15);
	EditorConfigST::Get()->GetLongValue(wxT("MaxOpenedTabs"), maxTabs);
	m_spinCtrlMaxOpenTabs->SetValue(maxTabs);
}

void EditorSettingsMiscPanel::OnClearButtonClick( wxCommandEvent& )
{
	ManagerST::Get()->ClearWorkspaceHistory();
	clMainFrame::Get()->GetMainBook()->ClearFileHistory();
}

void EditorSettingsMiscPanel::Save(OptionsConfigPtr options)
{

	if (m_showSplashScreen->IsChecked()) {
		clMainFrame::Get()->SetFrameFlag(true, CL_SHOW_SPLASH);
	} else {
		clMainFrame::Get()->SetFrameFlag(false, CL_SHOW_SPLASH);
	}

	EditorConfigST::Get()->SaveLongValue(wxT("SingleInstance"), m_singleAppInstance->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("CheckNewVersion"), m_versionCheckOnStartup->IsChecked() ? 1 : 0);
	EditorConfigST::Get()->SaveLongValue(wxT("ShowFullPathInFrameTitle"), m_fullFilePath->IsChecked() ? 1 : 0);

	bool oldUseSingleToolbar = !PluginManager::Get()->AllowToolbar();
	EditorConfigST::Get()->SaveLongValue(wxT("UseSingleToolbar"), m_useSingleToolbar->IsChecked() ? 1 : 0);

	int value = m_maxItemsFindReplace->GetValue();
	if (value < 1 || value > 50) {
		value = 10;
	}

	EditorConfigST::Get()->SaveLongValue(wxT("MaxItemsInFindReplaceDialog"), value);
	EditorConfigST::Get()->SaveLongValue(wxT("MaxOpenedTabs"), m_spinCtrlMaxOpenTabs->GetValue());

	//check to see of the icon size was modified
	int oldIconSize(24);

	OptionsConfigPtr oldOptions = EditorConfigST::Get()->GetOptions();
	if (oldOptions) {
		oldIconSize = oldOptions->GetIconsSize();
	}

	int iconSize(24);
	if (m_toolbarIconSize->GetSelection() == 0) {
		iconSize = 16;
	}
	options->SetIconsSize(iconSize);

	// save file font encoding
	options->SetFileFontEncoding(m_fileEncoding->GetStringSelection());

	if (oldIconSize != iconSize || oldUseSingleToolbar != m_useSingleToolbar->IsChecked()) {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 0);
		//notify the user
		m_restartRequired = true;
	} else {
		EditorConfigST::Get()->SaveLongValue(wxT("LoadSavedPrespective"), 1);
	}
}

void EditorSettingsMiscPanel::OnClearUI(wxUpdateUIEvent& e)
{
	wxArrayString a1, a2;
	clMainFrame::Get()->GetMainBook()->GetRecentlyOpenedFiles(a1);
	ManagerST::Get()->GetRecentlyOpenedWorkspaces(a2);
	e.Enable(!a1.IsEmpty() && !a2.IsEmpty());
}
