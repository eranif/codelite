//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : abbreviation.cpp
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

#include <wx/textdlg.h>
#include "abbreviationentry.h"
#include "abbreviationssettingsdlg.h"
#include <wx/bitmap.h>
#include <wx/menu.h>
#include "event_notifier.h"
#include "abbreviation.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/regex.h>
#include "cl_config.h"
#include "macromanager.h"
#include "clKeyboardManager.h"
#include "wxCodeCompletionBoxEntry.h"
#include "wxCodeCompletionBoxManager.h"

static AbbreviationPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new AbbreviationPlugin(manager);
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Abbreviation"));
    info.SetDescription(_("Abbreviation plugin"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

AbbreviationPlugin::AbbreviationPlugin(IManager* manager)
    : IPlugin(manager)
    , m_topWindow(NULL)
    , m_config("abbreviations.conf")
{
    m_longName = _("Abbreviation plugin");
    m_shortName = wxT("Abbreviation");
    m_topWindow = m_mgr->GetTheApp();
    EventNotifier::Get()->Connect(
        wxEVT_CCBOX_SELECTION_MADE, clCodeCompletionEventHandler(AbbreviationPlugin::OnAbbrevSelected), NULL, this);

    InitDefaults();
}

AbbreviationPlugin::~AbbreviationPlugin() {}

clToolBar* AbbreviationPlugin::CreateToolBar(wxWindow* parent)
{
    wxUnusedVar(parent);
    return NULL;
}

void AbbreviationPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("abbrev_insert"), _("Insert Expansion"), _("Insert Expansion"), wxITEM_NORMAL);
    menu->Append(item);

    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("abbrev_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, wxT("Abbreviation"), menu);

    m_topWindow->Connect(XRCID("abbrev_settings"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(AbbreviationPlugin::OnSettings),
                         NULL,
                         this);
    m_topWindow->Connect(XRCID("abbrev_insert"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(AbbreviationPlugin::OnAbbreviations),
                         NULL,
                         this);
}

void AbbreviationPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void AbbreviationPlugin::UnPlug()
{
    EventNotifier::Get()->Disconnect(
        wxEVT_CCBOX_SELECTION_MADE, clCodeCompletionEventHandler(AbbreviationPlugin::OnAbbrevSelected), NULL, this);
    m_topWindow->Disconnect(XRCID("abbrev_settings"),
                            wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(AbbreviationPlugin::OnSettings),
                            NULL,
                            this);
    m_topWindow->Disconnect(XRCID("abbrev_insert"),
                            wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler(AbbreviationPlugin::OnAbbreviations),
                            NULL,
                            this);
}

void AbbreviationPlugin::OnSettings(wxCommandEvent& e)
{
    AbbreviationsSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.ShowModal();
    m_config.Reload();
}

void AbbreviationPlugin::OnAbbreviations(wxCommandEvent& e)
{
    IEditor* editor = m_mgr->GetActiveEditor();
    if(!editor) {
        return;
    }

    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    wxString wordAtCaret = editor->GetWordAtCaret();

    bool autoInsert = (jsonData.IsAutoInsert() && wordAtCaret.IsEmpty() == false);
    if(autoInsert) {
        autoInsert = InsertExpansion(wordAtCaret);
    }

    if(!autoInsert) {
        static wxBitmap bmp = LoadBitmapFile(wxT("abbrev.png"));
        if(bmp.IsOk()) {
            wxCodeCompletionBoxEntry::Vec_t ccEntries;
            wxCodeCompletionBox::BmpVec_t bitmaps;
            bitmaps.push_back(bmp);

            // search for the old item
            const JSONElement::wxStringMap_t& entries = jsonData.GetEntries();
            JSONElement::wxStringMap_t::const_iterator iter = entries.begin();
            for(; iter != entries.end(); ++iter) {
                ccEntries.push_back(wxCodeCompletionBoxEntry::New(iter->first, 0));
            }
            wxCodeCompletionBoxManager::Get().ShowCompletionBox(
                editor->GetCtrl(), ccEntries, bitmaps, wxCodeCompletionBox::kNone, wxNOT_FOUND, this);
        }
    }
}

void AbbreviationPlugin::OnAbbrevSelected(clCodeCompletionEvent& e)
{
    if(e.GetEventObject() != this) {
        e.Skip();
        return;
    }

    wxString wordAtCaret = e.GetWord();
    InsertExpansion(wordAtCaret);
}

void AbbreviationPlugin::InitDefaults()
{
    // check to see if there are any abbreviations configured
    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    // search for the old item
    if(jsonData.GetEntries().empty()) {
        // fill some default abbreviations
        JSONElement::wxStringMap_t entries;
        entries[wxT("main")] = wxT("int main(int argc, char **argv) {\n    |\n}\n");
        entries[wxT("while")] = wxT("while(|) {\n    \n}\n");
        entries[wxT("dowhile")] = wxT("do {\n    \n} while( | );\n");
        entries[wxT("for_size")] = wxT("for(size_t i=0; i<|; ++i) {\n}\n");
        entries[wxT("for_int")] = wxT("for(int i=0; i<|; ++i) {\n}\n");
        jsonData.SetEntries(entries);
        m_config.WriteItem(&jsonData);
    }
    clKeyboardManager::Get()->AddGlobalAccelerator(
        "abbrev_insert", "Ctrl-Alt-SPACE", _("Plugins::Abbreviations::Show abbreviations completion box"));
}

bool AbbreviationPlugin::InsertExpansion(const wxString& abbreviation)
{
    // get the active editor
    IEditor* editor = m_mgr->GetActiveEditor();
    if(!editor || abbreviation.IsEmpty()) return false;

    // search for abbreviation that matches str
    // prepate list of abbreviations
    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    // search for the old item
    const JSONElement::wxStringMap_t& entries = jsonData.GetEntries();
    JSONElement::wxStringMap_t::const_iterator iter = entries.find(abbreviation);

    if(iter != entries.end()) {

        wxString text = iter->second;
        int selStart = editor->WordStartPos(editor->GetCurrentPosition(), true);
        int selEnd = editor->WordEndPos(editor->GetCurrentPosition(), true);
        int curPos = editor->GetCurrentPosition();
        int typedWordLen = curPos - selStart;

        if(typedWordLen < 0) {
            typedWordLen = 0;
        }

        // format the text to insert
        bool appendEol(false);
        if(text.EndsWith(wxT("\r")) || text.EndsWith(wxT("\n"))) {
            appendEol = true;
        }

        text = editor->FormatTextKeepIndent(text, selStart, Format_Text_Save_Empty_Lines);

        // remove the first line indenation that might have been placed by CL
        text.Trim(false).Trim();

        if(appendEol) {
            wxString eol;
            switch(editor->GetEOL()) {
            case 1:
                eol = wxT("\r");
                break;
            case 0:
                eol = wxT("\r\n");
                break;
            case 2:
                eol = wxT("\n");
                break;
            }
            text << eol;
        }

        //--------------------------------------------
        // replace any place holders
        //--------------------------------------------
        wxString project;
        text = MacroManager::Instance()->Expand(text, m_mgr, editor->GetProjectName());

        // locate the caret
        int where = text.Find(wxT("|"));
        if(where == wxNOT_FOUND) {
            where = text.length();
        }

        // remove the pipe (|) character
        text.Replace(wxT("|"), wxT(""));

        if(selEnd - selStart >= 0) {
            editor->SelectText(selStart, selEnd - selStart);
            editor->ReplaceSelection(text);
            editor->SetCaretAt(curPos + where - typedWordLen);
        }
        return true;
    } else
        return false;
}
