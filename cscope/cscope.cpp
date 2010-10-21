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
#include "csscopeconfdata.h"
#include "bitmap_loader.h"
#include <wx/app.h>
#include <wx/textdlg.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include "cscopestatusmessage.h"
#include "dirsaver.h"
#include "cscope.h"
#include <wx/aui/framemanager.h>
#include "procutils.h"
#include "wx/ffile.h"
#include "workspace.h"
#include <wx/xrc/xmlres.h>
#include "exelocator.h"
#include "cscopetab.h"
#include "cscopedbbuilderthread.h"
#include <wx/imaglist.h>

static Cscope* thePlugin = NULL;

static const wxString CSCOPE_NAME = wxT("CScope");

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
	info.SetAuthor(wxT("Eran Ifrah, patched by Stefan Roesch"));
	info.SetName(CSCOPE_NAME);
	info.SetDescription(wxT("CScope Integration for CodeLite"));
	info.SetVersion(wxT("v1.1"));
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
	m_shortName = CSCOPE_NAME;
	m_topWindow = m_mgr->GetTheApp();

	m_cscopeWin = new CscopeTab(m_mgr->GetOutputPaneNotebook(), m_mgr);
	m_mgr->GetOutputPaneNotebook()->AddPage(m_cscopeWin, CSCOPE_NAME, false, wxXmlResource::Get()->LoadBitmap(wxT("cscope")));

	Connect(wxEVT_CSCOPE_THREAD_DONE, wxCommandEventHandler(Cscope::OnCScopeThreadEnded), NULL, this);
	Connect(wxEVT_CSCOPE_THREAD_UPDATE_STATUS, wxCommandEventHandler(Cscope::OnCScopeThreadUpdateStatus), NULL, this);

	//start the helper thread
	CScopeThreadST::Get()->Start();
}

Cscope::~Cscope()
{
}

clToolBar *Cscope::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	clToolBar *tb = NULL;
	if (m_mgr->AllowToolbar()) {
		tb = new clToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, clTB_DEFAULT_STYLE);
		tb->SetToolBitmapSize(wxSize(size, size));

		// Sample code that adds single button to the toolbar
		// and associates an image to it
		BitmapLoader *bitmapLoader = m_mgr->GetStdIcons();
		if (size == 24) {
			//use the large icons set
			tb->AddTool(XRCID("cscope_find_symbol"),                       wxT("Find this C symbol"),                     bitmapLoader->LoadBitmap(wxT("toolbars/24/cscope/find_symbol")),                       wxT("Find this C symbol"));
			tb->AddTool(XRCID("cscope_functions_calling_this_function"),   wxT("Find functions calling this function"),   bitmapLoader->LoadBitmap(wxT("toolbars/24/cscope/function_calling_this_function")),    wxT("Find functions calling this function"));
			tb->AddTool(XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function"), bitmapLoader->LoadBitmap(wxT("toolbars/24/cscope/functions_called_by_this_function")), wxT("Find functions called by this function"));
		} else {
			//16
			tb->AddTool(XRCID("cscope_find_symbol"),                       wxT("Find this C symbol"),                     bitmapLoader->LoadBitmap(wxT("toolbars/16/cscope/find_symbol")),                       wxT("Find this C symbol"));
			tb->AddTool(XRCID("cscope_functions_calling_this_function"),   wxT("Find functions calling this function"),   bitmapLoader->LoadBitmap(wxT("toolbars/16/cscope/function_calling_this_function")),    wxT("Find functions calling this function"));
			tb->AddTool(XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function"), bitmapLoader->LoadBitmap(wxT("toolbars/16/cscope/functions_called_by_this_function")), wxT("Find functions called by this function"));
		}
		tb->Realize();
	}

	// Command events
	m_topWindow->Connect( XRCID("cscope_find_global_definition"),            wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this );
	m_topWindow->Connect( XRCID("cscope_create_db"),                         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this );
	m_topWindow->Connect( XRCID("cscope_settings"),                          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnDoSettings), NULL, (wxEvtHandler*)this );
	m_topWindow->Connect( XRCID("cscope_functions_calling_this_function"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_find_symbol"),                       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_find_user_symbol"),                  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindUserInsertedSymbol), NULL, (wxEvtHandler*)this );
	m_topWindow->Connect( XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);

	// UI events
	m_topWindow->Connect( XRCID("cscope_functions_called_by_this_function"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_create_db"),                         wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_functions_calling_this_function"),   wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_find_global_definition"),            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_find_symbol"),                       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Connect( XRCID("cscope_find_user_symbol"),                  wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void Cscope::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("cscope_find_user_symbol"), wxT("Find ..."), wxT("Find ..."), wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("cscope_find_symbol"), wxT("Find selected text"), wxT("Find this C symbol"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_find_global_definition"), wxT("Find this global definition"), wxT("Find this C global definition"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_called_by_this_function"), wxT("Find functions called by this function"), wxT("Find functions called by this function"), wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("cscope_functions_calling_this_function"), wxT("Find functions calling this function"), wxT("Find functions calling this function"), wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("cscope_create_db"), wxT("Create CScope database"), wxT("Create/Recreate the cscope database"), wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("cscope_settings"), wxT("CScope settings"), wxT("Configure cscope"), wxITEM_NORMAL);
	menu->Append(item);

	pluginsMenu->Append(wxID_ANY, CSCOPE_NAME, menu);
}

