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

#include "abbreviation.h"

#include "abbreviationentry.h"
#include "abbreviationssettingsdlg.h"
#include "clKeyboardManager.h"
#include "cl_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "macromanager.h"
#include "wxCodeCompletionBoxEntry.hpp"
#include "wxCodeCompletionBoxManager.h"

#include <algorithm>
#include <wx/app.h>
#include <wx/bitmap.h>
#include <wx/clntdata.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/regex.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

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
    info.SetAuthor("Eran Ifrah");
    info.SetName("Abbreviation");
    info.SetDescription(_("Abbreviation plugin"));
    info.SetVersion("v1.1");
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

class AbbreviationClientData : public wxClientData
{
public:
    AbbreviationClientData() {}
    virtual ~AbbreviationClientData() {}
};

AbbreviationPlugin::AbbreviationPlugin(IManager* manager)
    : IPlugin(manager)
    , m_topWindow(NULL)
    , m_config("abbreviations.conf")
{
    m_longName = _("Abbreviation plugin");
    m_shortName = "Abbreviation";
    m_topWindow = m_mgr->GetTheApp();
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &AbbreviationPlugin::OnAbbrevSelected, this);

    EventNotifier::Get()->Bind(wxEVT_CCBOX_SHOWING, &AbbreviationPlugin::OnCompletionBoxShowing, this);
    // m_helper = new AbbreviationServiceProvider(this);
    InitDefaults();
}

AbbreviationPlugin::~AbbreviationPlugin() {}

void AbbreviationPlugin::CreateToolBar(clToolBar* toolbar) { wxUnusedVar(toolbar); }

void AbbreviationPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("abbrev_insert"), _("Show abbreviations completion box"),
                          _("Show abbreviations completion box"), wxITEM_NORMAL);
    menu->Append(item);
    menu->AppendSeparator();
    item = new wxMenuItem(menu, XRCID("abbrev_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(XRCID("abbreviations_plugin_menu"), _("Abbreviation"), menu);
    m_topWindow->Bind(wxEVT_MENU, &AbbreviationPlugin::OnSettings, this, XRCID("abbrev_settings"));
    m_topWindow->Bind(wxEVT_MENU, &AbbreviationPlugin::OnShowAbbvreviations, this, XRCID("abbrev_insert"));
}

void AbbreviationPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void AbbreviationPlugin::UnPlug()
{
    DeletePluginMenu(XRCID("abbreviations_plugin_menu"));
    m_topWindow->Unbind(wxEVT_MENU, &AbbreviationPlugin::OnSettings, this, XRCID("abbrev_settings"));
    m_topWindow->Unbind(wxEVT_MENU, &AbbreviationPlugin::OnShowAbbvreviations, this, XRCID("abbrev_insert"));
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &AbbreviationPlugin::OnAbbrevSelected, this);
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SHOWING, &AbbreviationPlugin::OnCompletionBoxShowing, this);
}

void AbbreviationPlugin::OnSettings(wxCommandEvent& e)
{
    AbbreviationsSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.ShowModal();
    m_config.Reload();
}

