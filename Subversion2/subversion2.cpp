#include "subversion2.h"
#include "subversion_strings.h"
#include <wx/menu.h>
#include <wx/app.h>
#include "svn_preferences_dialog.h"
#include "svn_console.h"
#include "subversion_page.h"
#include <wx/xrc/xmlres.h>

static Subversion2* thePlugin = NULL;

//Define the plugin entry point
extern "C" EXPORT IPlugin *CreatePlugin(IManager *manager)
{
	if (thePlugin == 0) {
		thePlugin = new Subversion2(manager);
	}
	return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
	PluginInfo info;
	info.SetAuthor(wxT("eran"));
	info.SetName(wxT("Subversion2"));
	info.SetDescription(wxT("Subversion plugin for codelite2.0 based on the svn command line tool"));
	info.SetVersion(wxT("v1.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

Subversion2::Subversion2(IManager *manager)
		: IPlugin(manager)
{
	m_longName = wxT("Subversion plugin for codelite2.0 based on the svn command line tool");
	m_shortName = wxT("Subversion2");

	GetManager()->GetTheApp()->Connect(XRCID("subversion2_settings"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings), NULL, this);
	DoInitialize();
}

Subversion2::~Subversion2()
{
}

wxToolBar *Subversion2::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	// Create the toolbar to be used by the plugin
	wxToolBar *tb(NULL);

	/*
		// You can use the below code a snippet:
	 	// First, check that CodeLite allows plugin to register plugins
		if (m_mgr->AllowToolbar()) {
			// Support both toolbars icon size
			int size = m_mgr->GetToolbarIconSize();

			// Allocate new toolbar, which will be freed later by CodeLite
			tb = new wxToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);

			// Set the toolbar size
			tb->SetToolBitmapSize(wxSize(size, size));

			// Add tools to the plugins toolbar. You must provide 2 sets of icons: 24x24 and 16x16
			if (size == 24) {
				tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin24")), wxT("New Plugin Wizard..."));
				tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class24")), wxT("New Class..."));
				tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project24")), wxT("New wxWidget Project"));
			} else {
				tb->AddTool(XRCID("new_plugin"), wxT("New CodeLite Plugin Project"), wxXmlResource::Get()->LoadBitmap(wxT("plugin16")), wxT("New Plugin Wizard..."));
				tb->AddTool(XRCID("new_class"), wxT("Create New Class"), wxXmlResource::Get()->LoadBitmap(wxT("class16")), wxT("New Class..."));
				tb->AddTool(XRCID("new_wx_project"), wxT("New wxWidget Project"), wxXmlResource::Get()->LoadBitmap(wxT("new_wx_project16")), wxT("New wxWidget Project"));
			}
			// And finally, we must call 'Realize()'
			tb->Realize();
		}

		// return the toolbar, it can be NULL if CodeLite does not allow plugins to register toolbars
		// or in case the plugin simply does not require toolbar
	*/
	return tb;
}

void Subversion2::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxUnusedVar(pluginsMenu);

	// You can use the below code a snippet:
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("subversion2_settings"), _("Subversion Options"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, _("Subversion2"), menu);
}

void Subversion2::HookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void Subversion2::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	wxUnusedVar(menu);
	wxUnusedVar(type);
}

void Subversion2::UnPlug()
{
	// Remove the tab pined to the workspcae pane
	size_t index(Notebook::npos);
	index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_subversionPage);
	if (index != Notebook::npos) {
		m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
	}

	// Remove the tab pined to the output pane
	index = m_mgr->GetOutputPaneNotebook()->GetPageIndex(m_subversionShell);
	if (index != Notebook::npos) {
		m_mgr->GetOutputPaneNotebook()->RemovePage(index, false);
	}

	m_subversionPage->Destroy();
	m_subversionShell->Destroy();
}

void Subversion2::DoInitialize()
{
	Notebook *book = m_mgr->GetWorkspacePaneNotebook();
	m_subversionPage = new SubversionPage(book, this);

	book->AddPage(m_subversionPage, svnCONSOLE_TEXT, svnCONSOLE_TEXT);
	book = m_mgr->GetOutputPaneNotebook();
	m_subversionShell = new SvnConsole(book, this);

	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("svn_repo"));
	book->AddPage(m_subversionShell, svnCONSOLE_TEXT, svnCONSOLE_TEXT, bmp);
	
	DoSetSSH();
}

SvnSettingsData Subversion2::GetSettings()
{
	SvnSettingsData ssd;
	GetManager()->GetConfigTool()->ReadObject(wxT("SvnSettingsData"), &ssd);
	return ssd;
}

void Subversion2::SetSettings(SvnSettingsData& ssd)
{
	GetManager()->GetConfigTool()->WriteObject(wxT("SvnSettingsData"), &ssd);
}

void Subversion2::OnSettings(wxCommandEvent& event)
{
	SvnPreferencesDialog dlg(GetManager()->GetTheApp()->GetTopWindow(), this);
	if (dlg.ShowModal() == wxID_OK) {
		// Update the Subversion view
		GetSvnPage()->BuildTree();
		DoSetSSH();
	}
}

void Subversion2::DoSetSSH()
{
	wxString sshClient     = GetSettings().GetSshClient();
	wxString sshClientArgs = GetSettings().GetSshClientArgs();

	sshClient.Trim().Trim(false);
	sshClientArgs.Trim().Trim(false);

	// on Windows, SVN demands that the ssh client will not contain any
	// backward slashes
	sshClient.Replace(wxT("\\"), wxT("/"));

	if (sshClient.IsEmpty() == false) {
		wxString env_value(sshClient + wxT(" ") + sshClientArgs);
		wxSetEnv(wxT("SVN_SSH"), env_value.c_str());
	}
}
