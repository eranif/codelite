//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : externaltools.cpp              
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

#include "environmentconfig.h"
#include <wx/aui/framemanager.h>
#include "async_executable_cmd.h"
#include <wx/bitmap.h>
#include "dirsaver.h"
#include <wx/app.h>
#include "workspace.h"
#include "globals.h"
#include "externaltooldlg.h"
#include <wx/menu.h>
#include "externaltools.h"
#include <wx/xrc/xmlres.h>
#include <wx/log.h>
#include <algorithm>
#include "plugin_version.h"

static ExternalToolsPlugin* thePlugin = NULL;

struct DecSort {
	bool operator()(const ToolInfo &t1, const ToolInfo &t2) {
		return t1.GetName().CmpNoCase(t2.GetName()) > 0;
	}
};

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new ExternalToolsPlugin(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("ExternalTools"));
	info.SetDescription(wxT("A plugin that allows user to launch external tools from within CodeLite"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

BEGIN_EVENT_TABLE(ExternalToolsPlugin, IPlugin)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE,    ExternalToolsPlugin::OnToolProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDERRLINE, ExternalToolsPlugin::OnToolProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED,    ExternalToolsPlugin::OnToolProcess)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, 	   ExternalToolsPlugin::OnToolProcess)
END_EVENT_TABLE()

ExternalToolsPlugin::ExternalToolsPlugin(IManager *manager)
		: IPlugin(manager)
		, topWin(NULL)
		, m_pipedProcess(NULL)
{
	m_longName = wxT("A plugin that allows user to launch external tools from within CodeLite");
	m_shortName = wxT("ExternalTools");
	topWin = m_mgr->GetTheApp();

	topWin->Connect(XRCID("stop_external_tool"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnStopExternalTool), NULL, (wxEvtHandler*)this);
	topWin->Connect(XRCID("stop_external_tool"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ExternalToolsPlugin::OnStopExternalToolUI), NULL, (wxEvtHandler*)this);

	for (int i=0; i<MAX_TOOLS; i++) {
		wxString winid = wxString::Format(wxT("external_tool_%d"), i);
		topWin->Connect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
		topWin->Connect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ExternalToolsPlugin::OnLaunchExternalToolUI), NULL, this);
	}
}

ExternalToolsPlugin::~ExternalToolsPlugin()
{
	topWin->Disconnect(XRCID("external_tools_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnSettings), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("stop_external_tool"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnStopExternalTool), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("stop_external_tool"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ExternalToolsPlugin::OnStopExternalToolUI), NULL, (wxEvtHandler*)this);

	for (int i=0; i<MAX_TOOLS; i++) {
		wxString winid = wxString::Format(wxT("external_tool_%d"), i);
		topWin->Disconnect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
		topWin->Disconnect(wxXmlResource::GetXRCID(winid.c_str()), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ExternalToolsPlugin::OnLaunchExternalToolUI), NULL, this);
	}
}

wxToolBar *ExternalToolsPlugin::CreateToolBar(wxWindow *parent)
{
	//support both toolbars icon size
	m_tb = NULL;
	if (m_mgr->AllowToolbar()) {
		int size = m_mgr->GetToolbarIconSize();

		m_tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
		m_tb->SetToolBitmapSize(wxSize(size, size));

		ExternalToolsData inData;
		m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

		if (size == 24) {
			m_tb->AddTool(XRCID("external_tools_settings"), wxT("Configure external tools..."), wxXmlResource::Get()->LoadBitmap(wxT("configure_ext_tools24")), wxT("Configure external tools..."));
			m_tb->AddTool(XRCID("stop_external_tool"), wxT("Stop external tool"), wxXmlResource::Get()->LoadBitmap(wxT("stop_external_tool24")), wxT("Stop external tool"));
		} else {
			m_tb->AddTool(XRCID("external_tools_settings"), wxT("Configure external tools..."), wxXmlResource::Get()->LoadBitmap(wxT("configure_ext_tools16")), wxT("Configure external tools..."));
			m_tb->AddTool(XRCID("stop_external_tool"), wxT("Stop external tool"), wxXmlResource::Get()->LoadBitmap(wxT("stop_external_tool16")), wxT("Stop external tool"));
		}
		m_tb->AddSeparator();

		std::vector<ToolInfo> tools = inData.GetTools();
		std::sort(tools.begin(), tools.end(), DecSort());

		for (size_t i=0; i<tools.size(); i++) {
			ToolInfo ti = tools.at(i);

			wxFileName icon24(ti.GetIcon24());
			wxFileName icon16(ti.GetIcon16());

			if (size == 24) {
				wxBitmap bmp(wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default24")));
				if (icon24.FileExists()) {
					bmp.LoadFile(icon24.GetFullPath(), wxBITMAP_TYPE_PNG);
					if (bmp.IsOk() == false) {
						bmp = wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default24"));
					}
				}
				m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), bmp, ti.GetName());

			} else if (size == 16) {
				wxBitmap bmp(wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default16")));
				if (icon16.FileExists()) {
					bmp.LoadFile(icon16.GetFullPath(), wxBITMAP_TYPE_PNG);
					if (bmp.IsOk() == false) {
						bmp = wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default16"));
					}
				}

				m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), bmp, ti.GetName());
			}
		}

		m_tb->Realize();
	}

	return m_tb;
}

void ExternalToolsPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("external_tools_settings"), wxT("Configure external tools..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("External Tools"), menu);

	topWin->Connect(XRCID("external_tools_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnSettings), NULL, (wxEvtHandler*)this);
}

void ExternalToolsPlugin::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void ExternalToolsPlugin::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void ExternalToolsPlugin::UnPlug()
{
}

void ExternalToolsPlugin::OnSettings(wxCommandEvent& e)
{
	ExternalToolsData inData;
	m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);
	ExternalToolDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.SetTools(inData.GetTools());

	if (dlg.ShowModal() == wxID_OK) {
		//Do something here
		ExternalToolsData data;
		data.SetTools(dlg.GetTools());
		m_mgr->GetConfigTool()->WriteObject(wxT("ExternalTools"), &data);

		DoRecreateToolbar();
	}
}

void ExternalToolsPlugin::OnLaunchExternalTool(wxCommandEvent& e)
{
	ExternalToolsData inData;
	m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

	for (size_t i=0; i<inData.GetTools().size(); i++) {
		ToolInfo ti = inData.GetTools().at(i);
		if (wxXmlResource::GetXRCID(ti.GetId().c_str()) == e.GetId()) {
			DoLaunchTool(ti);
		}
	}
}

void ExternalToolsPlugin::DoLaunchTool(const ToolInfo& ti)
{
	wxString command, working_dir;
	wxString current_file;

	if (m_mgr->GetActiveEditor()) {
		current_file = m_mgr->GetActiveEditor()->GetFileName().GetFullPath();
	}

	command << wxT("\"") << ti.GetPath() << wxT("\" ") << ti.GetArguments();
	working_dir = ti.GetWd();

	if (m_mgr->IsWorkspaceOpen()) {
		command = ExpandAllVariables(command, m_mgr->GetWorkspace(), m_mgr->GetWorkspace()->GetActiveProjectName(), wxEmptyString, current_file);
		working_dir = ExpandAllVariables(working_dir, m_mgr->GetWorkspace(), m_mgr->GetWorkspace()->GetActiveProjectName(), wxEmptyString, current_file);
	} else {
		command = ExpandAllVariables(command, NULL, wxEmptyString, wxEmptyString, current_file);
		working_dir = ExpandAllVariables(working_dir, NULL, wxEmptyString, wxEmptyString, current_file);
	}

	// check to see if we require to save all files before continuing
	if (ti.GetSaveAllFiles()) {
		m_mgr->SaveAll();
	}

	if (ti.GetCaptureOutput() == false) {
		// change the directory to the requested working directory
		DirSaver ds;
		wxSetWorkingDirectory(working_dir);

		// apply environment
		m_mgr->GetEnv()->ApplyEnv(NULL);
		wxExecute(command);
		m_mgr->GetEnv()->UnApplyEnv();

	} else {
		// create a piped process
		if (m_pipedProcess && m_pipedProcess->IsBusy()) {
			// a process is already running
			return;
		}

		m_pipedProcess = new AsyncExeCmd(this);
		m_mgr->GetEnv()->ApplyEnv(NULL);

		DirSaver ds;
		wxSetWorkingDirectory(working_dir);

		// hide console if any,
		// redirect output
		m_pipedProcess->Execute(command, true, true);
		if (m_pipedProcess->GetProcess()) {
			// select the 'Output' tab
			SelectOutputTab();

			// success
			m_pipedProcess->GetProcess()->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(ExternalToolsPlugin::OnProcessEnd), NULL, this);
		}
		m_mgr->GetEnv()->UnApplyEnv();
	}
}

