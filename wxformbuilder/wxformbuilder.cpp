#include "formbuildsettingsdlg.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include "wxformbuilder.h"
#include <wx/xrc/xmlres.h>

static wxFormBuilder* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new wxFormBuilder(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("wxFormBuilder"));
	info.SetDescription(wxT("wxFormBuilder integration with CodeLite"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

wxFormBuilder::wxFormBuilder(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("wxFormBuilder integration with CodeLite");
	m_shortName = wxT("wxFormBuilder");
	m_topWin = m_mgr->GetTheApp();
}

wxFormBuilder::~wxFormBuilder()
{
}

wxToolBar *wxFormBuilder::CreateToolBar(wxWindow *parent)
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

void wxFormBuilder::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxMenu *menu = new wxMenu();
	wxMenuItem *item( NULL );

	item = new wxMenuItem( menu, XRCID("wxfb_settings"), wxT( "Settings..." ), wxT( "Settings..." ), wxITEM_NORMAL );
	menu->Append( item );

	pluginsMenu->Append(wxID_ANY, wxT("wxFormBuilder"), menu);

	m_topWin->Connect( XRCID("wxfb_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxFormBuilder::OnSettings ), NULL, this );
}

void wxFormBuilder::HookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Workspace) {
	}
}

void wxFormBuilder::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	if (type == MenuTypeFileView_Workspace) {
	}
}

void wxFormBuilder::UnPlug()
{
}

void wxFormBuilder::OnSettings(wxCommandEvent& e)
{
	FormBuildSettingsDlg dlg(m_mgr->GetTheApp()->GetTopWindow(), m_mgr);
	dlg.ShowModal();
}
