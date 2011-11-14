#include <wx/app.h>
#include <algorithm>
#include <wx/filefn.h>
#include "globals.h"
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
#include <wx/imaglist.h>
#include "svn_patch_dlg.h"
#include <wx/dir.h>

static Subversion2* thePlugin = NULL;

static void replaceAll(wxString& str, const wxString& findWhat, const wxString &replaceWith)
{
	std::string s            = str.To8BitData().data();
	std::string find_what    = findWhat.To8BitData().data();
	std::string replace_with = replaceWith.To8BitData().data();

	size_t where = s.find(find_what);
	while( where != std::string::npos ) {

		s.erase (where, find_what.length());
		s.insert(where, replace_with);

		where = s.find(find_what, where + replace_with.length());
	}

	str.Clear();
	str = wxString::From8BitData(s.c_str());
}

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
	info.SetDescription(_("Subversion plugin for codelite2.0 based on the svn command line tool"));
	info.SetVersion(wxT("v2.0"));
	return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion()
{
	return PLUGIN_INTERFACE_VERSION;
}

Subversion2::Subversion2(IManager *manager)
	: IPlugin           (manager)
	, m_explorerSepItem (NULL)
	, m_simpleCommand   (this)
	, m_diffCommand     (this)
	, m_blameCommand    (this)
	, m_svnClientVersion(0.0)
{
	m_longName = _("Subversion plugin for codelite2.0 based on the svn command line tool");
	m_shortName = wxT("Subversion2");

	DoInitialize();

	GetManager()->GetTheApp()->Connect(XRCID("subversion2_settings"),             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings),          NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_commit"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_update"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_add"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),               NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_delete"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_rename"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRename),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_revert"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),            NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_patch"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnPatch),             NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_diff"),                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDiff),              NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_log"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnLog),               NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_blame"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnBlame),             NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFile),        NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_set_as_view"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSelectAsView),      NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_unlock"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUnLockFile),        NULL, this);
	GetManager()->GetTheApp()->Connect(XRCID("svn_explorer_lock"),                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnLockFile),          NULL, this);

	GetManager()->GetTheApp()->Connect(wxEVT_GET_ADDITIONAL_COMPILEFLAGS, wxCommandEventHandler(Subversion2::OnGetCompileLine),         NULL, this);
	GetManager()->GetTheApp()->Connect(wxEVT_WORKSPACE_CONFIG_CHANGED,    wxCommandEventHandler(Subversion2::OnWorkspaceConfigChanged), NULL, this);
	GetManager()->GetTheApp()->Connect(wxEVT_PROJ_FILE_REMOVED,           wxCommandEventHandler(Subversion2::OnFileRemoved),            NULL, this);
}

Subversion2::~Subversion2()
{
}

clToolBar *Subversion2::CreateToolBar(wxWindow *parent)
{
	wxUnusedVar(parent);
	return NULL;
}

