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
#include "wx/wxprec.h"
#include "event_notifier.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "spellcheck.h"
#include "workspace.h"
#include "ctags_manager.h"
#include "scGlobals.h"
#include "IHunSpell.h"
#include "SpellCheckerSettings.h"

#include <wx/mstream.h>
#include <wx/xrc/xmlres.h>
#include <wx/tokenzr.h>
#include <wx/stc/stc.h>

#include "res/spellcheck16.b2c"
#include "res/spellcheck22.b2c"
//#include "res/contCheck16.b2c"
//#include "res/contCheck22.b2c"

static SpellCheck* thePlugin = NULL;
#define SPC_BASEID 10000
#define PARSE_TIME 500
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
    info.SetAuthor(wxT("Frank Lichtner"));
    info.SetName(wxT("SpellCheck"));
    info.SetDescription(_("CodeLite spell checker"));
    info.SetVersion(wxT("v1.6"));
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
    m_topWin->Disconnect(
        IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheck::OnSettings), NULL, this);
    m_topWin->Disconnect(XRCID(s_doCheckID.ToUTF8()),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(SpellCheck::OnCheck),
                         NULL,
                         this);
    m_topWin->Disconnect(XRCID(s_contCheckID.ToUTF8()),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(SpellCheck::OnContinousCheck),
                         NULL,
                         this);
    m_timer.Disconnect(wxEVT_TIMER, wxTimerEventHandler(SpellCheck::OnTimer), NULL, this);
    m_topWin->Disconnect(wxEVT_CMD_EDITOR_CONTEXT_MENU, wxCommandEventHandler(SpellCheck::OnContextMenu), NULL, this);
    m_topWin->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SpellCheck::OnWspClosed), NULL, this);

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
    m_pToolbar = NULL;
    m_topWin = wxTheApp;
    m_pEngine = new IHunSpell();
    m_currentWspPath = wxEmptyString;

    if(m_pEngine) {
        LoadSettings();
        wxString userDictPath = clStandardPaths::Get().GetUserDataDir();
        userDictPath << wxFILE_SEP_PATH << wxT("spellcheck") << wxFILE_SEP_PATH;

        if(!wxFileName::DirExists(userDictPath)) wxFileName::Mkdir(userDictPath);

        m_pEngine->SetUserDictPath(userDictPath);
        m_pEngine->SetPlugIn(this);

        if(!m_options.GetDictionaryFileName().IsEmpty()) m_pEngine->InitEngine();
    }
    m_timer.Connect(wxEVT_TIMER, wxTimerEventHandler(SpellCheck::OnTimer), NULL, this);
    m_topWin->Connect(wxEVT_CMD_EDITOR_CONTEXT_MENU, wxCommandEventHandler(SpellCheck::OnContextMenu), NULL, this);
    m_topWin->Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(SpellCheck::OnWspLoaded), NULL, this);
    m_topWin->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(SpellCheck::OnWspClosed), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &SpellCheck::OnEditorContextMenuShowing, this);
}
// ------------------------------------------------------------
clToolBar* SpellCheck::CreateToolBar(wxWindow* parent)
{
    if(m_mgr->AllowToolbar()) {
        int size = m_mgr->GetToolbarIconSize();
        m_pToolbar = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE_PLUGIN);
        m_pToolbar->SetToolBitmapSize(wxSize(size, size));
        m_pToolbar->AddTool(XRCID(s_doCheckID.ToUTF8()),
                            _("Check spelling..."),
                            m_mgr->GetStdIcons()->LoadBitmap("spellcheck", size),
                            _("Run spell-checker"));
        m_pToolbar->AddTool(XRCID(s_contCheckID.ToUTF8()),
                            _("Check continuous"),
                            m_mgr->GetStdIcons()->LoadBitmap("repeat", size),
                            _("Run continuous check"),
                            wxITEM_CHECK);
        m_pToolbar->Realize();
    }
    parent->Connect(XRCID(s_doCheckID.ToUTF8()),
                    wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(SpellCheck::OnCheck),
                    NULL,
                    this);
    parent->Connect(XRCID(s_contCheckID.ToUTF8()),
                    wxEVT_COMMAND_MENU_SELECTED,
                    wxCommandEventHandler(SpellCheck::OnContinousCheck),
                    NULL,
                    this);

    SetCheckContinuous(GetCheckContinuous());

    return m_pToolbar;
}
// ------------------------------------------------------------
void SpellCheck::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* pMenu = new wxMenu();
    wxMenuItem* pItem(NULL);

    pItem = new wxMenuItem(pMenu, IDM_SETTINGS, _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    pMenu->Append(pItem);
    pluginsMenu->Append(wxID_ANY, s_plugName, pMenu);

    m_topWin->Connect(
        IDM_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SpellCheck::OnSettings), NULL, this);
}
// ------------------------------------------------------------
wxMenu* SpellCheck::CreateSubMenu()
{
    wxMenu* menu = new wxMenu();

    wxMenuItem* pItem(NULL);
    pItem = new wxMenuItem(menu, XRCID(s_doCheckID.ToUTF8()), _("Check..."), _("Check..."), wxITEM_NORMAL);
    menu->Append(pItem);
    pItem = new wxMenuItem(
        menu, XRCID(s_contCheckID.ToUTF8()), _("Check continuous"), _("Start continuous check"), wxITEM_CHECK);
    menu->Append(pItem);
    return menu;
}
// ------------------------------------------------------------
void SpellCheck::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

