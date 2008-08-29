#include "dirsaver.h"
#include <wx/app.h>
#include "workspace.h"
#include "globals.h"
#include "externaltooldlg.h"
#include <wx/menu.h>
#include "externaltools.h"
#include <wx/xrc/xmlres.h>
#include <wx/log.h>

static ExternalToolsPlugin* thePlugin = NULL;

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

ExternalToolsPlugin::ExternalToolsPlugin(IManager *manager)
		: IPlugin(manager)
		, topWin(NULL)
{
	m_longName = wxT("A plugin that allows user to launch external tools from within CodeLite");
	m_shortName = wxT("ExternalTools");
	topWin = m_mgr->GetTheApp();
	
	topWin->Connect(XRCID("external_tool_0"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_1"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_2"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_3"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_4"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_5"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_6"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_7"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_8"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Connect(XRCID("external_tool_9"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
}

ExternalToolsPlugin::~ExternalToolsPlugin()
{
	topWin->Disconnect(XRCID("external_tools_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnSettings), NULL, (wxEvtHandler*)this);
	topWin->Disconnect(XRCID("external_tool_0"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_1"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_2"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_3"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_4"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_5"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_6"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_7"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_8"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
	topWin->Disconnect(XRCID("external_tool_9"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ExternalToolsPlugin::OnLaunchExternalTool), NULL, this);
}

wxToolBar *ExternalToolsPlugin::CreateToolBar(wxWindow *parent)
{
	/*
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->SetToolBitmapSize(wxSize(size, size));
	
	// TODO :: insert tools here to the toolbar, notice how the plugin should create code that supports 24x24 and 16x16 icons size
	if (size == 24) {
		tb->AddTool(XRCID("run_unit_tests"), wxT("Run Unit tests..."), wxXmlResource::Get()->LoadBitmap(wxT("run_unit_test24")), wxT("Run project as unit test project..."));
		
	} else {
		tb->AddTool(XRCID("run_unit_tests"), wxT("Run Unit tests..."), wxXmlResource::Get()->LoadBitmap(wxT("run_unit_test16")), wxT("Run project as unit test project..."));
	}
	tb->Realize();

	//Connect the events to us
	// TODO:: connect the events to parent window, but direct them to us using the Connect() method
	parent->Connect(XRCID("run_unit_tests"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnRunUnitTests), NULL, (wxEvtHandler*)this);
	parent->Connect(XRCID("run_unit_tests"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(UnitTestPP::OnRunUnitTestsUI), NULL, (wxEvtHandler*)this);
	return tb;
	*/
	return NULL;
}

void ExternalToolsPlugin::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("external_tools_settings"), wxT("Settings..."), wxEmptyString, wxITEM_NORMAL);
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
	ExternalToolDlg dlg(m_mgr->GetTheApp()->GetTopWindow());
	dlg.SetTools(inData.GetTools());
	
	if(dlg.ShowModal() == wxID_OK){
		//Do something here
		ExternalToolsData data;
		data.SetTools(dlg.GetTools());
		m_mgr->GetConfigTool()->WriteObject(wxT("ExternalTools"), &data);
	}
}

void ExternalToolsPlugin::OnLaunchExternalTool(wxCommandEvent& e)
{
	ExternalToolsData inData;
	m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

	for(size_t i=0; i<inData.GetTools().size(); i++){
		ToolInfo ti = inData.GetTools().at(i);
		if(wxXmlResource::GetXRCID(ti.GetId().c_str()) == e.GetId()){
			DoLaunchTool(ti);
		}
	}
}

void ExternalToolsPlugin::DoLaunchTool(const ToolInfo& ti)
{
	wxString command, working_dir;
	wxString current_file;
	
	if(m_mgr->GetActiveEditor()){
		current_file = m_mgr->GetActiveEditor()->GetFileName().GetFullPath();
	}
	
	command << wxT("\"") << ti.GetPath() << wxT("\" ") << ti.GetArguments();
	working_dir = ti.GetWd();
	
	if(m_mgr->IsWorkspaceOpen()){
		command = ExpandAllVariables(command, m_mgr->GetWorkspace(), m_mgr->GetWorkspace()->GetActiveProjectName(), wxEmptyString, current_file);
		working_dir = ExpandAllVariables(working_dir, m_mgr->GetWorkspace(), m_mgr->GetWorkspace()->GetActiveProjectName(), wxEmptyString, current_file);
	} else {
		command = ExpandAllVariables(command, NULL, wxEmptyString, wxEmptyString, current_file);
		working_dir = ExpandAllVariables(working_dir, NULL, wxEmptyString, wxEmptyString, current_file);
	}
	
	{
		// change the directory to the requested working directory
		DirSaver ds;
		wxSetWorkingDirectory(working_dir);
		wxExecute(command);
	}
}

