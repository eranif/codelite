//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : spellcheck.cpp
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

/////////////////////////////////////////////////////////////////////////////
// Name:        spellcheck.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: spellcheck.cpp 36 2014-02-23 16:41:03Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#include "spellcheck.h"

#include "IHunSpell.h"
#include "SpellCheckerSettings.h"
#include "clToolBarButtonBase.h"
#include "ctags_manager.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"
#include "scGlobals.h"
#include "workspace.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/mstream.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

// clang-format off
#include "res/spellcheck16.b2c"
#include "res/spellcheck22.b2c"
//#include "res/contCheck16.b2c"
//#include "res/contCheck22.b2c"
// clang-format on

namespace
{

SpellCheck* thePlugin = NULL;

constexpr size_t maxSuggestions = 15;

const int SPC_IGNORE_WORD = XRCID("spellcheck_ignore_word");
const int SPC_ADD_WORD = XRCID("spellcheck_add_word");
const int SPC_SUGGESTION_ID = wxWindow::NewControlId(maxSuggestions);
const int IDM_SETTINGS = XRCID("spellcheck_settings");

constexpr int PARSE_TIME = 500;

} // namespace

// ------------------------------------------------------------
// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new SpellCheck(manager);
    }
    return thePlugin;
}
// ------------------------------------------------------------
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor("Frank Lichtner");
    info.SetName("SpellCheck");
    info.SetDescription(_("CodeLite spell checker"));
    info.SetVersion("v1.6");
    return &info;
}
// ------------------------------------------------------------
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }
// ------------------------------------------------------------
SpellCheck::SpellCheck(IManager* manager)
    : IPlugin(manager)
    , m_pLastEditor(nullptr)
{
    Init();
}
// ------------------------------------------------------------
SpellCheck::~SpellCheck()
{
    m_timer.Unbind(wxEVT_TIMER, &SpellCheck::OnTimer, this);

    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnSettings, this, IDM_SETTINGS);
    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnCheck, this, XRCID(s_doCheckID.ToUTF8()));
    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnContinousCheck, this, XRCID(s_contCheckID.ToUTF8()));
    m_topWin->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &SpellCheck::OnContextMenu, this);
    m_topWin->Unbind(wxEVT_WORKSPACE_LOADED, &SpellCheck::OnWspLoaded, this);
    m_topWin->Unbind(wxEVT_WORKSPACE_CLOSED, &SpellCheck::OnWspClosed, this);

    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnSuggestion, this, SPC_SUGGESTION_ID,
                     SPC_SUGGESTION_ID + maxSuggestions - 1);
    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnAddWord, this, SPC_ADD_WORD);
    m_topWin->Unbind(wxEVT_MENU, &SpellCheck::OnIgnoreWord, this, SPC_IGNORE_WORD);

    if(m_pEngine != NULL) {
        SaveSettings();
        wxDELETE(m_pEngine);
    }
}

// ------------------------------------------------------------
void SpellCheck::Init()
{
    m_topWin = NULL;
    m_pEngine = NULL;
    m_longName = _("CodeLite spell-checker");
    m_shortName = s_plugName;
    m_sepItem = NULL;
    m_topWin = wxTheApp;
    m_pEngine = new IHunSpell();
    m_currentWspPath = wxEmptyString;

    if(m_pEngine) {
        LoadSettings();
        wxString userDictPath = clStandardPaths::Get().GetUserDataDir();
        userDictPath << wxFILE_SEP_PATH << "spellcheck" << wxFILE_SEP_PATH;

        if(!wxFileName::DirExists(userDictPath)) {
            wxFileName::Mkdir(userDictPath);
        }

        m_pEngine->SetUserDictPath(userDictPath);
        m_pEngine->SetPlugIn(this);

        if(!m_options.GetDictionaryFileName().IsEmpty()) {
            m_pEngine->InitEngine();
        }
    }

    m_timer.Bind(wxEVT_TIMER, &SpellCheck::OnTimer, this);
    m_topWin->Bind(wxEVT_CONTEXT_MENU_EDITOR, &SpellCheck::OnContextMenu, this);
    m_topWin->Bind(wxEVT_WORKSPACE_LOADED, &SpellCheck::OnWspLoaded, this);
    m_topWin->Bind(wxEVT_WORKSPACE_CLOSED, &SpellCheck::OnWspClosed, this);

    m_topWin->Bind(wxEVT_MENU, &SpellCheck::OnSuggestion, this, SPC_SUGGESTION_ID,
                   SPC_SUGGESTION_ID + maxSuggestions - 1);
    m_topWin->Bind(wxEVT_MENU, &SpellCheck::OnAddWord, this, SPC_ADD_WORD);
    m_topWin->Bind(wxEVT_MENU, &SpellCheck::OnIgnoreWord, this, SPC_IGNORE_WORD);
}
// ------------------------------------------------------------
void SpellCheck::CreateToolBar(clToolBar* toolbar)
{
    int size = m_mgr->GetToolbarIconSize();
    auto images = toolbar->GetBitmapsCreateIfNeeded();
    toolbar->AddSpacer();
    toolbar->AddTool(XRCID(s_doCheckID.ToUTF8()), _("Check spelling..."), images->Add("spellcheck", size),
                     _("Run spell-checker"));
    toolbar->AddToggleButton(XRCID(s_contCheckID.ToUTF8()), images->Add("repeat", size), _("Check continuous"));
    wxTheApp->Bind(wxEVT_MENU, &SpellCheck::OnCheck, this, XRCID(s_doCheckID.ToUTF8()));
    wxTheApp->Bind(wxEVT_MENU, &SpellCheck::OnContinousCheck, this, XRCID(s_contCheckID.ToUTF8()));
    SetCheckContinuous(GetCheckContinuous());
}