// ------------------------------------------------------------
void SpellCheck::UnHookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

// ------------------------------------------------------------
void SpellCheck::UnPlug()
{
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &SpellCheck::OnEditorContextMenuShowing, this);
    if(m_timer.IsRunning()) m_timer.Stop();
}

// ------------------------------------------------------------
// returns pointer to editor
IEditor* SpellCheck::GetEditor()
{
    IEditor* editor = m_mgr->GetActiveEditor();

    if(!editor) {
        ::wxMessageBox(s_noEditor, s_codeLite, wxICON_WARNING | wxOK);
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

    if(dlg.ShowModal() == wxID_OK) {
        m_pEngine->EnableScannerType(IHunSpell::kString, dlg.GetScanStrings());
        m_pEngine->EnableScannerType(IHunSpell::kCppComment, dlg.GetScanCPP());
        m_pEngine->EnableScannerType(IHunSpell::kCComment, dlg.GetScanC());
        m_pEngine->EnableScannerType(IHunSpell::kDox1, dlg.GetScanD1());
        m_pEngine->EnableScannerType(IHunSpell::kDox2, dlg.GetScanD2());
        m_pEngine->SetDictionaryPath(dlg.GetDictionaryPath());
        m_pEngine->ChangeLanguage(dlg.GetDictionaryFileName());
        m_pEngine->SetCaseSensitiveUserDictionary(dlg.GetCaseSensitiveUserDictionary());
        SaveSettings();
    }
}
// ------------------------------------------------------------
void SpellCheck::OnCheck(wxCommandEvent& e)
{
    IEditor* editor = GetEditor();

    if(!editor) return;
    wxString text = editor->GetEditorText();
    text += wxT(" "); // prevents indicator flickering at end of file

    if(m_pEngine != NULL) {
        if(GetCheckContinuous()) // switch continuous search off if running
            SetCheckContinuous(false);

        // if we don't have a dictionary yet, open settings
        if(!m_pEngine->GetDictionary()) {
            OnSettings(e);
            return;
        }

        switch(editor->GetLexerId()) {
        case 3: { // wxSCI_LEX_CPP
            if(m_mgr->IsWorkspaceOpen()) {
                m_pEngine->CheckCppSpelling(text);

                if(!GetCheckContinuous()) {
                    editor->ClearUserIndicators();
                }
            }
        } break;
        case 1: { // wxSCI_LEX_NULL
            m_pEngine->CheckSpelling(text);
            if(!GetCheckContinuous()) {
                editor->ClearUserIndicators();
            }
        } break;
        }
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
    m_mgr->GetConfigTool()->WriteObject(s_spOptions, &m_options);
}
// ------------------------------------------------------------
void SpellCheck::OnContinousCheck(wxCommandEvent& e)
{
    if(m_pEngine != NULL) {
        if(e.GetInt() == 0) {
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

        if (editor) {
            wxString text = editor->GetEditorText();

            switch(editor->GetLexerId()) {
            case 3: { // wxSCI_LEX_CPP
                if(m_mgr->IsWorkspaceOpen()) {
                    m_pEngine->CheckCppSpelling(text);
                }
            } break;
            default: { // wxSCI_LEX_NULL
                m_pEngine->CheckSpelling(text);
            } break;
            }
            m_timer.Start(PARSE_TIME);
        }
    }
}
// ------------------------------------------------------------
void SpellCheck::OnTimer(wxTimerEvent& e)
{
    wxTopLevelWindow* pWnd = dynamic_cast<wxTopLevelWindow*>(GetTopWnd());

    if(!pWnd->IsActive()) return;

    IEditor* editor = m_mgr->GetActiveEditor();

    if(!editor) return;

    if(GetCheckContinuous()) {
        // Only run the checks if we've not run them or the file is modified.
        const auto modificationCount(editor->GetModificationCount());
        if ((editor == m_pLastEditor) && (m_lastModificationCount == modificationCount))
            return;

        m_pLastEditor = editor;
        m_lastModificationCount = modificationCount;

        switch(editor->GetLexerId()) {
        case 3: { // wxSCI_LEX_CPP
            if(m_mgr->IsWorkspaceOpen()) {
                m_pEngine->CheckCppSpelling(editor->GetEditorText());
            }
        } break;
        default: { // wxSCI_LEX_NULL
            m_pEngine->CheckSpelling(editor->GetEditorText());
        } break;
        }
    }
}
// ------------------------------------------------------------
void SpellCheck::OnContextMenu(wxCommandEvent& e)
{
    m_pLastEditor = nullptr;

    IEditor* editor = GetEditor();

    if(!editor) {
        e.Skip();
        return;
    }

    wxPoint pt = wxGetMousePosition();
    pt = editor->GetCtrl()->ScreenToClient(pt);
    int pos = editor->GetCtrl()->PositionFromPoint(pt);

    if(editor->GetCtrl()->IndicatorValueAt(3, pos) == 1) {
        wxMenu popUp;
        m_timer.Stop();

        int start = editor->WordStartPos(pos, true);
        editor->SelectText(start, editor->WordEndPos(pos, true) - start);
        wxString sel = editor->GetSelection();
        wxArrayString sugg = m_pEngine->GetSuggestions(sel);

        for(wxUint32 i = 0; i < sugg.GetCount(); i++) {
            popUp.Append(SPC_BASEID + i, sugg[i], "");
        }

        if(sugg.GetCount() == 0)
            popUp.SetTitle(_("No suggestions"));
        else
            popUp.AppendSeparator();

        popUp.Append(SPC_BASEID - 1, _("Ignore"), "");
        popUp.Append(SPC_BASEID - 2, _("Add"), "");

        int index = editor->GetCtrl()->GetPopupMenuSelectionFromUser(popUp);

        if(index != wxID_NONE) {
            if(index >= SPC_BASEID) {
                index -= SPC_BASEID;
                editor->ReplaceSelection(sugg[index]);
            } else {
                if(index == SPC_BASEID - 1)
                    m_pEngine->AddWordToIgnoreList(sel);
                else if(index == SPC_BASEID - 2)
                    m_pEngine->AddWordToUserDict(sel);
            }
        }
        m_timer.Start(PARSE_TIME);
    } else
        e.Skip();
}
// ------------------------------------------------------------
void SpellCheck::SetCheckContinuous(bool value)
{
    m_options.SetCheckContinuous(value);

    if(value) {
        m_pLastEditor = nullptr;
        m_timer.Start(PARSE_TIME);

        if(m_pToolbar) {
            m_pToolbar->ToggleTool(XRCID(s_contCheckID.ToUTF8()), true);
            m_pToolbar->Refresh();
        }
    } else {
        if(m_timer.IsRunning()) m_timer.Stop();

        if(m_pToolbar) {
            m_pToolbar->ToggleTool(XRCID(s_contCheckID.ToUTF8()), false);
            m_pToolbar->Refresh();
        }
    }
}

// ------------------------------------------------------------
bool SpellCheck::IsTag(const wxString& token)
{
    std::vector<TagEntryPtr> tags;
    m_mgr->GetTagsManager()->FindSymbol(token, tags);
    return (tags.size() == 0) ? false : true;
}
// ------------------------------------------------------------
void SpellCheck::OnWspLoaded(wxCommandEvent& e)
{
    m_currentWspPath = e.GetString();
    e.Skip();
} // ------------------------------------------------------------
void SpellCheck::OnWspClosed(wxCommandEvent& e) { e.Skip(); }

void SpellCheck::OnEditorContextMenuShowing(clContextMenuEvent& e)
{
    e.Skip();
    wxMenu* menu = CreateSubMenu();
    menu->Check(XRCID(s_contCheckID.ToUTF8()), GetCheckContinuous());
    e.GetMenu()->Append(IDM_BASE, _("Spell Checker"), menu);
}

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
