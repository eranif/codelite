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

#include <algorithm>
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
#include <wx/clntdata.h>
#include "globals.h"

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
    info.SetVersion(wxT("v1.1"));
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
    m_shortName = wxT("Abbreviation");
    m_topWindow = m_mgr->GetTheApp();
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SELECTION_MADE, &AbbreviationPlugin::OnAbbrevSelected, this);
    EventNotifier::Get()->Bind(wxEVT_CCBOX_SHOWING, &AbbreviationPlugin::OnCompletionBoxShowing, this);
    EventNotifier::Get()->Bind(wxEVT_CC_WORD_COMPLETE, &AbbreviationPlugin::OnWordComplete, this);
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

    item = new wxMenuItem(menu, XRCID("abbrev_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, wxT("Abbreviation"), menu);
    m_topWindow->Bind(wxEVT_MENU, &AbbreviationPlugin::OnSettings, this, XRCID("abbrev_settings"));
}

void AbbreviationPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void AbbreviationPlugin::UnPlug()
{
    m_topWindow->Unbind(wxEVT_MENU, &AbbreviationPlugin::OnSettings, this, XRCID("abbrev_settings"));
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SELECTION_MADE, &AbbreviationPlugin::OnAbbrevSelected, this);
    EventNotifier::Get()->Unbind(wxEVT_CCBOX_SHOWING, &AbbreviationPlugin::OnCompletionBoxShowing, this);
    EventNotifier::Get()->Unbind(wxEVT_CC_WORD_COMPLETE, &AbbreviationPlugin::OnWordComplete, this);
}

void AbbreviationPlugin::OnSettings(wxCommandEvent& e)
{
    AbbreviationsSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
    dlg.ShowModal();
    m_config.Reload();
}

void AbbreviationPlugin::AddAbbreviations(clCodeCompletionEvent& e)
{
    wxString filter = e.GetWord().Lower();

    AbbreviationJSONEntry jsonData;
    if(!m_config.ReadItem(&jsonData)) {
        // merge the data from the old configuration
        AbbreviationEntry data;
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

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
            if(lcAbbv.StartsWith(filter)) {
                // Append our entries
                wxString textHelp;
                textHelp << "<strong>Abbreviation entry</strong>\n<hr><code>" << vt.second << "</code>";
                e.GetEntries().push_back(
                    wxCodeCompletionBoxEntry::New(vt.first, textHelp, bmp, new AbbreviationClientData()));
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
        m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

        jsonData.SetAutoInsert(data.GetAutoInsert());
        jsonData.SetEntries(data.GetEntries());
        m_config.WriteItem(&jsonData);
    }

    // search for the old item
    if(jsonData.GetEntries().empty()) {
        // fill some default abbreviations
        wxStringMap_t entries;
        entries["main"] = wxT("int main(int argc, char **argv) {\n    |\n}\n");
        entries["while"] = wxT("while(|) {\n    \n}\n");
        entries["dowhile"] = wxT("do {\n    \n} while( | );\n");
        entries["for_size"] = wxT("for(size_t |=0; |<; ++|) {\n}\n");
        entries["for_int"] = wxT("for(int |=0; |<; ++|) {\n}\n");
        entries["for_php"] = wxT("for($|=0; $|<; ++$|) {\n}\n");
        jsonData.SetEntries(entries);
        m_config.WriteItem(&jsonData);
    }
    clKeyboardManager::Get()->AddGlobalAccelerator("abbrev_insert", "Ctrl-Alt-SPACE",
                                                   _("Plugins::Abbreviations::Show abbreviations completion box"));
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
        if(text.EndsWith(wxT("\r")) || text.EndsWith(wxT("\n"))) {
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

        // locate the caret(s)
        std::vector<int> carets;
        int where = text.Find(wxT("|"));
        while(where != wxNOT_FOUND) {
            carets.push_back(where);
            where = text.find('|', where + 1);
        }

        // remove the pipe (|) character
        text.Replace(wxT("|"), wxT(" "));

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

void AbbreviationPlugin::OnCompletionBoxShowing(clCodeCompletionEvent& event)
{
    AddAbbreviations(event);
    event.Skip();
}

void AbbreviationPlugin::OnWordComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    AddAbbreviations(event);
}
