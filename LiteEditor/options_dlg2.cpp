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
#include "globals.h"

#include "frame.h"
#include "editor_options_general_guides_panel.h"
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
#include "windowattrmanager.h"

OptionsDlg2::OptionsDlg2( wxWindow* parent )
    : OptionsBaseDlg2( parent )
    , m_contentObjects()
    , restartRquired(false)
{
    Initialize();
    WindowAttrManager::Load(this, wxT("OptionsDlgAttr"), NULL);
    MSWSetNativeTheme( m_treeBook->GetTreeCtrl() );
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

    clMainFrame::Get()->GetMainBook()->ApplySettingsChanges();

    // Notify plugins about settings changed
    PostCmdEvent( wxEVT_EDITOR_SETTINGS_CHANGED );

    if ( this->restartRquired ) {
        WindowAttrManager::Save(this, wxT("OptionsDlgAttr"), NULL);
    }
}

void OptionsDlg2::Initialize()
{
    m_treeBook->AddPage(0,_("General"));
    AddSubPage(new EditorOptionsGeneralGuidesPanel(m_treeBook),     _("Guides"), true);
    AddSubPage(new EditorOptionsGeneralIndentationPanel(m_treeBook),_("Indentation"));
    AddSubPage(new EditorOptionsGeneralRightMarginPanel(m_treeBook),_("Right Margin Indicator"));
    AddSubPage(new EditorSettingsCaret(m_treeBook),                 _("Caret & Scrolling"));
    AddSubPage(new EditorOptionsGeneralSavePanel(m_treeBook),       _("Save Options"));

    m_treeBook->AddPage(0, wxT("C++"));
    AddSubPage(new EditorSettingsComments(m_treeBook),            _("Code Generation & Comments"));
    AddSubPage(new EditorSettingsCommentsDoxygenPanel(m_treeBook), wxT("Doxygen"));

    AddPage(new EditorSettingsFolding(m_treeBook),       _("Folding"));
    AddPage(new EditorSettingsBookmarksPanel(m_treeBook),_("Bookmarks & Breakpoints"));
    AddPage(new EditorSettingsDialogs(m_treeBook),       _("Dialogs"));
    AddPage(new EditorSettingsDockingWindows(m_treeBook),_("Windows & Tabs"));

#ifndef __WXMSW__
    // the Terminal page should NOT be added under Windows
    AddPage(new EditorSettingsTerminal(m_treeBook),      _("Terminal"));
#endif
    AddPage(new EditorSettingsMiscPanel(m_treeBook),     _("Misc"));
}
