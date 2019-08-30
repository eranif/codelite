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
#include "editorsettingsfolding.h"
#include "editorsettingsmiscpanel.h"
#include "manager.h"
#include "windowattrmanager.h"
#include "EditorOptionsGeneralEdit.h"
#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include <wx/wupdlock.h>

PreferencesDialog::PreferencesDialog(wxWindow* parent)
    : OptionsBaseDlg2(parent)
    , m_contentObjects()
    , restartRquired(false)
{
    Initialize();
    SetName("PreferencesDialog");
    WindowAttrManager::Load(this);
    MSWSetNativeTheme(m_treeBook->GetTreeCtrl());
    GetSizer()->Fit(this);
    GetSizer()->Layout();
    CenterOnParent();
    //::clSetDialogBestSizeAndPosition(this);
}

PreferencesDialog::~PreferencesDialog() {}

void PreferencesDialog::OnButtonOK(wxCommandEvent&)
{
    DoSave();
    EndModal(wxID_OK);
}

void PreferencesDialog::OnButtonCancel(wxCommandEvent&) { EndModal(wxID_CANCEL); }

void PreferencesDialog::OnButtonApply(wxCommandEvent&) { DoSave(); }

void PreferencesDialog::DoSave()
{
    // construct an OptionsConfig object and update the configuration
    OptionsConfigPtr options(new OptionsConfig(NULL));

    // for performance reasons, we start a transaction for the configuration
    // file
    EditorConfigST::Get()->Begin();
    typedef std::list<TreeBookNodeBase*>::iterator ContentIter;
    for(ContentIter it = m_contentObjects.begin(), end = m_contentObjects.end(); it != end; ++it) {
        if(*it) {
            TreeBookNodeBase* child = *it;
            child->Save(options);

            if(!this->restartRquired) {
                this->restartRquired = child->IsRestartRequired();
            }
        }
    }

    EditorConfigST::Get()->SetOptions(options);

    // save the modifications to the disk
    EditorConfigST::Get()->Save();

    // Notify plugins about settings changed
    PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
}

void PreferencesDialog::Initialize()
{
    AddPage(new EditorOptionsGeneralGuidesPanel(m_treeBook), _("Guides"), true);
    AddPage(new EditorOptionsGeneralEdit(m_treeBook), _("Edit"), false);
    AddPage(new EditorOptionsGeneralIndentationPanel(m_treeBook), _("Indentation"));
    AddPage(new EditorOptionsGeneralRightMarginPanel(m_treeBook), _("Right Margin Indicator"));
    AddPage(new EditorSettingsCaret(m_treeBook), _("Caret & Scrolling"));
    AddPage(new EditorOptionsGeneralSavePanel(m_treeBook), _("Save Options"));
    AddPage(new EditorSettingsComments(m_treeBook), _("Code"));
    AddPage(new EditorSettingsCommentsDoxygenPanel(m_treeBook), _("Documentation"));
    AddPage(new EditorSettingsFolding(m_treeBook), _("Folding"));
    AddPage(new EditorSettingsBookmarksPanel(m_treeBook), _("Bookmarks"));
    AddPage(new EditorSettingsDockingWindows(m_treeBook), _("Windows & Tabs"));

    // the Terminal page should NOT be added under Windows
    AddPage(new EditorSettingsTerminal(m_treeBook), _("Terminal"));
    AddPage(new EditorSettingsMiscPanel(m_treeBook), _("Misc"));
    SetMinSize(wxSize(300, 200));
}
