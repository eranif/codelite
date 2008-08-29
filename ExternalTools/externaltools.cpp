#include <wx/aui/framemanager.h>
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
	//support both toolbars icon size
	int size = m_mgr->GetToolbarIconSize();

	m_tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	m_tb->SetToolBitmapSize(wxSize(size, size));

	ExternalToolsData inData;
	m_mgr->GetConfigTool()->ReadObject(wxT("ExternalTools"), &inData);

	if (size == 24) {
		m_tb->AddTool(XRCID("external_tools_settings"), wxT("Configure external tools..."), wxXmlResource::Get()->LoadBitmap(wxT("configure_ext_tools24")), wxT("Configure external tools..."));
	} else {
		m_tb->AddTool(XRCID("external_tools_settings"), wxT("Configure external tools..."), wxXmlResource::Get()->LoadBitmap(wxT("configure_ext_tools16")), wxT("Configure external tools..."));
	}
	m_tb->AddSeparator();
	
	for (size_t i=0; i<inData.GetTools().size(); i++) {
		ToolInfo ti = inData.GetTools().at(i);

		wxFileName icon24(ti.GetIcon24());
		wxFileName icon16(ti.GetIcon16());

		if (size == 24) {
			wxBitmap bmp(wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default24")));
			if(icon24.FileExists()){
				bmp.LoadFile(icon24.GetFullPath(), wxBITMAP_TYPE_PNG);
				if(bmp.IsOk() == false){
					bmp = wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default24"));
				}
			}
			m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), bmp, ti.GetName());
			
		} else if (size == 16) {
			wxBitmap bmp(wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default16")));
			if(icon16.FileExists()){
				bmp.LoadFile(icon16.GetFullPath(), wxBITMAP_TYPE_PNG);
				if(bmp.IsOk() == false){
					bmp = wxXmlResource::Get()->LoadBitmap(wxT("ext_tool_default16"));
				}
			}

			m_tb->AddTool(wxXmlResource::GetXRCID(ti.GetId().c_str()), ti.GetName(), bmp, ti.GetName());
		}
	}

	m_tb->Realize();
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
	ExternalToolDlg dlg(m_mgr->GetTheApp()->GetTopWindow());
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

	{
		// change the directory to the requested working directory
		DirSaver ds;
		wxSetWorkingDirectory(working_dir);
		wxExecute(command);
	}
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
	m_mgr->GetDockingManager()->AddPane(m_tb, wxAuiPaneInfo().Name(GetShortName()).LeftDockable( true ).RightDockable( true ).Caption(GetShortName()).ToolbarPane().Top() );

	// Apply changes
	m_mgr->GetDockingManager()->Update();
}