void Subversion2::CreatePluginMenu(wxMenu *pluginsMenu)
{
	wxUnusedVar(pluginsMenu);

	// You can use the below code a snippet:
	wxMenu *menu = new wxMenu();
	wxMenuItem *item(NULL);
	item = new wxMenuItem(menu, XRCID("subversion2_settings"), _("Subversion Options"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	pluginsMenu->Append(wxID_ANY, wxT("Subversion2"), menu);
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

	item = new wxMenuItem(menu, XRCID("svn_explorer_set_as_view"), _("Display this path in the Subversion View"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_update"), _("Update"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_commit"), _("Commit"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_lock"), _("Lock file"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_unlock"), _("UnLock file"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_add"), _("Add"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_delete"), _("Delete"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	item = new wxMenuItem(menu, XRCID("svn_explorer_rename"), _("Rename"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_revert"), _("Revert"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_diff"), _("Create Diff"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	item = new wxMenuItem(menu, XRCID("svn_explorer_patch"), _("Apply Patch"), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);
	menu->AppendSeparator();

	item = new wxMenuItem(menu, XRCID("svn_explorer_blame"), _("Blame ..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	item = new wxMenuItem(menu, XRCID("svn_explorer_log"), _("Change Log..."), wxEmptyString, wxITEM_NORMAL);
	menu->Append(item);

	menu->AppendSeparator();
	wxMenu *subMenu;
	subMenu = new wxMenu;
	subMenu->Append(XRCID("svn_explorer_ignore_file"),         _("Ignore this file"));
	subMenu->Append(XRCID("svn_explorer_ignore_file_pattern"), _("Ignore this file pattern"));
	menu->Append(wxID_ANY, _("Ignore"), subMenu);

	return menu;
}

void Subversion2::UnPlug()
{
	GetManager()->GetTheApp()->Disconnect(XRCID("subversion2_settings"),             wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSettings),          NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_commit"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnCommit),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_update"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnUpdate),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_add"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnAdd),               NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_delete"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDelete),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_rename"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRename),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_revert"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnRevert),            NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_patch"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnPatch),             NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_diff"),                wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnDiff),              NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_log"),                 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnLog),               NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_blame"),               wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnBlame),             NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFile),        NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_ignore_file_pattern"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnIgnoreFilePattern), NULL, this);
	GetManager()->GetTheApp()->Disconnect(XRCID("svn_explorer_set_as_view"),         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Subversion2::OnSelectAsView),      NULL, this);
	GetManager()->GetTheApp()->Disconnect(wxEVT_GET_ADDITIONAL_COMPILEFLAGS, wxCommandEventHandler(Subversion2::OnGetCompileLine), NULL, this);

	m_subversionView->DisconnectEvents();

	// Remove the tab pined to the workspcae pane
	size_t index(Notebook::npos);
	SvnSettingsData ssd = GetSettings();
	index = m_mgr->GetWorkspacePaneNotebook()->GetPageIndex(m_subversionView);
	if (index != Notebook::npos) {
		m_mgr->GetWorkspacePaneNotebook()->RemovePage(index, false);
	}
	ssd.SetSvnTabIndex(index);
	SetSettings(ssd);

	m_subversionView->Destroy();
}

void Subversion2::DoInitialize()
{
	// create tab (possibly detached)
	Notebook *book = m_mgr->GetWorkspacePaneNotebook();
	if( IsSubversionViewDetached() ) {
		// Make the window child of the main panel (which is the grand parent of the notebook)
		DockablePane *cp = new DockablePane(book->GetParent()->GetParent(), book, svnCONSOLE_TEXT, wxNullBitmap, wxSize(200, 200));
		m_subversionView = new SubversionView(cp, this);
		cp->SetChildNoReparent(m_subversionView);

	} else {
		m_subversionView = new SubversionView(book, this);
		size_t index = GetSettings().GetSvnTabIndex();
		if(index == Notebook::npos)
			book->AddPage(m_subversionView, svnCONSOLE_TEXT, false);
		else
			book->InsertPage(index, m_subversionView, svnCONSOLE_TEXT, false);
	}

	DoSetSSH();
	// We need to perform a dummy call to svn so it will create all the default
	// setup directory layout
	wxString      command;
	wxArrayString output;

	command << GetSvnExeName(false) << wxT(" --help ");

	ProcUtils::ExecuteCommand(command, output);
	RecreateLocalSvnConfigFile();
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
	command << GetSvnExeName(false) << loginString << wxT(" add \"") << DoGetFileExplorerItemFullPath() << wxT("\"");
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnStatusHandler(this, event.GetId(), this));
}

void Subversion2::OnCommit(wxCommandEvent& event)
{
	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}

	CommitDialog dlg(GetManager()->GetTheApp()->GetTopWindow(), this);
	if(dlg.ShowModal() == wxID_OK) {
		bool nonInteractive = GetNonInteractiveMode(event);
		wxString comment = dlg.GetMesasge();
		command << GetSvnExeName(nonInteractive) << loginString << wxT(" commit \"") << DoGetFileExplorerItemFullPath() << wxT("\" -m \"") << comment << wxT("\"");
		GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnCommitHandler(this, event.GetId(), this));
	}
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
	diffAgainst = wxGetTextFromUser(_("Insert base revision to diff against:"), _("Diff against"), wxT("BASE"), GetManager()->GetTheApp()->GetTopWindow());
	if (diffAgainst.empty()) {
		// user clicked 'Cancel'
		diffAgainst = wxT("BASE");
	}

	wxString command;
	wxString loginString;
	if(LoginIfNeeded(event, DoGetFileExplorerItemPath(), loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString << wxT("diff -r") << diffAgainst << wxT(" ") << DoGetFileExplorerItemFullPath();
	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnDiffHandler(this, event.GetId(), this), false);
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

//	if(nonInteractive)
//		executeable << wxT(" --non-interactive ");

	// --trust-server-cert was introduced in version >=1.6
	// but it also requires --non-interactive mode enabled
//	if(GetSvnClientVersion() >= 1.6 && nonInteractive) {
//		executeable << wxT(" --trust-server-cert ");
//	}

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

void Subversion2::RecreateLocalSvnConfigFile()
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

	wxString diffTool = GetSettings().GetExternalDiffViewer();
	if(!(GetSettings().GetFlags() & SvnUseExternalDiff)) {
		diffTool.Empty();
	}

	wxFFile fp;
	fp.Open(configFile.c_str(), wxT("w+b"));
	if(fp.IsOpened()) {
		fp.Write(wxT("[miscellany]\n"));
		fp.Write(wxT("global-ignores = "));
		fp.Write(ignorePatterns);
		//fp.Write(wxT("\n\n[auth]\n"));
		//fp.Write(wxT("password-stores =\n"));
		//fp.Write(wxT("store-passwords = no\n"));
		fp.Write(wxT("\n"));
		fp.Write(wxT("[helpers]\n"));

		if(diffTool.IsEmpty() == false) {
			fp.Write(wxT("diff-cmd = "));
			fp.Write(diffTool);
			fp.Write(wxT("\n"));
		}

		fp.Close();
	}
}