void ExternalToolsPlugin::OnProcessEnd(wxProcessEvent &event)
{
	m_pipedProcess->ProcessEnd(event);
	m_pipedProcess->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(ExternalToolsPlugin::OnProcessEnd), NULL, this);
	delete m_pipedProcess;
	m_pipedProcess = NULL;
}

void ExternalToolsPlugin::DoRecreateToolbar()
{
	wxWindow *parent(NULL);
	if (m_tb) {
		// we have a toolbar, remove it from the docking manager
		m_mgr->GetDockingManager()->DetachPane(m_tb);
		parent = m_tb->GetParent();
		m_tb->Destroy();
	} else {
		parent = m_mgr->GetTheApp()->GetTopWindow();
	}

	m_tb = CreateToolBar(parent);
	if (m_tb) {
		m_mgr->GetDockingManager()->AddPane(m_tb, wxAuiPaneInfo().Name(GetShortName()).LeftDockable( true ).RightDockable( true ).Caption(GetShortName()).ToolbarPane().Top() );

		// Apply changes
		m_mgr->GetDockingManager()->Update();
	}
}

bool ExternalToolsPlugin::IsRedirectedToolRunning()
{
	if (m_pipedProcess == NULL)
		return false;

	return (m_pipedProcess && m_pipedProcess->IsBusy());
}

void ExternalToolsPlugin::OnToolProcess(wxCommandEvent& event)
{
	wxString msg(event.GetString());
	msg = msg.Trim().Trim(false);
	m_mgr->AppendOutputMsg(msg + wxT("\n"));
}

void ExternalToolsPlugin::OnLaunchExternalToolUI(wxUpdateUIEvent& e)
{
	e.Enable(!IsRedirectedToolRunning());
}

void ExternalToolsPlugin::SelectOutputTab()
{
	// make sure the Ouput view is visible
	wxAuiPaneInfo &info = m_mgr->GetDockingManager()->GetPane(wxT("Output View"));
	if (info.IsOk() && !info.IsShown()) {
		info.Show();
		m_mgr->GetDockingManager()->Update();
	}

	Notebook *book = m_mgr->GetOutputPaneNotebook();
	wxString curSel = book->GetPageText((size_t)book->GetSelection());
	if (curSel == wxT("Output")) {
		return;
	}

	for (size_t i=0; i<(size_t)book->GetPageCount(); i++) {
		if (book->GetPageText(i) == wxT("Output")) {
			book->SetSelection(i);
			break;
		}
	}
}

void ExternalToolsPlugin::OnStopExternalTool(wxCommandEvent& e)
{
	if (m_pipedProcess) {
		m_pipedProcess->Stop();
	}
}

void ExternalToolsPlugin::OnStopExternalToolUI(wxUpdateUIEvent& e)
{
	e.Enable(this->IsRedirectedToolRunning());
}
