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

#include "EditorOptionsGeneralEdit.h"
#include "EditorOptionsGeneralGuidesPanel.h"
#include "clTabRendererMinimal.hpp"
#include "editoroptionsgeneralindentationpanel.h"
#include "editoroptionsgeneralsavepanel.h"
#include "editorsettingsbookmarkspanel.h"
#include "editorsettingscaret.h"
#include "editorsettingscomments.h"
#include "editorsettingsdockingwidows.h"
#include "editorsettingsfolding.h"
#include "editorsettingsmiscpanel.h"
#include "editorsettingsterminal.h"
#include "frame.h"
#include "globals.h"
#include "manager.h"
#include "plugin.h"
#include "windowattrmanager.h"

#include <wx/persist.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>
#include <wx/wupdlock.h>

PreferencesDialog::PreferencesDialog(wxWindow* parent)
    : OptionsBaseDlg2(parent)
{
    Initialize();
    MSWSetWindowDarkTheme(this);
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
    EditorConfigST::Get()->SetOptions(m_options);

    // save the modifications to the disk
    EditorConfigST::Get()->Save();

    // Notify plugins about settings changed
    PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
}

void PreferencesDialog::Initialize()
{
    Freeze();
    m_options = EditorConfigST::Get()->GetOptions();

    AddPage(new EditorOptionsGeneralGuidesPanel(m_treeBook, m_options), _("Guides"), true);
    AddPage(new EditorOptionsGeneralEdit(m_treeBook, m_options), _("Edit"), false);
    AddPage(new EditorOptionsGeneralIndentationPanel(m_treeBook, m_options), _("Indentation"));
    AddPage(new EditorSettingsCaret(m_treeBook, m_options), _("Caret / Scrolling"));
    AddPage(new EditorOptionsGeneralSavePanel(m_treeBook, m_options), _("Save Options"));
    AddPage(new EditorSettingsComments(m_treeBook, m_options), _("Code"));
    AddPage(new EditorSettingsFolding(m_treeBook, m_options), _("Folding"));
    AddPage(new EditorSettingsDockingWindows(m_treeBook, m_options), _("Tabs"));
    AddPage(new EditorSettingsBookmarksPanel(m_treeBook, m_options), _("Bookmarks"));
    AddPage(new EditorSettingsTerminal(m_treeBook, m_options), _("Terminal"));
    AddPage(new EditorSettingsMiscPanel(m_treeBook, m_options), _("Misc"));
    for(size_t i = 0; i < m_treeBook->GetPageCount(); ++i) {
        OptionsConfigPage* p = dynamic_cast<OptionsConfigPage*>(m_treeBook->GetPage(i));
        if(p) {
            p->Finalize();
        }
    }
    Thaw();
    clSetDialogBestSizeAndPosition(this);
}