void Subversion2::DoGetSvnVersion()
{
	wxString command;
	command << GetSvnExeName(false) << wxT(" --version ");
	m_simpleCommand.Execute(command, wxT(""), new SvnVersionHandler(this, wxNOT_FOUND, NULL), this);
}

void Subversion2::Patch(bool dryRun, const wxString &workingDirectory, wxEvtHandler *owner, int id)
{
	PatchDlg dlg(GetManager()->GetTheApp()->GetTopWindow());
	if (dlg.ShowModal() == wxID_OK) {
		wxBusyCursor cursor;

		wxString patchFile;
		patchFile               = dlg.m_filePicker->GetPath();
		int eolPolicy           = dlg.m_radioBoxEOLPolicy->GetSelection();
		bool removeFileWhenDone = false;

		if(eolPolicy != 0) {
			// Read the file
			wxString fileContent;
			if (ReadFileWithConversion(patchFile, fileContent)) {
				switch(eolPolicy) {
				case 1: // Windows EOL
					replaceAll(fileContent, wxT("\r"), wxT(""));
					replaceAll(fileContent, wxT("\n"), wxT("\r\n"));
					break;
				case 2: // Convert to UNIX style
					replaceAll(fileContent, wxT("\r\n"), wxT("\n"));
					break;
				}

				// Write the content to a new file
				wxFFile fileTemp;
				wxString tmpFile = wxFileName::CreateTempFileName(wxT("clsvn"), &fileTemp);
				if(fileTemp.IsOpened()) {
					if(fileTemp.Write(fileContent)) {
						fileTemp.Close();
						removeFileWhenDone = true;
						patchFile = tmpFile;
					}
				}
			}
		}

		if (patchFile.IsEmpty() == false) {

			// execute the command
			wxString command;
			command << wxT("patch -l -p0 ");
			if(dryRun)
				command << wxT(" --dry-run  ");
			command << wxT(" -i \"") << patchFile << wxT("\"");

			SvnCommandHandler *handler(NULL);
			if(dryRun) {
				handler = new SvnPatchDryRunHandler(this, id, owner, removeFileWhenDone, patchFile);
			} else {
				handler = new SvnPatchHandler(this, id, owner, removeFileWhenDone, patchFile);
			}
			m_simpleCommand.Execute(command, workingDirectory, handler, this);
		}
	}
}

void Subversion2::OnLog(wxCommandEvent& event)
{
	ChangeLog(DoGetFileExplorerItemPath(), DoGetFileExplorerItemFullPath(), event);
}

bool Subversion2::GetNonInteractiveMode(wxCommandEvent& event)
{
	return event.GetInt() != INTERACTIVE_MODE;
}

