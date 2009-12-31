#include <wx/app.h>
#include "subversion_password_db.h"
#include "svnxml.h"
#include <wx/tokenzr.h>
#include "detachedpanesinfo.h"
#include "dockablepane.h"
#include "subversion2.h"
#include "procutils.h"
#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include "svn_login_dialog.h"
#include "svn_command_handlers.h"
#include <wx/textdlg.h>
#include "commit_dialog.h"
#include "svnstatushandler.h"
#include "subversion_strings.h"
#include <wx/menu.h>
#include "svn_preferences_dialog.h"
#include "svn_console.h"
#include "subversion_view.h"
#include <wx/xrc/xmlres.h>
#include <wx/menuitem.h>
#include <wx/menu.h>
#include <wx/filedlg.h>

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
	info.SetAuthor(wxT("Eran Ifrah"));
	info.SetName(wxT("Subversion"));
	info.SetDescription(wxT("Subversion plugin for codelite2.0 based on the svn command line tool"));
	info.SetVersion(wxT("v2.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

Subversion2::Subversion2(IManager *manager)
		: IPlugin          (manager)
		, m_explorerSepItem(NULL)
		, m_svnClientVersion(0.0)
{
	m_longName = wxT("Subversion plugin for codelite2.0 based on the svn command line tool");
	m_shortName = wxT("Subversion2");

	DoInitialize();

	GetManager()->GetTheApp()->Connect(XRCID("subversion2_settings"),             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings),          NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_commit"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_update"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_add"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),               NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_delete"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_revert"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_patch"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnPatch),             NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_diff"),                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDiff),              NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_log"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnLog),               NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_blame"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnBlame),             NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFile),        NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_set_as_view"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSelectAsView),      NULL, this);
	
	GetManager()->GetTheApp()->Connect(wxEVT_GET_ADDITIONAL_COMPILEFLAGS, wxCommandEventHandler(Subversion2::OnGetCompileLine), NULL, this);
}

