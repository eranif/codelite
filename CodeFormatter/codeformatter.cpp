//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : codeformatter.cpp              
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
 #include "globals.h"
#include "codeformatter.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h> //wxInitialize/wxUnInitialize
#include "wx/ffile.h"
#include "formatoptions.h"
#include "wx/log.h"
#include "codeformatterdlg.h"
#include "wx/menu.h"
#include "precompiled_header.h"


extern "C" EXPORT char* STDCALL
	AStyleMain(const char* pSourceIn,
	           const char* pOptions,
	           void(STDCALL *fpError)(int, const char*),
	           char*(STDCALL *fpAlloc)(unsigned long));


//------------------------------------------------------------------------
// Astyle functions required by AStyleLib
// Error handler for the Artistic Style formatter
void  STDCALL ASErrorHandler(int errorNumber, const char* errorMessage)
{
	wxString errStr;
	errStr << _U(errorMessage) << wxT(" (error ") << errorNumber << wxT(")");
	wxLogMessage(errStr);
}

// Allocate memory for the Artistic Style formatter
char* STDCALL ASMemoryAlloc(unsigned long memoryNeeded)
{
	// error condition is checked after return from AStyleMain
	char* buffer = new char [memoryNeeded];
	return buffer;
}
//------------------------------------------------------------------------
static CodeFormatter *theFormatter = NULL;

//Allocate the code formatter on the heap, it will be freed by
//the application
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (theFormatter == 0) {
		theFormatter = new CodeFormatter(manager);
	}
	return theFormatter;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("CodeFormatter"));
	info.SetDescription(wxT("Source Code Formatter based on the open source AStyle tool"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

CodeFormatter::CodeFormatter(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("Source Code Formatter (AStyle)");
	m_shortName = wxT("CodeFormatter");
}

CodeFormatter::~CodeFormatter()
{
}

wxToolBar *CodeFormatter::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(size, size));

	if (size == 24) {
		tb->AddTool(XRCID("format_source"), wxT("Format Source"), wxXmlResource::Get()->LoadBitmap(wxT("code_format24")), wxT("Format Source Code"));
		tb->AddTool(XRCID("formatter_options"), wxT("Format Options"), wxXmlResource::Get()->LoadBitmap(wxT("code_format_options24")), wxT("Source Code Formatter Options..."));
	} else {
		//16
		tb->AddTool(XRCID("format_source"), wxT("Format Source"), wxXmlResource::Get()->LoadBitmap(wxT("code_format16")), wxT("Format Source Code"));
		tb->AddTool(XRCID("formatter_options"), wxT("Format Options"), wxXmlResource::Get()->LoadBitmap(wxT("code_format_options16")), wxT("Source Code Formatter Options..."));
	}

#if defined (__WXMAC__)
	tb->AddSeparator();
#endif
	tb->Realize();

	//Connect the events to us
	parent->Connect(XRCID("format_source"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeFormatter::OnFormat), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("formatter_options"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CodeFormatter::OnFormatOptions), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("format_source"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeFormatter::OnFormatUI), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("formatter_options"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(CodeFormatter::OnFormatOptionsUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void CodeFormatter::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("format_source"), wxT("Format Current Source\tCtrl+I"), wxT("Format Current Source"), wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("formatter_options"), wxT("Options..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Source Code Formatter"), menu);
}

void CodeFormatter::OnFormat(wxCommandEvent &e)
{
	wxUnusedVar(e);
	IEditor *editor = m_mgr->GetActiveEditor();
	if (!editor)
		return;

	DoFormatFile(editor);
}

void CodeFormatter::DoFormatFile(IEditor *editor)
{
	long curpos = editor->GetCurrentPosition();
	//execute the formatter
	FormatOptions fmtroptions;
	m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);
	wxString options = fmtroptions.ToString();

	wxString output;
	AstyleFormat(editor->GetEditorText(), options, output);
	if (output.IsEmpty() == false) {
		editor->SetEditorText(output);
		editor->SetCaretAt(curpos);
	}
}

void CodeFormatter::AstyleFormat(const wxString &input, const wxString &options, wxString &output)
{
	char *textOut = AStyleMain(_C(input), _C(options), ASErrorHandler, ASMemoryAlloc);
	if (textOut) {
		output = _U(textOut);
		output.Trim();
		output << wxT("\n");
		delete [] textOut;
	}
}

void CodeFormatter::OnFormatOptions(wxCommandEvent &e)
{
	//load options from settings file
	FormatOptions fmtroptions;
	m_mgr->GetConfigTool()->ReadObject(wxT("FormatterOptions"), &fmtroptions);

	wxString sampleFile;
	wxString content;
	sampleFile << m_mgr->GetStartupDirectory() << wxT("/astyle.sample");
	ReadFileWithConversion(sampleFile, content);

	CodeFormatterDlg *dlg = new CodeFormatterDlg(NULL, this, fmtroptions.GetOptions(), content);
	if (dlg->ShowModal() == wxID_OK) {
		//update the settings on the configuration file
		fmtroptions.SetOption(dlg->GetOptions());
		m_mgr->GetConfigTool()->WriteObject(wxT("FormatterOptions"), &fmtroptions);
	}

	dlg->Destroy();
	wxUnusedVar(e);
}

void CodeFormatter::OnFormatUI(wxUpdateUIEvent &e)
{
	e.Enable(m_mgr->GetActiveEditor() != NULL);
}

void CodeFormatter::OnFormatOptionsUI(wxUpdateUIEvent &e)
{
	e.Enable(true);
}

void CodeFormatter::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(type);
	wxUnusedVar(menu);
}

void CodeFormatter::UnPlug()
{
	//nothing to do
}
