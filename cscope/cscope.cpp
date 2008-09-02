//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscope.cpp
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
#include "cscopestatusmessage.h"
#include "dirsaver.h"
#include "cscope.h"
#include <wx/aui/framemanager.h>
#include "procutils.h"
#include "wx/ffile.h"
#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include "cscopetab.h"
#include "cscopedbbuilderthread.h"

static Cscope* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new Cscope(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("cscope"));
	info.SetDescription(wxT("Cscope Integration for CodeLite"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

Cscope::Cscope(IManager *manager)
		: IPlugin(manager)
		, m_topWindow(NULL)
{
	m_longName = wxT("Cscope Integration for CodeLite");
	m_shortName = wxT("Cscope");
	m_topWindow = m_mgr->GetTheApp();

	m_cscopeWin = new CscopeTab(m_mgr->GetOutputPaneNotebook(), m_mgr);
	m_mgr->GetOutputPaneNotebook()->AddPage(m_cscopeWin, wxT("cscope"), wxXmlResource::Get()->LoadBitmap(wxT("cscope")), false);

	Connect(wxEVT_CSCOPE_THREAD_DONE, wxCommandEventHandler(Cscope::OnCScopeThreadEnded), NULL, this);
	Connect(wxEVT_CSCOPE_THREAD_UPDATE_STATUS, wxCommandEventHandler(Cscope::OnCScopeThreadUpdateStatus), NULL, this);

	//start the helper thread
	CScopeThreadST::Get()->Start();
}

Cscope::~Cscope()
{
}

wxToolBar *Cscope::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	wxToolBar *tb = NULL;
	tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(size, size));

	// Sample code that adds single button to the toolbar
	// and associates an image to it
	if (size == 24) {
		//use the large icons set
		tb->AddTool(XRCID("cscope_find_symbol"), wxT("Find this C symbol"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_find_symbol24")), wxT("Find this C symbol"));
		tb->AddTool(XRCID("cscope_functions_calling_this_function"), wxT("Find functions calling this function"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_func_called24")), wxT("Find functions calling this function"));
		tb->AddTool(XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_func_calling24")), wxT("Find functions called by this function"));
	} else {
		//16
		tb->AddTool(XRCID("cscope_find_symbol"), wxT("Find this C symbol"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_find_symbol16")), wxT("Find this C symbol"));
		tb->AddTool(XRCID("cscope_functions_calling_this_function"), wxT("Find functions calling this function"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_func_called16")), wxT("Find functions calling this function"));
		tb->AddTool(XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function"), wxXmlResource::Get()->LoadBitmap(wxT("cscope_func_calling16")), wxT("Find functions called by this function"));
	}
	tb->Realize();

	// Connect the events to us
	parent->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("cscope_find_symbol"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void Cscope::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), wxT("Find this C symbol\tCtrl+0"), wxT("Find this C symbol"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function\tCtrl+2"), wxT("Find functions called by this function"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), wxT("Find functions calling this function\tCtrl+3"), wxT("Find functions calling this function"), wxITEM_NORMAL);
	menu->Append(item);

	pluginsMenu->Append(wxID_ANY, wxT("cscope"), menu);
}

void Cscope::HookPopupMenu(wxMenu *menu, MenuType type)
{
	//at first, we hook cscope into the editor's context menu
	if (type == MenuTypeEditor) {
		m_sepItem = menu->AppendSeparator();
		menu->Append(XRCID("CSCOPE_EDITOR_POPUP"), wxT("cscope"), CreateEditorPopMenu());
	}
}

void Cscope::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("CSCOPE_EDITOR_POPUP"));
		if (item) {
			menu->Destroy(item);
			m_topWindow->Disconnect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
		}

		if (m_sepItem) {
			menu->Destroy(m_sepItem);
			m_sepItem = NULL;
		}
	}
}

void Cscope::UnPlug()
{
	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_cscopeWin == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
			m_mgr->GetOutputPaneNotebook()->RemovePage(i, false);
			m_cscopeWin->Destroy();
			break;
		}
	}

	CScopeThreadST::Get()->Stop();
	CScopeThreadST::Free();
}

//---------------------------------------------------------------------------------
wxMenu *Cscope::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), wxT("&Find this C symbol"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), wxT("Find this &global definition"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"), wxT("Find functions &called by this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), wxT("Fi&nd functions calling this function"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	//connect the events
	m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	return menu;
}

wxString Cscope::DoCreateListFile()
{
	wxArrayString projects;
	m_mgr->GetWorkspace()->GetProjectList(projects);
	wxString err_msg;
	std::vector< wxFileName > files;
	std::vector< wxFileName > tmpfiles;
	m_cscopeWin->SetMessage(wxT("Creating file list..."), 5);

	for (size_t i=0; i< projects.GetCount(); i++) {
		ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
		if ( proj ) {
			proj->GetFiles(tmpfiles, true);
		}
	}

	//iterate over the files and convert them to be relative path
	wxString wspPath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	for (size_t i=0; i< tmpfiles.size(); i++ ) {
		tmpfiles.at(i).MakeRelativeTo(wspPath);
		files.push_back(tmpfiles.at(i));
	}

	//create temporary file and save the file there
	wxString list_file( wspPath );
	list_file << wxT("cscope_file.list");

	wxFFile file(list_file, wxT("w+b"));
	if (!file.IsOpened()) {
		wxLogMessage(wxT("Failed to open temporary file ") + list_file);
		return wxEmptyString;
	}

	//write the content of the files into the tempfile
	wxString content;
	for (size_t i=0; i< files.size(); i++) {
		wxFileName fn(files.at(i));
		content << fn.GetFullPath() << wxT("\n");
	}

	file.Write( content );
	file.Flush();
	file.Close();
	return list_file;
}

void Cscope::DoCscopeCommand(const wxString &command, const wxString &endMsg)
{
	//try to locate the cscope database
	wxArrayString output;

	//set the focus to the cscope tab
	Notebook *book = m_mgr->GetOutputPaneNotebook();

	//make sure that the Output pane is visible
	wxAuiManager *aui = m_mgr->GetDockingManager();
	if (aui) {
		wxAuiPaneInfo &info = aui->GetPane(wxT("Output View"));
		if (info.IsOk() && !info.IsShown()) {
			info.Show();
			aui->Update();
		}
	}

	wxString curSel = book->GetPageText((size_t)book->GetSelection());
	if (curSel != wxT("cscope")) {
		for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
			if (book->GetPageText(i) == wxT("cscope")) {
				book->SetSelection(i);
				break;
			}
		}
	}

	//create the search thread and return
	CscopeRequest *req = new CscopeRequest();
	req->SetOwner(this);
	req->SetCmd(command);
	req->SetEndMsg(endMsg);
	req->SetWorkingDir(m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));

	CScopeThreadST::Get()->Add( req );
}

void Cscope::OnFindSymbol(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -L -0 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: find C symbol '") << word << wxT("':");
	DoCscopeCommand(command, endMsg);
}

void Cscope::OnFindGlobalDefinition(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}
	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -L -1 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: find global definition of '") << word << wxT("':");
	DoCscopeCommand(command, endMsg);
}

void Cscope::OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -L -2 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: functions called by '") << word << wxT("':");
	DoCscopeCommand(command, endMsg);
}

void Cscope::OnFindFunctionsCallingThisFunction(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile();

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -L -3 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: functions calling '") << word << wxT("':");
	DoCscopeCommand(command, endMsg);
}

wxString Cscope::GetCscopeExeName()
{
	return wxT("cscope ");
}

void Cscope::OnCScopeThreadEnded(wxCommandEvent &e)
{
	CscopeResultTable *result = (CscopeResultTable*)e.GetClientData();
	m_cscopeWin->BuildTable( result );
}

void Cscope::OnCScopeThreadUpdateStatus(wxCommandEvent &e)
{
	CScopeStatusMessage *msg = (CScopeStatusMessage *)e.GetClientData();
	if (msg) {
		m_cscopeWin->SetMessage(msg->GetMessage(), msg->GetPercentage());
		delete msg;
	}
	e.Skip();
}

void Cscope::OnCscopeUI(wxUpdateUIEvent &e)
{
	bool isEditor = m_mgr->GetActiveEditor() ? true : false;
	e.Enable(m_mgr->IsWorkspaceOpen() && isEditor);
}
