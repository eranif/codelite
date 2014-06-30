//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : codebeautifier.cpp
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

#include "codebeautifier.h"
#include <wx/xrc/xmlres.h>
#include "UncrustifyCodeBeautifier/uncrustify.h"
#include "globals.h"
#include "event_notifier.h"
#include "file_logger.h"

static CodeBeautifier* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new CodeBeautifier(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("NilC"));
	info.SetName(wxT("CodeBeautifier"));
	info.SetDescription(wxT("Source Code Formatter based on the open source Uncrustify tool"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

// Error handler for the UncrustifyCodeBeautifier
void UncrustifyErrorHandler(const char* errorMessage)
{
	wxString errStr;
	errStr << _U(errorMessage);
	wxMessageBox(errStr, _("Uncrustify Error"), wxOK | wxICON_ERROR);
	//CL_DEBUG(errStr.c_str());
}

CodeBeautifier::CodeBeautifier(IManager *manager)
	: IPlugin(manager)
{
	m_longName = wxT("Source Code Formatter (Uncrustify)");
	m_shortName = wxT("CodeBeautifier");
}

CodeBeautifier::~CodeBeautifier()
{
}

void CodeBeautifier::DoFormatFile(IEditor *editor)
{
	long cursorPosition = editor->GetCurrentPosition();

	wxString input;
	bool formatSelectionOnly = !editor->GetSelection().IsEmpty();

	if (formatSelectionOnly) {
		// get the lines contained in the selection
		int selStart   = editor->GetSelectionStart();
		int selEnd     = editor->GetSelectionEnd();
		int lineNumber = editor->LineFromPos(selStart);

		selStart = editor->PosFromLine(lineNumber);
		selEnd = editor->LineEnd( editor->LineFromPos(selEnd) );

		editor->SelectText(selStart, selEnd - selStart);
		input = editor->GetSelection();

	} else {
		input = editor->GetEditorText();
	}

	wxString output = ExecuteUncrustify(input, editor->GetFileName().GetExt());

	if (!output.IsEmpty()) {
		if( formatSelectionOnly ) {
			// format the text (add the indentation)
			output = editor->FormatTextKeepIndent(output, editor->GetSelectionStart(), Format_Text_Default);
			editor->ReplaceSelection(output);

		} else {
			editor->SetEditorText(output);
			editor->SetCaretAt(cursorPosition);
		}
	}
}

wxString CodeBeautifier::ExecuteUncrustify(const wxString &input, const wxString &ext)
{
	wxString pathToBuffer = m_mgr->GetStartupDirectory() + wxT("/uncrustify.buffer");
	WriteFileUTF8(pathToBuffer, input);

	wxCharBuffer configFile = (m_mgr->GetStartupDirectory() + wxT("/uncrustify.cfg")).mb_str();

	const int argumentsCount = 8;
	char ** arguments = new char *[argumentsCount];
	arguments[0] = new char[4];
	strcpy(arguments[0], "-c");

	arguments[1] = new char[configFile.length()];
	strcpy(arguments[1], configFile.data());

	arguments[2] = new char[4];
	strcpy(arguments[2], "-f");

	arguments[3] = new char[pathToBuffer.mb_str().length()];
	strcpy(arguments[3], pathToBuffer.mb_str());

	arguments[4] = new char[4];
	strcpy(arguments[4], "-o");

	arguments[5] = new char[pathToBuffer.mb_str().length()];
	strcpy(arguments[5], pathToBuffer.mb_str());

	arguments[6] = new char[4];
	strcpy(arguments[6], "-l");

	arguments[7] = new char[ext.mb_str().length()];
	strcpy(arguments[7], ext.mb_str());

	UncrustifyExecute(argumentsCount, arguments, UncrustifyErrorHandler);

	wxString output;
	ReadFileWithConversion(pathToBuffer, output);

	for (int i = 0; i < argumentsCount; i++) {
		delete [] arguments[i];
	}

	delete [] arguments;

	return output;
}

clToolBar *CodeBeautifier::CreateToolBar(wxWindow *parent)
{
	return NULL;
}

void CodeBeautifier::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("uncrustify_format_source"), _("Format Current Source"), _("Format Current Source"), wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, _("Code Beautifier"), menu);

	m_mgr->GetTheApp()->Connect(XRCID("uncrustify_format_source"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeBeautifier::OnFormat), NULL, (wxEvtHandler*)this);
}

void CodeBeautifier::UnPlug()
{
	m_mgr->GetTheApp()->Disconnect(XRCID("uncrustify_format_source"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeBeautifier::OnFormat), NULL, (wxEvtHandler*)this);
}

void CodeBeautifier::OnFormat(wxCommandEvent &e)
{
	IEditor *editor(NULL);
	wxString fileToFormat = e.GetString();

	// If we got a file name in the event, use it instead of the active editor
	if(fileToFormat.IsEmpty() == false) {
		editor = m_mgr->FindEditor(fileToFormat);
	} else {
		editor = m_mgr->GetActiveEditor();
	}

	// get the editor that requires formatting
	if (!editor)
		return;

	// Notify about indentation about to start
	wxCommandEvent startEvent(wxEVT_CODEFORMATTER_INDENT_STARTING);
	startEvent.SetString( editor->GetFileName().GetFullPath() );
	EventNotifier::Get()->ProcessEvent(startEvent);

	m_mgr->SetStatusMessage(wxString::Format(wxT("%s: %s..."), _("Formatting"), editor->GetFileName().GetFullPath().c_str()), 0);

	DoFormatFile(editor);

	// Notify that a file was indented
	wxCommandEvent completeEvent(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
	completeEvent.SetString( editor->GetFileName().GetFullPath() );
	EventNotifier::Get()->AddPendingEvent(completeEvent);

	m_mgr->SetStatusMessage(_("Done"), 0);
}