void Cscope::HookPopupMenu(wxMenu *menu, MenuType type)
{
	//at first, we hook cscope into the editor's context menu
	if (type == MenuTypeEditor) {
		menu->Append(XRCID("CSCOPE_EDITOR_POPUP"), CSCOPE_NAME, CreateEditorPopMenu());
	}
}

void Cscope::UnPlug()
{
	m_topWindow->Disconnect( XRCID("cscope_functions_called_by_this_function"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect( XRCID("cscope_create_db"),                         wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnWorkspaceOpenUI), NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect( XRCID("cscope_functions_calling_this_function"),   wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect( XRCID("cscope_find_global_definition"),            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect( XRCID("cscope_find_symbol"),                       wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Cscope::OnCscopeUI),        NULL, (wxEvtHandler*)this);
	
	m_topWindow->Disconnect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	m_topWindow->Disconnect(XRCID("cscope_create_db"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this);

	// before this plugin is un-plugged we must remove the tab we added
	for (size_t i=0; i<m_mgr->GetOutputPaneNotebook()->GetPageCount(); i++) {
		if (m_cscopeWin == m_mgr->GetOutputPaneNotebook()->GetPage(i)) {
			m_mgr->GetOutputPaneNotebook()->RemovePage(i);
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

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("cscope_create_db"), wxT("Create CScope database"), wxT("Create/Recreate the cscope database"), wxITEM_NORMAL);
	menu->Append(item);

	//connect the events
	m_topWindow->Connect(XRCID("cscope_find_symbol"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindSymbol), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_find_global_definition"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindGlobalDefinition), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_called_by_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCalledByThisFuncion), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_functions_calling_this_function"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnFindFunctionsCallingThisFunction), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("cscope_create_db"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Cscope::OnCreateDB), NULL, (wxEvtHandler*)this);
	return menu;
}

wxString Cscope::DoCreateListFile(bool force)
{
	// get the scope
	CScopeConfData settings;
	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);

	//create temporary file and save the file there
	wxString wspPath = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	wxString list_file( wspPath );
	list_file << wxT("cscope_file.list");

	if (force || settings.GetRebuildOption() || !::wxFileExists(list_file))
	{
		wxArrayString projects;
		m_mgr->GetWorkspace()->GetProjectList(projects);
		wxString err_msg;
		std::vector< wxFileName > files;
		std::vector< wxFileName > tmpfiles;
		m_cscopeWin->SetMessage(wxT("Creating file list..."), 5);

		if (settings.GetScanScope() == SCOPE_ENTIRE_WORKSPACE) {
			for (size_t i=0; i< projects.GetCount(); i++) {
				ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), err_msg);
				if ( proj ) {
					proj->GetFiles(tmpfiles, true);
				}
			}
		} else {
			// SCOPE_ACTIVE_PROJECT
			wxString projName = m_mgr->GetWorkspace()->GetActiveProjectName();
			ProjectPtr proj = m_mgr->GetWorkspace()->FindProjectByName(projName, err_msg);
			if ( proj ) {
				proj->GetFiles(tmpfiles, true);
			}
		}

		//iterate over the files and convert them to be relative path
		for (size_t i=0; i< tmpfiles.size(); i++ ) {
			tmpfiles.at(i).MakeRelativeTo(wspPath);
			files.push_back(tmpfiles.at(i));
		}

		//create temporary file and save the file there
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
	}

	return list_file;
}

void Cscope::DoCscopeCommand(const wxString &command, const wxString &findWhat, const wxString &endMsg)
{
	// We haven't yet found a valid cscope exe, so look for one
	wxString where;
	if ( !ExeLocator::Locate( GetCscopeExeName(), where ) ) {
		wxString msg; msg << _("I can't find 'cscope' anywhere. Please check if it's installed.") << wxT('\n')
						  << _("Or tell me where it can be found, from the menu: 'Plugins | CScope | Settings'");
		wxMessageBox( msg, _("CScope not found"), wxOK|wxCENTER|wxICON_WARNING );
		return;
	}

	//try to locate the cscope database
	wxArrayString output;

	//set the focus to the cscope tab
	wxBookCtrlBase *book = m_mgr->GetOutputPaneNotebook();

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
	if (curSel != CSCOPE_NAME) {
		for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
			if (book->GetPageText(i) == CSCOPE_NAME) {
				book->SetSelection(i);
				break;
			}
		}
	}

	//create the search thread and return
	CscopeRequest *req = new CscopeRequest();
	req->SetOwner     (this    );
	req->SetCmd       (command );
	req->SetEndMsg    (endMsg  );
	req->SetFindWhat  (findWhat);
	req->SetWorkingDir(m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR));

	CScopeThreadST::Get()->Add( req );
}