// ------------------------------------------------------------
void SpellCheck::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* pMenu = new wxMenu();
    wxMenuItem* pItem(NULL);

    pItem = new wxMenuItem(pMenu, IDM_SETTINGS, _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    pMenu->Append(pItem);
    pluginsMenu->Append(wxID_ANY, _("SpellCheck"), pMenu);

    m_topWin->Bind(wxEVT_MENU, &SpellCheck::OnSettings, this, IDM_SETTINGS);
}
// ------------------------------------------------------------
void SpellCheck::OnContextMenu(clContextMenuEvent& e)
{
    const auto editor = GetEditor();
    const auto menu = e.GetMenu();
    if(!editor || !menu) {
        return;
    }

    const auto subMenuName(_("Spell Checker"));
    auto pSubMenu(new wxMenu);

    wxPoint pt = wxGetMousePosition();
    pt = editor->GetCtrl()->ScreenToClient(pt);
    const int pos = editor->GetCtrl()->PositionFromPoint(pt);

    if(editor->GetCtrl()->IndicatorValueAt(3, pos) == 1) {
        m_pLastEditor = nullptr;

        int start = editor->WordStartPos(pos, true);
        editor->SelectText(start, editor->WordEndPos(pos, true) - start);
        wxString sel = editor->GetSelection();
        wxArrayString sugg = m_pEngine->GetSuggestions(sel);

        for(size_t i = 0; i < std::min<>(sugg.GetCount(), size_t(maxSuggestions)); i++) {
            pSubMenu->Append(SPC_SUGGESTION_ID + i, sugg[i], "");
        }

        if(sugg.GetCount() != 0) {
            pSubMenu->AppendSeparator();
        }

        pSubMenu->Append(SPC_IGNORE_WORD, _("Ignore"), "");
        pSubMenu->Append(SPC_ADD_WORD, _("Add"), "");

        pSubMenu->AppendSeparator();
        AppendSubMenuItems(*pSubMenu);

        // Place at top of context menu for ease of access.
        menu->PrependSeparator();
        menu->Prepend(wxID_ANY, subMenuName, pSubMenu);
    } else {
        AppendSubMenuItems(*pSubMenu);
        menu->Append(wxID_ANY, subMenuName, pSubMenu);
    }
}
// ------------------------------------------------------------
void SpellCheck::AppendSubMenuItems(wxMenu& subMenu)
{
    subMenu.Append(XRCID(s_doCheckID.ToUTF8()), _("Check..."), _("Check..."));
    subMenu.Append(XRCID(s_contCheckID.ToUTF8()), _("Check continuous"), _("Start continuous check"), wxITEM_CHECK);
    subMenu.Check(XRCID(s_contCheckID.ToUTF8()), GetCheckContinuous());
    subMenu.Append(IDM_SETTINGS, _("Settings..."), _("Settings..."), wxITEM_NORMAL);
}
// ------------------------------------------------------------
void SpellCheck::UnPlug()
{
    if(m_timer.IsRunning()) {
        m_timer.Stop();
    }
}