Subversion2::~Subversion2()
{
	GetManager()->GetTheApp()->Disconnect(XRCID("subversion2_settings"),             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings),          NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_commit"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_update"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_add"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),               NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_delete"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_revert"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_patch"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnPatch),             NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_diff"),                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDiff),              NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_log"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnLog),               NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_blame"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnBlame),             NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFile),        NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_set_as_view"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSelectAsView),      NULL, this);
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

	item = new wxMenuItem(menu, XRCID("svn_explorer_set_as_view"), wxT("Display this path in the Subverion View"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_update"), wxT("Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_commit"), wxT("Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_add"), wxT("Add"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_delete"), wxT("Delete"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_revert"), wxT("Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_diff"), wxT("Create Diff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	item = new wxMenuItem(menu, XRCID("svn_explorer_patch"), wxT("Apply Patch"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_blame"), wxT("Blame ..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_explorer_log"), wxT("Change Log..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	wxMenu *subMenu;
	subMenu = new wxMenu;
	subMenu->Append(XRCID("svn_explorer_ignore_file"),         wxT("Ignore this file"));
	subMenu->Append(XRCID("svn_explorer_ignore_file_pattern"), wxT("Ignore this file pattern"));
	menu->Append(wxID_ANY, wxT("Ignore"), subMenu);

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
	index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_subversionView);
	if (index != Notebook::npos) {
		m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
	}

	// Remove the tab pined to the output pane
	index = m_mgr->GetOutputPaneNotebook()->GetPageIndex(m_subversionConsole);
	if (index != Notebook::npos) {
		m_mgr->GetOutputPaneNotebook()->RemovePage(index, false);
	}

	m_subversionView->Destroy();
	m_subversionConsole->Destroy();
}

void Subversion2::DoInitialize()
{
	// create tab (possibly detached)
	Notebook *book = m_mgr->GetWorkspacePaneNotebook();
	m_subversionView = new SubversionView(book, this);
	if( IsSubversionViewDetached() ) {
		// Make the window child of the main panel (which is the parent of the notebook)
		new DockablePane(book->GetParent()->GetParent(), book, m_subversionView, svnCONSOLE_TEXT, wxNullBitmap, wxSize(200, 200));

	} else {

		book->AddPage(m_subversionView, svnCONSOLE_TEXT, svnCONSOLE_TEXT, wxNullBitmap, true);
	}

	book = m_mgr->GetOutputPaneNotebook();
	m_subversionConsole = new SvnConsole(book, this);

	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("svn_repo"));
	book->AddPage(m_subversionConsole, svnCONSOLE_TEXT, svnCONSOLE_TEXT, bmp);

	DoSetSSH();
	// We need to perform a dummy call to svn so it will create all the default
	// setup directory layout
	wxString      command;
	wxArrayString output;

	command << GetSvnExeName(false) << wxT(" --help ");

	ProcUtils::ExecuteCommand(command, output);
	UpdateIgnorePatterns();
	DoGetSvnVersion();
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
	wxUnusedVar(event);
	EditSettings();
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
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString << wxT(" --recursive add \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnStatusHandler(this, event.GetId(), this));
}

void Subversion2::OnCommit(wxCommandEvent& event)
{
	wxString comment = wxGetTextFromUser(wxT("Enter Commit Message"), wxT("Svn Commit"), wxT(""), GetManager()->GetTheApp()->GetTopWindow());
	comment = CommitDialog::NormalizeMessage(comment);

	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString << wxT(" commit \"") << DoGetFileExplorerItemFullPath() << wxT("\" -m \"") << comment << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnCommitHandler(this, event.GetId(), this));
}

void Subversion2::OnDelete(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString << wxT(" delete --force \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnRevert(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}

	command << GetSvnExeName(false) << loginString << wxT(" revert --recursive \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnUpdate(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString << wxT(" update \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnUpdateHandler(this, event.GetId(), this));
}

void Subversion2::OnDiff(wxCommandEvent& event)
{
	wxString diffAgainst(wxT("BASE"));
	diffAgainst = wxGetTextFromUser(wxT("Insert base revision to diff against:"), wxT("Diff against"), wxT("BASE"), GetManager()->GetTheApp()->GetTopWindow());
	if (diffAgainst.empty()) {
		// user clicked 'Cancel'
		diffAgainst = wxT("BASE");
	}

	bool     useExtDiff = GetSettings().GetFlags() & SvnUseExternalDiff;
	wxString extDiff    = GetSettings().GetExternalDiffViewer();
	extDiff.Trim().Trim(false);

	// Only use external diff viewer when the selected file is equal to 1 and the selection is a file!
	if ( useExtDiff && extDiff.IsEmpty() == false) {
		wxString extDiffCmd = GetSettings().GetExternalDiffViewerCommand();

		// export BASE revision of file to tmp file
		const wxString& base = wxFileName::CreateTempFileName( wxT("svnExport"), (wxFile*)NULL );
		::wxRemoveFile( base ); // just want the name, not the file.

		wxString exportCmd;
		wxString loginString;
		if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
			return;
		}
		bool nonInteractive = GetNonInteractiveMode(event);
		exportCmd << GetSvnExeName(nonInteractive) << loginString;
		exportCmd << wxT("export -r ") << diffAgainst << wxT(" \"") << DoGetFileExplorerItemFullPath() << wxT("\" ") << base;

		// Launch export command
		wxArrayString output;
		ProcUtils::ExecuteCommand(exportCmd, output);

		// We now got 2 files:
		// m_selectionInfo.m_paths.Item(0) and 'base'
		extDiffCmd.Replace(wxT("$(MyFile)"),       wxString::Format( wxT("\"%s\""), DoGetFileExplorerItemFullPath().c_str()));
		extDiffCmd.Replace(wxT("$(OriginalFile)"), wxString::Format( wxT("\"%s\""), base.c_str()));

		wxString command;
		command << wxT("\"") << extDiff << wxT("\" ") << extDiffCmd;

		// Launch the external diff
		GetConsole()->AppendText(command + wxT("\n"));
		m_diffCommand.Execute(command, DoGetFileExplorerItemPath(), NULL);

	} else {
		// Simple diff
		wxString command;
		wxString loginString;
		if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
			return;
		}
		bool nonInteractive = GetNonInteractiveMode(event);
		command << GetSvnExeName(nonInteractive) << loginString << wxT("diff -r") << diffAgainst;
		GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnDiffHandler(this, event.GetId(), this), false);
	}
}

