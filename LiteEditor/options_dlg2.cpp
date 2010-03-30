//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : options_dlg2.cpp
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

#include "options_dlg2.h"
#include "plugin.h"
#include "editorsettingsdockingwidows.h"
#include "editorsettingsterminal.h"
#include "editorsettingscaret.h"

#include "frame.h"
#include "editor_options_general_guides_panel.h"
#include "editoroptionsgeneralcodenavpanel.h"
#include "editoroptionsgeneralindentationpanel.h"
#include "editoroptionsgeneralrightmarginpanel.h"
#include "editoroptionsgeneralsavepanel.h"
#include "globals.h"

#include "editorsettingscomments.h"
#include "editorsettingscommentsdoxygenpanel.h"
#include "editorsettingsbookmarkspanel.h"
#include "editorsettingsdialogs.h"
#include "editorsettingsfolding.h"
#include "editorsettingsmiscpanel.h"
#include "manager.h"
#include "../Plugin/windowattrmanager.h"

OptionsDlg2::OptionsDlg2( wxWindow* parent )
		: OptionsBaseDlg2( parent )
		, m_contentObjects()
		, restartRquired(false)
{
	Initialize();
	WindowAttrManager::Load(this, wxT("OptionsDlgAttr"), NULL);
	GetSizer()->Layout();
}

OptionsDlg2::~OptionsDlg2()
{
	if ( !this->restartRquired )
		WindowAttrManager::Save(this, wxT("OptionsDlgAttr"), NULL);
}

void OptionsDlg2::OnButtonOK( wxCommandEvent&)
{
	DoSave();
	EndModal(wxID_OK);
}

void OptionsDlg2::OnButtonCancel( wxCommandEvent&)
{
	EndModal(wxID_CANCEL);
}

void OptionsDlg2::OnButtonApply( wxCommandEvent& )
{
	DoSave();
}

void OptionsDlg2::DoSave()
{
	// construct an OptionsConfig object and update the configuration
	OptionsConfigPtr options(new OptionsConfig(NULL));

	// for performance reasons, we start a transaction for the configuration
	// file
	EditorConfigST::Get()->Begin();
	typedef std::list<TreeBookNodeBase*>::iterator ContentIter;
	for (ContentIter it = m_contentObjects.begin(), end = m_contentObjects.end(); it != end; ++it) {
		if (*it) {
			TreeBookNodeBase* child = *it;
			child->Save( options );

			if ( !this->restartRquired ) {
				this->restartRquired = child->IsRestartRequired();
			}
		}
	}

	EditorConfigST::Get()->SetOptions(options);

	// save the modifications to the disk
	EditorConfigST::Get()->Save();

	Frame::Get()->GetMainBook()->ApplySettingsChanges();

	// Notify plugins about settings changed
	PostCmdEvent( wxEVT_EDITOR_SETTINGS_CHANGED );

	if ( this->restartRquired ) {
		WindowAttrManager::Save(this, wxT("OptionsDlgAttr"), NULL);
	}
}

void OptionsDlg2::Initialize()
{
	m_treeBook->AddPage(0, wxT("General"));
	AddSubPage(new EditorOptionsGeneralGuidesPanel(m_treeBook),      wxT("Guides"), true);
	AddSubPage(new EditorOptionsGeneralIndentationPanel(m_treeBook), wxT("Indentation"));
	AddSubPage(new EditorOptionsGeneralRightMarginPanel(m_treeBook), wxT("Right Margin Indicator"));
	AddSubPage(new EditorSettingsCaret(m_treeBook),                  wxT("Caret"));
	AddSubPage(new EditorOptionsGeneralSavePanel(m_treeBook),        wxT("Save Options"));

	m_treeBook->AddPage(0, wxT("C++"));
	AddSubPage(new EditorSettingsComments(m_treeBook),             wxT("Comments"));
	AddSubPage(new EditorSettingsCommentsDoxygenPanel(m_treeBook), wxT("Doxygen"));
	AddSubPage(new EditorOptionsGeneralCodeNavPanel(m_treeBook),   wxT("Quick Code Navigation"));
	
	AddPage(new EditorSettingsFolding(m_treeBook),        wxT("Folding"));
	AddPage(new EditorSettingsBookmarksPanel(m_treeBook), wxT("Bookmarks & Breakpoints"));
	AddPage(new EditorSettingsDialogs(m_treeBook),        wxT("Dialogs"));
	AddPage(new EditorSettingsDockingWindows(m_treeBook), wxT("Docking Windows"));

#ifndef __WXMSW__
	// the Terminal page should NOT be added under Windows
	AddPage(new EditorSettingsTerminal(m_treeBook),       wxT("Terminal"));
#endif
	AddPage(new EditorSettingsMiscPanel(m_treeBook),      wxT("Misc"));
}