// ------------------------------------------------------------
// returns pointer to editor
IEditor* SpellCheck::GetEditor()
{
    IEditor* editor = m_mgr->GetActiveEditor();

    if(!editor) {
        ::wxMessageBox(::wxGetTranslation(s_noEditor), s_codeLite, wxICON_WARNING | wxOK);
        return NULL;
    }
    return editor;
}
// ------------------------------------------------------------
void SpellCheck::OnSettings(wxCommandEvent& e)
{
    m_pLastEditor = nullptr;

    SpellCheckerSettings dlg(m_mgr->GetTheApp()->GetTopWindow());
    dlg.SetHunspell(m_pEngine);
    dlg.SetScanStrings(m_pEngine->IsScannerType(IHunSpell::kString));
    dlg.SetScanCPP(m_pEngine->IsScannerType(IHunSpell::kCppComment));
    dlg.SetScanC(m_pEngine->IsScannerType(IHunSpell::kCComment));
    dlg.SetScanD1(m_pEngine->IsScannerType(IHunSpell::kDox1));
    dlg.SetScanD2(m_pEngine->IsScannerType(IHunSpell::kDox2));
    dlg.SetDictionaryFileName(m_pEngine->GetDictionary());
    dlg.SetDictionaryPath(m_pEngine->GetDictionaryPath());
    dlg.SetCaseSensitiveUserDictionary(m_pEngine->GetCaseSensitiveUserDictionary());
    dlg.SetIgnoreSymbolsInTagsDatabase(m_pEngine->GetIgnoreSymbolsInTagsDatabase());

    if(dlg.ShowModal() == wxID_OK) {
        m_pEngine->EnableScannerType(IHunSpell::kString, dlg.GetScanStrings());
        m_pEngine->EnableScannerType(IHunSpell::kCppComment, dlg.GetScanCPP());
        m_pEngine->EnableScannerType(IHunSpell::kCComment, dlg.GetScanC());
        m_pEngine->EnableScannerType(IHunSpell::kDox1, dlg.GetScanD1());
        m_pEngine->EnableScannerType(IHunSpell::kDox2, dlg.GetScanD2());
        m_pEngine->SetDictionaryPath(dlg.GetDictionaryPath());
        m_pEngine->ChangeLanguage(dlg.GetDictionaryFileName());
        m_pEngine->SetCaseSensitiveUserDictionary(dlg.GetCaseSensitiveUserDictionary());
        m_pEngine->SetIgnoreSymbolsInTagsDatabase(dlg.GetIgnoreSymbolsInTagsDatabase());
        SaveSettings();
    }
}

// ------------------------------------------------------------
void SpellCheck::OnCheck(wxCommandEvent& e)
{
    IEditor* editor = GetEditor();
    CHECK_PTR_RET(m_pEngine);
    CHECK_PTR_RET(editor);

    bool old_continous_value = GetCheckContinuous();
    if(GetCheckContinuous()) {
        // switch continuous search off if running
        SetCheckContinuous(false);
    }

    // if we don't have a dictionary yet, open settings
    if(!m_pEngine->GetDictionary()) {
        OnSettings(e);
        return;
    }

    m_pEngine->CheckSpelling();
    if(!GetCheckContinuous()) {
        editor->ClearUserIndicators();
    }

    if(old_continous_value) {
        SetCheckContinuous(true);
    }
}