void Subversion2::OnPatch(wxCommandEvent& event)
{
	Patch(false, DoGetFileExplorerItemPath(), this, event.GetId());
}

wxString Subversion2::GetSvnExeName(bool nonInteractive)
{
	SvnSettingsData ssd = GetSettings();
	wxString executeable;
	bool encloseQuotations = false;
	wxString exeName = ssd.GetExecutable();
	exeName.Trim().Trim(false);
	encloseQuotations = (exeName.Find(wxT(" ")) != wxNOT_FOUND);
	if (encloseQuotations) {
		executeable << wxT("\"") << ssd.GetExecutable() << wxT("\" ");
	} else {
		executeable << ssd.GetExecutable() << wxT(" ");
	}

	if(nonInteractive)
		executeable << wxT(" --non-interactive ");

	// --trust-server-cert was introduced in version >=1.6
	// but it also requires --non-interactive mode enabled
	if(m_svnClientVersion >= 1.6 && nonInteractive) {
		executeable << wxT(" --trust-server-cert ");
	}

	executeable << wxT(" --config-dir \"") << GetUserConfigDir() << wxT("\" ");
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

wxString Subversion2::GetUserConfigDir()
{
	wxString configDir(wxStandardPaths::Get().GetUserDataDir());
	if(wxFileName::DirExists(configDir) == false) {
		wxMkdir(configDir);
	}

	configDir << wxFileName::GetPathSeparator() << wxT("subversion");
	return configDir;
}

void Subversion2::UpdateIgnorePatterns()
{
	wxString configFile;
	wxString configDir = GetUserConfigDir();
	configFile << configDir << wxFileName::GetPathSeparator() << wxT("config");

	// Convert any whitespace to space
	wxString ignorePatterns(GetSettings().GetIgnoreFilePattern());
	ignorePatterns.Replace(wxT("\r\n"), wxT(" "));
	ignorePatterns.Replace(wxT("\n"),   wxT(" "));
	ignorePatterns.Replace(wxT("\t"),   wxT(" "));
	ignorePatterns.Replace(wxT("\v"),   wxT(" "));

	wxFFile fp;
	fp.Open(configFile.c_str(), wxT("w+b"));
	if(fp.IsOpened()) {
		fp.Write(wxT("[miscellany]\n"));
		fp.Write(wxT("global-ignores = "));
		fp.Write(ignorePatterns);
		fp.Write(wxT("\n\n[auth]\n"));
		fp.Write(wxT("password-stores ="));
		fp.Write(wxT("\n"));
		fp.Close();
	}
}

void Subversion2::DoGetSvnVersion()
{
	wxString command;
	command << GetSvnExeName(false) << wxT(" --version ");
	m_simpleCommand.Execute(command, wxT(""), new SvnVersionHandler(this, wxNOT_FOUND, NULL));
}

void Subversion2::Patch(bool dryRun, const wxString &workingDirectory, wxEvtHandler *owner, int id)
{
// open a file selector to select the patch file
	const wxString ALL(	wxT("Patch files (*.patch;*.diff)|*.patch;*.diff|")
	                    wxT("All Files (*)|*"));


	wxString patchFile = wxFileSelector(wxT("Select Patch File:"),
										NULL,
										NULL,
										NULL,
										ALL,
										0,
										GetManager()->GetTheApp()->GetTopWindow());
	if (patchFile.IsEmpty() == false) {

		// execute the command
		wxString command;
		command << wxT("patch -p0 ");
		if(dryRun)
			command << wxT(" --dry-run ");
		command << wxT(" -i \"") << patchFile << wxT("\"");

		SvnCommandHandler *handler(NULL);
		if(dryRun) {
			handler = new SvnPatchDryRunHandler(this, id, owner);
		} else {
			handler = new SvnPatchHandler(this, id, owner);
		}
		m_simpleCommand.Execute(command, workingDirectory, handler);
	}
}

void Subversion2::OnLog(wxCommandEvent& event)
{
	SvnLogDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
	dlg.m_to->SetValue(wxT("BASE"));
	dlg.m_compact->SetValue(true);
	dlg.m_from->SetFocus();
	if(dlg.ShowModal() == wxID_OK) {
		wxString command;
		wxString loginString;
		if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
			return;
		}
		
		bool nonInteractive = GetNonInteractiveMode(event);
		command << GetSvnExeName(nonInteractive) << loginString << wxT(" log -r") << dlg.m_from->GetValue() << wxT(":") << dlg.m_to->GetValue() << wxT(" \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
		GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnLogHandler(this, dlg.m_compact->IsChecked(), event.GetId(), this), false);
	}
}

