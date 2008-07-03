//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : unittestpp.cpp              
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

#include "unittestspage.h"
#include "ctags_manager.h"
#include "unittestcppoutputparser.h"
#include <wx/tokenzr.h>
#include "environmentconfig.h"
#include "pipedprocess.h"
#include "dirsaver.h"
#include "procutils.h"
#include "workspace.h"
#include "project.h"
#include "testclassdlg.h"
#include "newunittestdlg.h"
#include <wx/ffile.h>
#include <wx/msgdlg.h>
#include "unittestpp.h"
#include <wx/app.h>
#include <wx/xrc/xmlres.h>
#include <wx/menuitem.h>
#include <wx/menu.h>

static UnitTestPP* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new UnitTestPP(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxGetUserName());
	info.SetName(wxT("UnitTestPP"));
	info.SetDescription(wxT("A Unit test plugin based on the UnitTest++ framework"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

UnitTestPP::UnitTestPP(IManager *manager)
		: IPlugin(manager)
		, m_sepItem(NULL)
		, m_proc(NULL)
{
	m_longName = wxT("A Unit test plugin based on the UnitTest++ framework");
	m_shortName = wxT("UnitTestPP");
	m_topWindow = m_mgr->GetTheApp();
}

UnitTestPP::~UnitTestPP()
{
}

wxToolBar *UnitTestPP::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(size, size));

	if (size == 24) {
		tb->AddTool(XRCID("run_unit_tests"), wxT("Run Unit tests..."), wxXmlResource::Get()->LoadBitmap(wxT("run_unit_test24")), wxT("Run project as unit test project..."));
		
	} else {
		tb->AddTool(XRCID("run_unit_tests"), wxT("Run Unit tests..."), wxXmlResource::Get()->LoadBitmap(wxT("run_unit_test16")), wxT("Run project as unit test project..."));
	}
	tb->Realize();

	//Connect the events to us
	parent->Connect(XRCID("run_unit_tests"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnRunUnitTests), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("run_unit_tests"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(UnitTestPP::OnRunUnitTestsUI), NULL, (wxEvtHandler*)this);
	return tb;
}

void UnitTestPP::CreatePluginMenu(wxMenu *pluginsMenu)
{
	//TODO:: create the menu for the 'Plugin' menu entry in the
	//menu bar
}

void UnitTestPP::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		m_sepItem = menu->AppendSeparator();
		menu->Append(XRCID("UNITTESTPP_EDITOR_POPUP"), wxT("UnitTest++"), CreateEditorPopMenu());
	}
}

void UnitTestPP::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeEditor) {
		wxMenuItem *item = menu->FindItem(XRCID("UNITTESTPP_EDITOR_POPUP"));
		if (item) {
			menu->Destroy(item);
			m_topWindow->Disconnect(XRCID("unittestpp_new_simple_test"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnNewSimpleTest), NULL, (wxEvtHandler*)this);
			m_topWindow->Disconnect(XRCID("unittestpp_new_class_test"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnNewClassTest), NULL, (wxEvtHandler*)this);
		}

		if (m_sepItem) {
			menu->Destroy(m_sepItem);
			m_sepItem = NULL;
		}
	}
}

void UnitTestPP::UnPlug()
{
	//TODO:: perform the unplug action for this plugin
}


wxMenu *UnitTestPP::CreateEditorPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("unittestpp_new_simple_test"), wxT("Create new &test..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("unittestpp_new_class_test"), wxT("Create tests for &class..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	//connect the events
	m_topWindow->Connect(XRCID("unittestpp_new_simple_test"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnNewSimpleTest), NULL, (wxEvtHandler*)this);
	m_topWindow->Connect(XRCID("unittestpp_new_class_test"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnNewClassTest), NULL, (wxEvtHandler*)this);
	return menu;
}