// ------------------------------------------------------------
void SpellCheck::LoadSettings()
{
    m_mgr->GetConfigTool()->ReadObject(s_spOptions, &m_options);
    m_pEngine->SetDictionaryPath(m_options.GetDictionaryPath());
    m_pEngine->SetDictionary(m_options.GetDictionaryFileName());
    m_pEngine->EnableScannerType(IHunSpell::kString, m_options.GetScanStr());
    m_pEngine->EnableScannerType(IHunSpell::kCppComment, m_options.GetScanCPP());
    m_pEngine->EnableScannerType(IHunSpell::kCComment, m_options.GetScanC());
    m_pEngine->EnableScannerType(IHunSpell::kDox1, m_options.GetScanD1());
    m_pEngine->EnableScannerType(IHunSpell::kDox2, m_options.GetScanD2());
    m_pEngine->SetCaseSensitiveUserDictionary(m_options.GetCaseSensitiveUserDictionary());
    m_pEngine->SetIgnoreSymbolsInTagsDatabase(m_options.GetIgnoreSymbolsInTagsDatabase());
}
// ------------------------------------------------------------
void SpellCheck::SaveSettings()
{
    m_options.SetDictionaryPath(m_pEngine->GetDictionaryPath());
    m_options.SetDictionaryFileName(m_pEngine->GetDictionary());
    m_options.SetScanStr(m_pEngine->IsScannerType(IHunSpell::kString));
    m_options.SetScanCPP(m_pEngine->IsScannerType(IHunSpell::kCppComment));
    m_options.SetScanC(m_pEngine->IsScannerType(IHunSpell::kCComment));
    m_options.SetScanD1(m_pEngine->IsScannerType(IHunSpell::kDox1));
    m_options.SetScanD2(m_pEngine->IsScannerType(IHunSpell::kDox2));
    m_options.SetCaseSensitiveUserDictionary(m_pEngine->GetCaseSensitiveUserDictionary());
    m_options.SetIgnoreSymbolsInTagsDatabase(m_pEngine->GetIgnoreSymbolsInTagsDatabase());
    m_mgr->GetConfigTool()->WriteObject(s_spOptions, &m_options);
}
// ------------------------------------------------------------
void SpellCheck::OnContinousCheck(wxCommandEvent& e)
{
    CHECK_PTR_RET(m_pEngine);

    if(!e.IsChecked()) {
        SetCheckContinuous(false);
        ClearIndicatorsFromEditors();
        return;
    }

    SetCheckContinuous(true);

    // if we don't have a dictionary yet, open settings
    if(!m_pEngine->GetDictionary()) {
        OnSettings(e);
        return;
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    m_pEngine->CheckSpelling();
    if(!GetCheckContinuous()) {
        editor->ClearUserIndicators();
    }
    m_timer.Start(PARSE_TIME);
}

// ------------------------------------------------------------
void SpellCheck::OnTimer(wxTimerEvent& e)
{
    wxTopLevelWindow* pWnd = dynamic_cast<wxTopLevelWindow*>(GetTopWnd());

    if(!pWnd->IsActive()) {
        return;
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);
    CHECK_COND_RET(GetCheckContinuous());

    // Only run the checks if we've not run them or the file is modified.
    const auto modificationCount(editor->GetModificationCount());
    if((editor == m_pLastEditor) && (m_lastModificationCount == modificationCount)) {
        return;
    }

    m_pLastEditor = editor;
    m_lastModificationCount = modificationCount;
    m_pLastEditor->ClearUserIndicators();
    m_pEngine->CheckSpelling();
}

// ------------------------------------------------------------
void SpellCheck::SetCheckContinuous(bool value)
{
    m_options.SetCheckContinuous(value);
    clToolBarButtonBase* btn = clGetManager()->GetToolBar()->FindById(XRCID(s_contCheckID.ToUTF8()));

    if(value) {
        m_pLastEditor = nullptr;
        m_timer.Start(PARSE_TIME);

        if(btn) {
            btn->Check(true);
            clGetManager()->GetToolBar()->Refresh();
        }
    } else {
        if(m_timer.IsRunning()) {
            m_timer.Stop();
        }
        if(btn) {
            btn->Check(false);
            clGetManager()->GetToolBar()->Refresh();
        }
    }
}

// ------------------------------------------------------------
void SpellCheck::OnWspLoaded(clWorkspaceEvent& e)
{
    m_currentWspPath = e.GetString();
    e.Skip();
}

// ------------------------------------------------------------
void SpellCheck::OnWspClosed(clWorkspaceEvent& e) { e.Skip(); }
// ------------------------------------------------------------
void SpellCheck::OnSuggestion(wxCommandEvent& e)
{
    const auto editor = GetEditor();
    if(!editor) {
        return;
    }

    const auto pMenu = dynamic_cast<wxMenu*>(e.GetEventObject());
    if(!pMenu) {
        return;
    }

    const auto pMenuItem = pMenu->FindItem(e.GetId());
    if(!pMenuItem) {
        return;
    }

    editor->ReplaceSelection(pMenuItem->GetItemLabel());
}
// ------------------------------------------------------------
void SpellCheck::OnIgnoreWord(wxCommandEvent& e)
{
    const auto editor = GetEditor();
    if(!editor) {
        return;
    }

    const auto selection = editor->GetSelection();
    if(selection.IsEmpty()) {
        return;
    }

    m_pEngine->AddWordToIgnoreList(selection);
}
// ------------------------------------------------------------
void SpellCheck::OnAddWord(wxCommandEvent& e)
{
    const auto editor = GetEditor();
    if(!editor) {
        return;
    }

    const auto selection = editor->GetSelection();
    if(selection.IsEmpty()) {
        return;
    }

    m_pEngine->AddWordToUserDict(selection);
}
// ------------------------------------------------------------
void SpellCheck::ClearIndicatorsFromEditors()
{
    // Remove the indicators from all the editors
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        (*iter)->ClearUserIndicators();
    }
}

// ------------------------------------------------------------