bool Subversion2::LoginIfNeeded(wxCommandEvent& event, const wxString &workingDirectory, wxString& loginString)
{
	RecreateLocalSvnConfigFile();
//
//	SvnInfo  svnInfo;
//	wxString repoUrl;
//
//	if(event.GetInt() == LOGIN_REQUIRES_URL) {
//		repoUrl = event.GetString();
//
//	} else {
//		DoGetSvnInfoSync( svnInfo, workingDirectory );
//		repoUrl = svnInfo.m_url;
//	}
//
//	bool loginFailed = (event.GetInt() == LOGIN_REQUIRES) || (event.GetInt() == LOGIN_REQUIRES_URL);
//
//	SubversionPasswordDb db;
//	wxString user, password;
//
//	if(loginFailed) {
//		// if we got here, it means that we already tried to login with either user prompt / using the stored password
//		// to prevent an endless loop, remove the old entry from the password db
//		db.DeleteLogin(repoUrl);
//	}
//
//	if(db.GetLogin(repoUrl, user, password)) {
//		loginString << wxT(" --username ") << user << wxT(" --password \"") << password << wxT("\" ");
//		return true;
//	}
//
//	// Use the root URL as the key for the login here
//	loginString.Empty();
//	if(loginFailed) {
//		SvnLoginDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
//		if (dlg.ShowModal() == wxID_OK) {
//			loginString << wxT(" --username ") << dlg.GetUsername() << wxT(" --password \"") << dlg.GetPassword() << wxT("\" ");
//			// Store the user name and password
//			db.SetLogin(repoUrl, dlg.GetUsername(), dlg.GetPassword());
//			return true;
//		} else {
//			return false;
//		}
//	}
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
	for(size_t i=0; i<ignorePatternArr.GetCount(); i++) {
		ignorePatternStr << ignorePatternArr.Item(i) << wxT(" ");
	}
	ignorePatternStr.RemoveLast();
	ssd.SetIgnoreFilePattern(ignorePatternStr);

	// write down the changes
	SetSettings( ssd );

	// update the config file
	RecreateLocalSvnConfigFile();

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
		RecreateLocalSvnConfigFile();
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

	/*bool nonInteractive = unused var commented out*/
	GetNonInteractiveMode(event);
	if(LoginIfNeeded(event, files.Item(0), loginString) == false) {
		return;
	}

	GetConsole()->EnsureVisible();
	command << GetSvnExeName() << wxT(" blame ") << loginString;
	for(size_t i=0 ; i<files.GetCount(); i++) {
		command << wxT("\"") << files.Item(i) << wxT("\" ");
	}

	GetConsole()->AppendText(command + wxT("\n"));
	m_blameCommand.Execute(command, wxT(""), new SvnBlameHandler(this, event.GetId(), this), this);
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

	for(size_t i=0; i<xmlArr.GetCount(); i++) {
		xmlStr << xmlArr.Item(i);
	}

	SvnXML::GetSvnInfo(xmlStr, svnInfo);
	wxLog::EnableLogging(true);
}

bool Subversion2::IsPathUnderSvn(const wxString& path)
{
	wxFileName fn(path);
	wxString svnDirectory1(fn.GetPath());
	wxString svnDirectory2(fn.GetPath());
	svnDirectory1 << wxFileName::GetPathSeparator() << wxT(".svn");
	svnDirectory2 << wxFileName::GetPathSeparator() << wxT("_svn");

	if(wxDirExists(svnDirectory1.c_str()))
		return true;

	if(wxDirExists(svnDirectory2.c_str()))
		return true;

	return false;
}

void Subversion2::OnSwitchURL(wxCommandEvent& event)
{
	SvnInfo svnInfo;
	wxString path = DoGetFileExplorerItemPath();
	DoGetSvnInfoSync(svnInfo, path);
	DoSwitchURL(DoGetFileExplorerItemPath(), svnInfo.m_sourceUrl, event);
}

void Subversion2::DoSwitchURL(const wxString& workingDirectory, const wxString &sourceUrl, wxCommandEvent& event)
{
	SvnInfo svnInfo;
	DoGetSvnInfoSync(svnInfo, workingDirectory);

	wxString loginString;
	if(LoginIfNeeded(event, workingDirectory, loginString) == false) {
		return;
	}
	bool nonInteractive = GetNonInteractiveMode(event);

	wxString targetUrl = wxGetTextFromUser(_("Enter new URL:"), wxT("Svn Switch..."), sourceUrl);
	if(targetUrl.IsEmpty()) {
		return;
	}

	wxString command;

	command << GetSvnExeName(nonInteractive) << wxT(" switch ") << targetUrl << loginString;
	GetConsole()->Execute(command, workingDirectory, new SvnDefaultCommandHandler(this, wxNOT_FOUND, NULL));
}