bool Subversion2::GetNonInteractiveMode(wxCommandEvent& event)
{
	return event.GetInt() != INTERACTIVE_MODE;
}

bool Subversion2::LoginIfNeeded(wxCommandEvent& event, const wxString &workingDirectory, wxString& loginString)
{
	UpdateIgnorePatterns();
	
	SvnInfo svnInfo;
	DoGetSvnInfoSync( svnInfo, workingDirectory );
	
	bool loginFailed = (event.GetInt() == LOGIN_REQUIRES);
	
	SubversionPasswordDb db;
	wxString user, password;
	
	if(loginFailed) {
		// if we got here, it means that we already tried to login with either user prompt / using the stored password
		// to prevent an endless loop, remove the old entry from the password db
		db.DeleteLogin(svnInfo.m_url);
	}
	
	if(db.GetLogin(svnInfo.m_url, user, password)) {
		loginString << wxT(" --username ") << user << wxT(" --password ") << password << wxT(" ");
		return true;
	}
	
	// Use the root URL as the key for the login here
	loginString.Empty();
	if(loginFailed) {
		SvnLoginDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
		if (dlg.ShowModal() == wxID_OK) {
			loginString << wxT(" --username ") << dlg.GetUsername() << wxT(" --password ") << dlg.GetPassword() << wxT(" ");
			// Store the user name and password
			db.SetLogin(svnInfo.m_url, dlg.GetUsername(), dlg.GetPassword());
			return true;
		} else {
			return false;
		}
	}
	return true;
}

void Subversion2::IgnoreFiles(const wxArrayString& files, bool pattern)
{
	SvnSettingsData ssd = GetSettings();
	wxArrayString ignorePatternArr = wxStringTokenize(ssd.GetIgnoreFilePattern(), wxT(" \r\n\t\v"), wxTOKEN_STRTOK);

	for(size_t i=0; i<files.GetCount(); i++) {
		wxString entry;
		wxFileName fn(files.Item(i));
		if(pattern) {
			entry << wxT("*.") << fn.GetExt();
		} else {
			entry << fn.GetFullName();
		}

		if(ignorePatternArr.Index(entry) == wxNOT_FOUND) {
			ignorePatternArr.Add(entry);
		}
	}

	wxString ignorePatternStr;
	for(size_t i=0; i<ignorePatternArr.GetCount(); i++){
		ignorePatternStr << ignorePatternArr.Item(i) << wxT(" ");
	}
	ignorePatternStr.RemoveLast();
	ssd.SetIgnoreFilePattern(ignorePatternStr);

	// write down the changes
	SetSettings( ssd );

	// update the config file
	UpdateIgnorePatterns();

	// refresh the view
	GetSvnView()->BuildTree();
}