void AbbreviationPlugin::GetAbbreviations(wxCodeCompletionBoxEntry::Vec_t& V, const wxString& filter)
{
    wxString lcFilter = filter.Lower();

    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject("AbbreviationsData", &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    wxBitmap bmp = clGetManager()->GetStdIcons()->LoadBitmap("replace-blue");
    if(bmp.IsOk()) {
        // search for the old item
        const wxStringMap_t& entries = jsonData.GetEntries();
        std::for_each(entries.begin(), entries.end(), [&](const wxStringMap_t::value_type& vt) {
            // Only add matching entries (entries that "starts_with")
            wxString lcAbbv = vt.first.Lower();
            if(lcAbbv.StartsWith(lcFilter)) {
                // Append our entries
                wxString textHelp;
                textHelp << "**Abbreviation entry**\n===```" << vt.second << "```";
                V.push_back(wxCodeCompletionBoxEntry::New(vt.first, textHelp, bmp, new AbbreviationClientData()));
            }
        });
    }
}

void AbbreviationPlugin::OnAbbrevSelected(clCodeCompletionEvent& e)
{
    if(!e.GetEntry()) {
        e.Skip();
        return;
    }

    AbbreviationClientData* cd = dynamic_cast<AbbreviationClientData*>(e.GetEntry()->GetClientData());
    if(!cd) {
        e.Skip();
        return;
    }
    InsertExpansion(e.GetWord());
}

void AbbreviationPlugin::InitDefaults()
{
    // check to see if there are any abbreviations configured
    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject("AbbreviationsData", &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    // search for the old item
    if(jsonData.GetEntries().empty()) {
        // fill some default abbreviations
        wxStringMap_t entries;
        entries["main"] = "int main(int argc, char **argv) {\n    |\n}\n";
        entries["while"] = "while(|) {\n    \n}\n";
        entries["dowhile"] = "do {\n    \n} while( | );\n";
        entries["for_size"] = "for(size_t |=0; |<; ++|) {\n}\n";
        entries["for_int"] = "for(int |=0; |<; ++|) {\n}\n";
        entries["for_php"] = "for($|=0; $|<; ++$|) {\n}\n";
        jsonData.SetEntries(entries);
        m_config.WriteItem(&jsonData);
    }
    clKeyboardManager::Get()->AddAccelerator("abbrev_insert", _("Abbreviations"),
                                             _("Show abbreviations completion box"), "Ctrl-Alt-SPACE");
}

bool AbbreviationPlugin::InsertExpansion(const wxString& abbreviation)
{
    // get the active editor
    IEditor* editor = m_mgr->GetActiveEditor();
    if(!editor || abbreviation.IsEmpty()) {
        return false;
    }

    // search for abbreviation that matches str
    // prepate list of abbreviations
    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject("AbbreviationsData", &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    // search for the old item
    const wxStringMap_t& entries = jsonData.GetEntries();
    wxStringMap_t::const_iterator iter = entries.find(abbreviation);

    if(iter != entries.end()) {

        wxString text = iter->second;
        int selStart = editor->WordStartPos(editor->GetCurrentPosition(), true);
        int selEnd = editor->WordEndPos(editor->GetCurrentPosition(), true);
        int curPos = editor->GetCurrentPosition();
        int typedWordLen = curPos - selStart;
        wxString textOrig;
        wxString textLeadingSpaces;

        if(typedWordLen < 0) {
            typedWordLen = 0;
        }

        // format the text to insert
        bool appendEol(false);
        if(text.EndsWith("\r") || text.EndsWith("\n")) {
            appendEol = true;
        }

        textOrig = text;
        text.Trim(false);
        textLeadingSpaces = textOrig.Left(textOrig.length() - text.length());

        text = editor->FormatTextKeepIndent(text, selStart, Format_Text_Save_Empty_Lines);

        // remove the first line indenation that might have been placed by CL
        text.Trim(false).Trim();
        text = textLeadingSpaces + text;

        if(appendEol) {
            wxString eol;
            switch(editor->GetEOL()) {
            case 1:
                eol = "\r";
                break;
            case 0:
                eol = "\r\n";
                break;
            case 2:
                eol = "\n";
                break;
            }
            text << eol;
        }

        //--------------------------------------------
        // replace any place holders
        //--------------------------------------------
        wxString project;
        text = MacroManager::Instance()->Expand(text, m_mgr, editor->GetProjectName());

        // locate the caret(s)
        std::vector<int> carets;
        int where = text.Find("|");
        while(where != wxNOT_FOUND) {
            carets.push_back(where);
            where = text.find('|', where + 1);
        }

        // remove the pipe (|) character
        text.Replace("|", " ");

        if(selEnd - selStart >= 0) {
            editor->SelectText(selStart, selEnd - selStart);
            editor->ReplaceSelection(text);
            editor->GetCtrl()->ClearSelections();

            bool first = true;
            std::for_each(carets.begin(), carets.end(), [&](int where) {
                int caretPos = curPos + where - typedWordLen;
                if(first) {
                    editor->GetCtrl()->SetSelection(caretPos, caretPos + 1);
                    first = false;
                } else {
                    editor->GetCtrl()->AddSelection(caretPos, caretPos + 1);
                }
            });
        }
        return true;
    } else
        return false;
}

void AbbreviationPlugin::OnCompletionBoxShowing(clCodeCompletionEvent& event) { event.Skip(); }

void AbbreviationPlugin::OnShowAbbvreviations(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    wxCodeCompletionBoxEntry::Vec_t V;
    GetAbbreviations(V, editor->GetWordAtPosition(editor->GetCurrentPosition()));
    if(!V.empty()) {
        wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, V, wxCodeCompletionBox::kRefreshOnKeyType,
                                                            wxNOT_FOUND);
    }
}

// Helper
// AbbreviationServiceProvider::AbbreviationServiceProvider(AbbreviationPlugin* plugin)
//    : ServiceProvider("Abbreviations", eServiceType::kCodeCompletion)
//    , m_plugin(plugin)
//{
//    SetPriority(10);
//    Bind(wxEVT_CC_WORD_COMPLETE, &AbbreviationServiceProvider::OnWordComplete, this);
//}
//
// AbbreviationServiceProvider::~AbbreviationServiceProvider()
//{
//    Unbind(wxEVT_CC_WORD_COMPLETE, &AbbreviationServiceProvider::OnWordComplete, this);
//}
//
// void AbbreviationServiceProvider::OnWordComplete(clCodeCompletionEvent& event)
//{
//    event.Skip();
//    if(event.GetTriggerKind() == LSP::CompletionItem::kTriggerUser) {
//        // user used Ctr-SPACE
//        m_plugin->AddAbbreviations(event);
//    }
//}