void Subversion2::ChangeLog(const wxString& path, const wxString &fullpath, wxCommandEvent &event)
{
	SvnInfo info;
	DoGetSvnInfoSync(info, path);

	SvnLogDialog dlg(GetManager()->GetTheApp()->GetTopWindow());
	dlg.m_to->SetValue(wxT("BASE"));
	dlg.m_compact->SetValue(true);
	dlg.m_from->SetFocus();
	if(dlg.ShowModal() == wxID_OK) {
		wxString command;
		wxString loginString;
		if(LoginIfNeeded(event, path, loginString) == false) {
			return;
		}

		bool nonInteractive = GetNonInteractiveMode(event);
		command << GetSvnExeName(nonInteractive) << loginString << wxT(" log -r") << dlg.m_from->GetValue() << wxT(":") << dlg.m_to->GetValue() << wxT(" \"") << fullpath << wxT("\"");
		GetConsole()->Execute(command,
		                      path,
		                      new SvnLogHandler(this, info.m_sourceUrl, dlg.m_compact->IsChecked(), event.GetId(), this),
		                      false);
	}
}

void Subversion2::OnLockFile(wxCommandEvent& event)
{
	wxArrayString paths;
	paths.Add( DoGetFileExplorerItemFullPath() );
	DoLockFile(DoGetFileExplorerItemPath(), paths, event, true);
}

void Subversion2::OnUnLockFile(wxCommandEvent& event)
{
	wxArrayString paths;
	paths.Add( DoGetFileExplorerItemFullPath() );
	DoLockFile(DoGetFileExplorerItemPath(), paths, event, false);
}

void Subversion2::DoLockFile(const wxString& workingDirectory, const wxArrayString &fullpaths, wxCommandEvent& event, bool lock)
{
	wxString command;
	wxString loginString;

	if(fullpaths.empty())
		return;

	if(LoginIfNeeded(event, workingDirectory, loginString) == false) {
		return;
	}

	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString;
	if(lock) {
		command << wxT(" lock ") ;
	} else {
		command << wxT(" unlock ");
	}

	for(size_t i=0; i<fullpaths.size(); i++)
		command << wxT("\"") << fullpaths.Item(i) << wxT("\" ");

	GetConsole()->Execute(command, DoGetFileExplorerItemPath(), new SvnDefaultCommandHandler(this, event.GetId(), this));
}

void Subversion2::OnWorkspaceConfigChanged(wxCommandEvent& event)
{
	event.Skip();
	m_subversionView->BuildTree();
}

void Subversion2::OnFileRemoved(wxCommandEvent& event)
{
	event.Skip();
	wxArrayString *files = (wxArrayString*)event.GetClientData();
	if(files && !files->IsEmpty() && files->GetCount() == 1) {
		wxFileName fn(files->Item(0));
		if(IsPathUnderSvn(fn.GetFullPath())) {
			if(wxMessageBox(wxString::Format(wxT("Would you like to remove '%s' from the svn as well?"), fn.GetFullName().c_str()),
			                wxT("Subversion"),
			                wxYES_NO|wxCANCEL|wxCENTER,
			                GetManager()->GetTheApp()->GetTopWindow()) == wxYES) {
				wxString command;
				RecreateLocalSvnConfigFile();
				command << GetSvnExeName(false) << wxT(" delete --force \"") << fn.GetFullPath() << wxT("\"");
				GetConsole()->Execute(command,
				                      m_subversionView->GetRootDir(),
				                      new SvnDefaultCommandHandler(this, event.GetId(),
				                              this));
			}
		}
	}
}

void Subversion2::OnRename(wxCommandEvent& event)
{
	wxFileName oldname(DoGetFileExplorerItemFullPath());

	wxString newname = wxGetTextFromUser(_("New name:"), _("Svn rename..."), oldname.GetFullName());
	if(newname.IsEmpty() || newname == oldname.GetFullName())
		return;

	DoRename(DoGetFileExplorerItemPath(), oldname.GetFullName(), newname, event);
}

void Subversion2::DoRename(const wxString& workingDirectory, const wxString& oldname, const wxString& newname, wxCommandEvent& event)
{
	wxString command;
	wxString loginString;

	if(LoginIfNeeded(event, workingDirectory, loginString) == false) {
		return;
	}

	if(oldname.IsEmpty() || newname.IsEmpty() || workingDirectory.IsEmpty())
		return;

	bool nonInteractive = GetNonInteractiveMode(event);
	command << GetSvnExeName(nonInteractive) << loginString <<  wxT(" rename --force ") << oldname << wxT(" ") << newname;
	GetConsole()->Execute(command, workingDirectory, new SvnDefaultCommandHandler(this, event.GetId(), this));
}

SvnConsole* Subversion2::GetConsole()
{
	return GetSvnView()->GetSubversionConsole();
}