void UnitTestPP::OnNewClassTest(wxCommandEvent& e)
{
	wxUnusedVar(e);
	
	//position has changed, compare line numbers
	wxString clsName;
	IEditor *editor = m_mgr->GetActiveEditor();
	if(editor) {
		int line = editor->GetCurrentLine();
		TagEntryPtr tag = m_mgr->GetTagsManager()->FunctionFromFileLine(editor->GetFileName(), line+1);
		if( tag && tag->GetScope().IsEmpty() == false && tag->GetScope() != wxT("<global>") ) {
			clsName = tag->GetScope();
		}
	}
	
	TestClassDlg *dlg = new TestClassDlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg->SetClassName(clsName);
	
	if (dlg->ShowModal() == wxID_OK) {
		wxArrayString arr = dlg->GetTestsList();
		wxString fixture = dlg->GetFixtureName();
		wxString filename = dlg->GetFileName();
		dlg->Destroy();
		
		wxFileName fn(filename);

		// first open / create the target file
		if ( wxFileName::FileExists(filename) == false ) {
			// the file does not exist!
			wxFFile file(filename, wxT("wb"));
			if ( !file.IsOpened() ) {
				wxMessageBox(wxString::Format(wxT("Could not create target file '%s'"), filename.c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
				return;
			}
			
			// since this is a new file, it will most probably will need the include file
			file.Write(wxT("#include <UnitTest++.h>\n"));
			file.Close();
			
			TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileView);
			wxString file_name;
			if(m_mgr->GetActiveEditor()) {
				file_name = m_mgr->GetActiveEditor()->GetFileName().GetFullPath();
			}
			
			wxString fp = item.m_fileName.GetFullPath();
			if (item.m_item.IsOk() && fp == file_name) {
				wxTreeItemId parentItem = m_mgr->GetTree(TreeFileView)->GetItemParent(item.m_item);
				wxArrayString paths;
				paths.Add(fn.GetFullPath());
				if (m_mgr->AddFilesToVirtualFodler(parentItem, paths) == false) {
					//probably not a virtual folder
					wxString msg;
					msg << wxT("CodeLite created the test file successfully, but was unable to add the generated file to any virtual folder\n");
					msg << wxT("You can right click on virtual folder (in the 'Workspace' tab) and manually add them\n");
					wxMessageBox(msg, wxT("CodeLite"), wxOK|wxICON_INFORMATION);
				}
			}
		}

		// file name exist
		// open the file in the editor
		m_mgr->OpenFile(filename, wxEmptyString);


		IEditor *editor = m_mgr->GetActiveEditor();
		if (!editor || (editor && editor->GetFileName().GetFullPath() != fn.GetFullPath())) {
			wxMessageBox(wxString::Format(wxT("Could not open target file '%s'"), filename.c_str()), wxT("CodeLite"), wxICON_WARNING|wxOK);
			return;
		}

		for (size_t i=0; i<arr.GetCount(); i++) {
			// Create the test in the format of:
			// Test<FuncName>
			wxString name = arr.Item(i);
			wxString prefix = name.Mid(0, 1);

			name = name.Mid(1);
			prefix.MakeUpper();
			prefix << name;

			if ( fixture.IsEmpty() ) {
				DoCreateSimpleTest(wxT("Test") + prefix, editor);
			} else {
				DoCreateFixtureTest(wxT("Test") + prefix, fixture, editor);
			}
		}
	} else {
		// wxID_CANCEL
		dlg->Destroy();
	}
}

void UnitTestPP::OnNewSimpleTest(wxCommandEvent& e)
{
	wxUnusedVar(e);
	NewUnitTestDlg *dlg = new NewUnitTestDlg(m_mgr->GetTheApp()->GetTopWindow());
	if (dlg->ShowModal() == wxID_OK) {
		// create the unit test
		wxString testName = dlg->GetTestName();
		wxString fixture  = dlg->GetFixtureName();

		if (dlg->UseFixture()) {
			DoCreateFixtureTest(testName, fixture, m_mgr->GetActiveEditor());
		} else {
			DoCreateSimpleTest(testName, m_mgr->GetActiveEditor());
		}
	}
	dlg->Destroy();
}

void UnitTestPP::DoCreateFixtureTest(const wxString& name, const wxString& fixture, IEditor *editor)
{
	wxString text;

	text << wxT("\nTEST_FIXTURE(") << fixture << wxT(", ") << name << wxT(")\n");
	text << wxT("{\n");
	text << wxT("}\n");

	if (editor) {
		editor->AppendText(text);
	}
}

void UnitTestPP::DoCreateSimpleTest(const wxString& name, IEditor *editor)
{
	wxString text;

	text << wxT("\nTEST(") << name << wxT(")\n");
	text << wxT("{\n");
	text << wxT("}\n");

	if (editor) {
		editor->AppendText(text);
	}
}

void UnitTestPP::OnRunUnitTests(wxCommandEvent& e)
{
	wxString projectName = m_mgr->GetWorkspace()->GetActiveProjectName();
	wxString wd, err_msg;
	wxString cmd = m_mgr->GetProjectExecutionCommand(projectName, wd);

	DirSaver ds;
	
	// first we need to CD to the project directory
	ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projectName, err_msg);
	if(p) {
		::wxSetWorkingDirectory(p->GetFileName().GetPath());
	}
	
	// now change the directory 
	wxSetWorkingDirectory(wd);
	
	//m_proc will be deleted upon termination
	m_proc = new PipedProcess(wxNewId(), cmd);
	if (m_proc) {

		//set the environment variables
		m_mgr->GetEnv()->ApplyEnv();

		if (m_proc->Start() == 0) {

			//set the environment variables
			m_mgr->GetEnv()->UnApplyEnv();

			//failed to start the process
			delete m_proc;
			m_proc = NULL;
			return;
		}
		m_mgr->GetEnv()->UnApplyEnv();
		m_proc->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(UnitTestPP::OnProcessTerminated), NULL, this);
	}
}

void UnitTestPP::OnRunUnitTestsUI(wxUpdateUIEvent& e)
{
	e.Enable(m_mgr->IsWorkspaceOpen() && !m_proc);
}

void UnitTestPP::OnProcessTerminated(wxProcessEvent& e)
{
	wxString output;
	m_proc->ReadAll(output);
	delete m_proc;
	m_proc = NULL;

	wxArrayString arr = wxStringTokenize(output, wxT("\r\n"));
	UnitTestCppOutputParser parser(arr);
	
	// parse the results
	TestSummary summary;
	parser.Parse(summary);
	
	// create new report page, and add it to the editor
	static int counter(1);
	
	if(summary.totalTests == 0){
		return;
	}
	
	UnitTestsPage *page = new UnitTestsPage(m_mgr->GetMainNotebook(), summary, m_mgr);
	m_mgr->GetMainNotebook()->AddPage(page, wxString::Format(wxT("UnitTest++ Report <%d>"), counter), wxNullBitmap, true);
	counter++;
	
	wxString msg;
	double errCount = summary.errorCount;
	double totalTests = summary.totalTests;
	
	double err_percent = (errCount / totalTests)*100;
	double pass_percent = ((totalTests - errCount) / totalTests)*100;
	msg << err_percent << wxT("%");
	page->UpdateFailedBar((size_t)summary.errorCount, msg);
	
	msg.clear();
	msg << pass_percent << wxT("%");
	page->UpdatePassedBar((size_t)(summary.totalTests - summary.errorCount), msg);
	
}
