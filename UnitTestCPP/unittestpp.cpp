#include "newunittestdlg.h"
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

UnitTestPP::UnitTestPP(IManager *manager)
		: IPlugin(manager)
		, m_sepItem(NULL)
{
	m_longName = wxT("A Unit test plugin based on the UnitTest++ framework");
	m_shortName = wxT("UnitTestPP");
	m_topWindow = wxTheApp;
}

UnitTestPP::~UnitTestPP()
{
}

wxToolBar *UnitTestPP::CreateToolBar(wxWindow *parent)
{
	wxToolBar *tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	// TODO :: Add your toolbar items here...
	// Sample code that adds single button to the toolbar
	// and associates an image to it
	// tb->AddTool(XRCID("new_plugin"), wxT("New Plugin Wizard..."), wxXmlResource::Get()->LoadBitmap(wxT("plugin_add")), wxT("New Plugin Wizard..."));
	// tb->Realize();
	// Connect the events to us
	// parent->Connect(XRCID("new_plugin"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UnitTestPP::OnNewPlugin), NULL, (wxEvtHandler*)this);
	// parent->Connect(XRCID("new_plugin"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(UnitTestPP::OnNewPluginUI), NULL, (wxEvtHandler*)this);
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
	wxMessageBox(wxT("Not implementd yet, be patient :)"));
}

void UnitTestPP::OnNewSimpleTest(wxCommandEvent& e)
{
	wxUnusedVar(e);
	NewUnitTestDlg *dlg = new NewUnitTestDlg(wxTheApp->GetTopWindow());
	if(dlg->ShowModal() == wxID_OK) {
		// create the unit test
		wxString testName = dlg->GetTestName();
		wxString fixture  = dlg->GetFixtureName();
		
		if(dlg->UseFixture()) {
			DoCreateFixtureTest(testName, fixture);
		} else {
			DoCreateSimpleTest(testName);
		}
	}
	dlg->Destroy();
}

void UnitTestPP::DoCreateFixtureTest(const wxString& name, const wxString& fixture)
{
	wxString text;
	
	text << wxT("TEST_FIXTURE(") << fixture << wxT(", ") << name << wxT(")\n");
	text << wxT("{\n");
	text << wxT("}\n");
	
	if(m_mgr->GetActiveEditor()) {
		m_mgr->GetActiveEditor()->AppendText(text);
	}
}

void UnitTestPP::DoCreateSimpleTest(const wxString& name)
{
	wxString text;
	
	text << wxT("TEST(") << name << wxT(")\n");
	text << wxT("{\n");
	text << wxT("}\n");
	
	if(m_mgr->GetActiveEditor()) {
		m_mgr->GetActiveEditor()->AppendText(text);
	}
}