void Cscope::OnFindSymbol(wxCommandEvent &e)
{
	// sanity
	if ( m_mgr->GetActiveEditor() == NULL ) {
		return;
	}

	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	DoFindSymbol(word);
}

void Cscope::OnFindGlobalDefinition(wxCommandEvent &e)
{
	// sanity
	if ( m_mgr->GetActiveEditor() == NULL ) {
		return;
	}

	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}
	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile(false);

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -d -L -1 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: find global definition of '") << word << wxT("'");
	DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnFindFunctionsCalledByThisFuncion(wxCommandEvent &e)
{
	// sanity
	if ( m_mgr->GetActiveEditor() == NULL ) {
		return;
	}

	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile(false);

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -d -L -2 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: functions called by '") << word << wxT("'");
	DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnFindFunctionsCallingThisFunction(wxCommandEvent &e)
{
	wxString word = m_mgr->GetActiveEditor()->GetWordAtCaret();
	if (word.IsEmpty()) {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile(false);

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << wxT(" -d -L -3 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: functions calling '") << word << wxT("'");
	DoCscopeCommand(command, word, endMsg);
}

void Cscope::OnCreateDB(wxCommandEvent &e)
{
	// sanity
	if ( m_mgr->IsWorkspaceOpen() == false )  {
		return;
	}

	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile(true);

	// get the reverted index option
	wxString command;
	wxString endMsg;
	CScopeConfData settings;

	command << GetCscopeExeName();

	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
	if (settings.GetBuildRevertedIndexOption())
	{
		command << wxT(" -q");
		endMsg << wxT("Recreated inverted CScope DB");
	}
	else
	{
		command << wxT(" -b");
		endMsg << wxT("Recreated CScope DB");
	}

	// Do the actual create db
	// since the process is always running from the workspace
	// directory, there is no need to specify the full path of the list file
	command << wxT(" -L -i cscope_file.list");
	DoCscopeCommand(command, wxEmptyString, endMsg);
}

void Cscope::OnDoSettings(wxCommandEvent &e)
{
	// atm the only setting to set is the cscope filepath
	// First find the current value, if any
	CScopeConfData settings;
	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
	wxString filepath = settings.GetCscopeExe();

	// Since there's only the one thing to ask, keep it simple for now
	wxString fp = wxGetTextFromUser(_("Please enter the filepath where cscope can be found"), _("Where is cscope?"), filepath);
	if ( fp.IsEmpty() ) {
		return;
	}

	settings.SetCscopeExe(fp);
	m_mgr->GetConfigTool()->WriteObject(wxT("CscopeSettings"), &settings);
}

wxString Cscope::GetCscopeExeName()
{
	CScopeConfData settings;
	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
	return settings.GetCscopeExe();
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

		if( msg->GetFindWhat().IsEmpty() == false ) {
			m_cscopeWin->SetFindWhat(msg->GetFindWhat());
		}
		delete msg;
	}
	e.Skip();
}

void Cscope::OnCscopeUI(wxUpdateUIEvent &e)
{
	CHECK_CL_SHUTDOWN();
	bool isEditor = m_mgr->GetActiveEditor() ? true : false;
	e.Enable(m_mgr->IsWorkspaceOpen() && isEditor);
}

void Cscope::OnWorkspaceOpenUI(wxUpdateUIEvent& e)
{
	CHECK_CL_SHUTDOWN();
	e.Enable(m_mgr->IsWorkspaceOpen());
}

void Cscope::OnFindUserInsertedSymbol(wxCommandEvent& e)
{
	CHECK_CL_SHUTDOWN();

	wxString word = wxGetTextFromUser(wxT("Find What:"), wxT("cscope: find symbol"), wxT(""), m_mgr->GetTheApp()->GetTopWindow());
	if(word.IsEmpty())
		return;

	DoFindSymbol( word );
}

void Cscope::DoFindSymbol(const wxString& word)
{
	m_cscopeWin->Clear();
	wxString list_file = DoCreateListFile(false);

	// get the rebuild option
	wxString rebuildOption = wxT("");
	CScopeConfData settings;

	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &settings);
	if (!settings.GetRebuildOption()) {
		rebuildOption = wxT(" -d");
	}

	//Do the actual search
	wxString command;
	wxString endMsg;
	command << GetCscopeExeName() << rebuildOption << wxT(" -L -0 ") << word << wxT(" -i ") << list_file;
	endMsg << wxT("cscope results for: find C symbol '") << word << wxT("'");
	DoCscopeCommand(command, word, endMsg);
}
