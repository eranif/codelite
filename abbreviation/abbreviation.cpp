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
#include "abbreviation.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/regex.h>

static AbbreviationPlugin* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new AbbreviationPlugin(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("abbreviation"));
	info.SetDescription(_("Abbreviation plugin"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

AbbreviationPlugin::AbbreviationPlugin(IManager *manager)
		: IPlugin(manager)
		, m_topWindow(NULL)
{
	m_longName = _("Abbreviation plugin");
	m_shortName = wxT("abbreviation");
	m_topWindow = m_mgr->GetTheApp();
	m_topWindow->Connect(wxEVT_CCBOX_SELECTION_MADE, wxCommandEventHandler(AbbreviationPlugin::OnAbbrevSelected), NULL, this);

	InitDefaults();
}

AbbreviationPlugin::~AbbreviationPlugin()
{
}

clToolBar *AbbreviationPlugin::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	return NULL;
}

void AbbreviationPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item( NULL );

	item = new wxMenuItem(menu, XRCID("abbrev_show"), _( "Show Abbreviations" ), _( "Show Abbreviations" ), wxITEM_NORMAL );
	menu->Append( item );

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("abbrev_settings"), _( "Settings..." ), _( "Settings..." ), wxITEM_NORMAL );
	menu->Append( item );


	pluginsMenu->Append(wxID_ANY, wxT("Abbreviation"), menu);

	m_topWindow->Connect( XRCID("abbrev_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AbbreviationPlugin::OnSettings ), NULL, this );
	m_topWindow->Connect( XRCID("abbrev_show"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AbbreviationPlugin::OnAbbreviations), NULL, this );
}

void AbbreviationPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void AbbreviationPlugin::UnPlug()
{
	m_topWindow->Disconnect(wxEVT_CCBOX_SELECTION_MADE, wxCommandEventHandler(AbbreviationPlugin::OnAbbrevSelected), NULL, this);
	m_topWindow->Disconnect( XRCID("abbrev_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AbbreviationPlugin::OnSettings ), NULL, this );
	m_topWindow->Disconnect( XRCID("abbrev_show"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AbbreviationPlugin::OnAbbreviations), NULL, this );
}

void AbbreviationPlugin::OnSettings(wxCommandEvent& e)
{
	AbbreviationsSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.ShowModal();
}

void AbbreviationPlugin::OnAbbreviations(wxCommandEvent& e)
{
	static wxBitmap bmp = LoadBitmapFile(wxT("abbrev.png")) ;
	IEditor *editor = m_mgr->GetActiveEditor();
	if ( editor && bmp.IsOk() ) {
		editor->RegisterImageForKind(wxT("Abbreviation"), bmp);
		std::vector<TagEntryPtr> tags;

		// prepate list of abbreviations
		AbbreviationEntry data;
		m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

		// search for the old item
		std::map<wxString, wxString> entries = data.GetEntries();
		std::map<wxString, wxString>::iterator iter = entries.begin();
		for (; iter != entries.end(); iter ++) {
			TagEntryPtr t(new TagEntry());
			t->SetName(iter->first);
			t->SetKind(wxT("Abbreviation"));
			tags.push_back(t);
		}
		editor->ShowCompletionBox(tags, editor->GetWordAtCaret(), this);
	}
}

void AbbreviationPlugin::OnAbbrevSelected(wxCommandEvent& e)
{
	// get the active editor
	IEditor *editor = m_mgr->GetActiveEditor();

	// Note that we do not delete str!
	wxString *str = (wxString*)e.GetClientData();

	if (!editor || !str)
		return;

	// hide the completion box
	editor->HideCompletionBox();

	// search for abbreviation that matches str
	// prepate list of abbreviations
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

	// search for the old item
	std::map<wxString, wxString> entries = data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(*str);


	if (iter != entries.end()) {

		wxString text = iter->second;
		int selStart = editor->WordStartPos(editor->GetCurrentPosition(), true);
		int selEnd   = editor->WordEndPos(editor->GetCurrentPosition(), true);
		int curPos   = editor->GetCurrentPosition();
		int typedWordLen = curPos - selStart;

		if (typedWordLen < 0) {
			typedWordLen = 0;
		}

		// format the text to insert
		bool appendEol(false);
		if (text.EndsWith(wxT("\r")) || text.EndsWith(wxT("\n"))) {
			appendEol = true;
		}

		text = editor->FormatTextKeepIndent(text, selStart);

		// remove the first line indenation that might have been placed by CL
		text.Trim(false).Trim();

		if (appendEol) {
			wxString eol;
			switch (editor->GetEOL()) {
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
		static wxRegEx reVarPattern(wxT("\\$\\(( *)([a-zA-Z0-9_]+)( *)\\)"));

		while (reVarPattern.Matches(text)) {
			// add result
			wxString v = reVarPattern.GetMatch(text);
			wxString replaceWith = wxGetTextFromUser(wxString::Format(_("Enter replacement for '%s':"), v.c_str()), _("CodeLite"));
			text.Replace(v, replaceWith);
		}

		// locate the caret
		int where = text.Find(wxT("|"));
		if (where == wxNOT_FOUND) {
			where = text.length();
		}

		// remove the pipe (|) character
		text.Replace(wxT("|"), wxT(""));

		if (selEnd - selStart >= 0) {
			editor->SelectText(selStart, selEnd - selStart);
			editor->ReplaceSelection(text);
			editor->SetCaretAt(curPos + where - typedWordLen);
		}
	}
}

void AbbreviationPlugin::InitDefaults()
{
	// check to see if there are any abbreviations configured
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);

	// search for the old item
	if (data.GetEntries().empty()) {
		// fill some default abbreviations
		std::map<wxString, wxString> entries;
		entries[wxT("main")] = wxT("int main(int argc, char **argv)\n{\n\t|\n}\n");
		entries[wxT("while")] = wxT("while(|)\n{\n\t\n}\n");
		entries[wxT("dowhile")] = wxT("do\n{\n\t\n}while(|)\n");
		entries[wxT("tryblock")] = wxT("try\n{\n\t|\n}\ncatch($(ExceptionType) e)\n{\n}\n");
		entries[wxT("for_size")] = wxT("for(size_t i=0; i<|; i++)\n{\n}\n");
		entries[wxT("for_int")] = wxT("for(int i=0; i<|; i++)\n{\n}\n");
		data.SetEntries(entries);

		m_mgr->GetConfigTool()->WriteObject(wxT("AbbreviationsData"), &data);
	}
}
