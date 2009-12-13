#include "subversion2.h"
#include "svn_login_dialog.h"
#include "svn_command_handlers.h"
#include <wx/textdlg.h>
#include "commit_dialog.h"
#include "svnstatushandler.h"
#include "subversion_strings.h"
#include <wx/menu.h>
#include <wx/app.h>
#include "svn_preferences_dialog.h"
#include "svn_console.h"
#include "subversion_page.h"
#include <wx/xrc/xmlres.h>
#include <wx/menuitem.h>
#include <wx/menu.h>

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
		: IPlugin          (manager)
		, m_explorerSepItem(NULL)
{
	m_longName = wxT("Subversion plugin for codelite2.0 based on the svn command line tool");
	m_shortName = wxT("Subversion2");

	DoInitialize();

	GetManager()->GetTheApp()->Connect(XRCID("subversion2_settings"),  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings), NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_commit"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),   NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_update"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),   NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_add"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),      NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_delete"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),   NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_revert"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),   NULL, this);
	Connect(XRCID("svn_commit2"), wxCommandEventHandler(Subversion2::OnCommit2), NULL, this);
}

Subversion2::~Subversion2()
{
	GetManager()->GetTheApp()->Disconnect(XRCID("subversion2_settings"),  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings), NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_commit"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),   NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_update"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),   NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_add"),      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),      NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_delete"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),   NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_revert"),   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),   NULL, this);
	Disconnect(XRCID("svn_commit2"), wxCommandEventHandler(Subversion2::OnCommit2), NULL, this);
}

wxToolBar *Subversion2::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	// Create the toolbar to be used by the plugin
	wxToolBar *tb(NULL);
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
	if (type == MenuTypeFileExplorer) {
		if (!menu->FindItem(XRCID("SUBVERSION_EXPLORER_POPUP"))) {
			m_explorerSepItem = menu->PrependSeparator();
			menu->Prepend(XRCID("SUBVERSION_EXPLORER_POPUP"), wxT("Subversion"), CreateFileExplorerPopMenu());
		}
	}
}
wxMenu* Subversion2::CreateFileExplorerPopMenu()
{
	//Create the popup menu for the file explorer
	//The only menu that we are interseted is the file explorer menu
	wxMenu* menu = new wxMenu();
	wxMenuItem *item(NULL);

	item = new wxMenuItem(menu, XRCID("svn_explorer_commit"), wxT("Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_update"), wxT("Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_add"), wxT("Add"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_delete"), wxT("Delete"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_revert"), wxT("Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	return menu;
}

void Subversion2::UnHookPopupMenu(wxMenu *menu, MenuType type)
{
	wxMenuItem *item = menu->FindItem(XRCID("SUBVERSION_EXPLORER_POPUP"));
	if (item) {
		menu->Destroy(item);
		menu->Destroy(m_explorerSepItem);
		m_explorerSepItem = NULL;
	}
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
	m_subversionPage = new SubversionView(book, this);

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

////////////////////////////////////////////////
// File Explorer SVN command handlers
////////////////////////////////////////////////

void Subversion2::OnAdd(wxCommandEvent& event)
{
	wxString command;
	command << GetSvnExeName(true) << wxT(" --recursive add \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnStatusHandler(this));
}

void Subversion2::OnCommit(wxCommandEvent& event)
{
	wxString comment = wxGetTextFromUser(wxT("Enter Commit Message"), wxT("Svn Commit"));
	comment = CommitDialog::NormalizeMessage(comment);

	wxString command;
	command << GetSvnExeName(true) << wxT(" commit \"") << DoGetFileExplorerItemFullPath() << wxT("\" -m \"") << comment << wxT("\"");
	GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnCommitHandler(this, this));
}

void Subversion2::OnDelete(wxCommandEvent& event)
{
	wxString command;
	command << GetSvnExeName(false) << wxT(" delete --force \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnDefaultCommandHandler(this));
}

void Subversion2::OnRevert(wxCommandEvent& event)
{
	wxString command;
	command << GetSvnExeName(false) << wxT(" revert --recursive \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnDefaultCommandHandler(this));
}

void Subversion2::OnUpdate(wxCommandEvent& event)
{
	wxString command;
	command << GetSvnExeName(false) << wxT(" update \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnUpdateHandler(this));
}

void Subversion2::OnCommit2(wxCommandEvent& event)
{
	SvnLoginDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
	if (dlg.ShowModal() == wxID_OK) {
		wxString command;
		command << GetSvnExeName(true) << wxT(" commit --username ") << dlg.GetUsername() << wxT(" --password ") << dlg.GetPassword() << wxT(" ");

		wxString comment = wxGetTextFromUser(wxT("Enter Commit Message"), wxT("Svn Commit"));
		comment = CommitDialog::NormalizeMessage(comment);

		command << wxT(" \"") << DoGetFileExplorerItemFullPath() << wxT("\" -m \"") << comment << wxT("\"");
		GetShell()->Execute(command, DoGetFileExplorerItemPath(), new SvnCommitHandler(this, this));
	}

}

wxString Subversion2::GetSvnExeName(bool includeIgnoreList)
{
	SvnSettingsData ssd = GetSettings();
	wxString executeable;
	bool encloseQuotations = false;
	wxString exeName = ssd.GetExecutable();
	exeName.Trim().Trim(false);
	encloseQuotations = (exeName.Find(wxT(" ")) != wxNOT_FOUND);
	if (encloseQuotations) {
		executeable << wxT("\"") << ssd.GetExecutable() << wxT("\" --non-interactive --trust-server-cert ");
	} else {
		executeable << ssd.GetExecutable() << wxT(" --non-interactive --trust-server-cert ");
	}

	if (includeIgnoreList) {
		executeable << wxT(" --config-option config:miscellany:global-ignores=\"") << ssd.GetIgnoreFilePattern() << wxT("\" ");
	}
	return executeable;
}

wxString Subversion2::DoGetFileExplorerItemFullPath()
{
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	wxString filename ( item.m_fileName.GetFullPath() );
	filename.Trim().Trim(false);

	if(filename.EndsWith(wxT("\\"))) {
		filename.RemoveLast();

	} else if(filename.EndsWith(wxT("/"))) {
		filename.RemoveLast();

	}
	return filename;
}

wxString Subversion2::DoGetFileExplorerItemPath()
{
	TreeItemInfo item = m_mgr->GetSelectedTreeItemInfo(TreeFileExplorer);
	return item.m_fileName.GetPath();
}