void Subversion2::OnIgnoreFile(wxCommandEvent& event)
{
	wxArrayString arr;
	arr.Add(DoGetFileExplorerItemFullPath());
	IgnoreFiles(arr, false);
}

void Subversion2::OnIgnoreFilePattern(wxCommandEvent& event)
{
	wxArrayString arr;
	arr.Add(DoGetFileExplorerItemFullPath());
	IgnoreFiles(arr, true);
}

void Subversion2::EditSettings()
{
	SvnPreferencesDialog dlg(GetManager()->GetTheApp()->GetTopWindow(), this);
	if (dlg.ShowModal() == wxID_OK) {
		// Update the Subversion view
		GetSvnView()->BuildTree();
		DoSetSSH();
		UpdateIgnorePatterns();
	}
}

bool Subversion2::IsSubversionViewDetached()
{
	DetachedPanesInfo dpi;
	m_mgr->GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
	wxArrayString detachedPanes = dpi.GetPanes();
	return detachedPanes.Index(svnCONSOLE_TEXT) != wxNOT_FOUND;
}

void Subversion2::OnSelectAsView(wxCommandEvent& event)
{
	wxUnusedVar(event);
	GetSvnView()->BuildTree(DoGetFileExplorerItemPath());
}

void Subversion2::OnBlame(wxCommandEvent& event)
{
	wxArrayString files;
	files.Add(DoGetFileExplorerItemFullPath());
	Blame(event, files);
}

void Subversion2::Blame(wxCommandEvent& event, const wxArrayString& files)
{
	wxString command;
	wxString loginString;
	
	if(files.GetCount() == 0)
		return;
	
	bool nonInteractive = GetNonInteractiveMode(event);
	if(LoginIfNeeded(event, files.Item(0), loginString) == false) {
		return;
	}

	command << GetSvnExeName(nonInteractive) << loginString << wxT(" blame ");

	// Concatenate list of files to be updated
	for (size_t i=0; i<files.GetCount(); i++) {
		command << wxT("\"") << files.Item(i) << wxT("\" ");
	}
	
	GetConsole()->EnsureVisible();
	GetConsole()->AppendText(command + wxT("\n"));
	m_blameCommand.Execute(command, wxT(""), new SvnBlameHandler(this, event.GetId(), this));
}

void Subversion2::OnGetCompileLine(wxCommandEvent& event)
{
	if ( !(GetSettings().GetFlags() & SvnExposeRevisionMacro) )
		return;
	
	wxString macroName ( GetSettings().GetRevisionMacroName() );
	macroName.Trim().Trim(false);
	
	if(macroName.IsEmpty())
		return;
	
	wxString workingDirectory = m_subversionView->GetRootDir();
	workingDirectory.Trim().Trim(false);
	
	SvnInfo svnInfo;
	DoGetSvnInfoSync(svnInfo, workingDirectory);
	
	wxString content = event.GetString();
	content << wxT(" -D");
	content << macroName << wxT("=\\\"");
	content << svnInfo.m_revision << wxT("\\\" ");
	event.SetString( content );
	event.Skip();
}

void Subversion2::DoGetSvnInfoSync(SvnInfo& svnInfo, const wxString &workingDirectory)
{
	wxString svnInfoCommand;
	wxString xmlStr;
	
	svnInfoCommand << GetSvnExeName() << wxT(" info --xml ");
	if(workingDirectory.Find(wxT(" ")))
		svnInfoCommand << wxT("\"") << workingDirectory << wxT("\"");
	else 
		svnInfoCommand << workingDirectory;
	
	wxArrayString xmlArr;
	
	wxLog::EnableLogging(false);
	ProcUtils::ExecuteCommand(svnInfoCommand, xmlArr);
	
	for(size_t i=0; i<xmlArr.GetCount(); i++){
		xmlStr << xmlArr.Item(i);
	}
	
	SvnXML::GetSvnInfo(xmlStr, svnInfo);
	wxLog::EnableLogging(true);
}
