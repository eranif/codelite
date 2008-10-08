////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : app.cpp
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

#include <wx/socket.h>
#include "performance.h"
#include "conffilelocator.h"
#include "app.h"
#include <wx/snglinst.h>
#include <wx/image.h>

#include "xmlutils.h"
#include "editor_config.h"
#include <wx/xrc/xmlres.h>
#include <wx/sysopt.h>
#include "manager.h"
#include "macros.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "stack_walker.h"
#include <wx/cmdline.h>
#include "procutils.h"
#include "globals.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "splashscreen.h"
#include <wx/stdpaths.h>

#if defined(__WXMAC__)||defined(__WXGTK__)
#include <signal.h> // sigprocmask
#endif

#ifdef __WXMSW__
#include <wx/msw/registry.h> //registry keys
#endif

extern wxChar *SvnRevision;

#ifdef __WXMAC__
#include <mach-o/dyld.h>

//On Mac we determine the base path using system call
//_NSGetExecutablePath(path, &path_len);
wxString MacGetBasePath()
{
	char path[257];
	uint32_t path_len = 256;
	_NSGetExecutablePath(path, &path_len);

	//path now contains
	//CodeLite.app/Contents/MacOS/
	wxFileName fname(wxString(path, wxConvUTF8));

	//remove he MacOS part of the exe path
	wxString file_name = fname.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	wxString rest;
	file_name.EndsWith(wxT("MacOS/"), &rest);
	rest.Append(wxT("SharedSupport/"));

	return rest;
}
#endif

#if wxVERSION_NUMBER < 2900
static const wxCmdLineEntryDesc cmdLineDesc[] = {
	{wxCMD_LINE_SWITCH, wxT("v"), wxT("version"), wxT("Print current version"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, wxT("b"), wxT("basedir"),  wxT("The base directory of CodeLite"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_NONE }
};
#else
static const wxCmdLineEntryDesc cmdLineDesc[] = {
	{wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, "b", "basedir",  "The base directory of CodeLite", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_NONE }
};
#endif

static void massCopy(const wxString &sourceDir, const wxString &spec, const wxString &destDir)
{
	wxArrayString files;
	wxDir::GetAllFiles(sourceDir, &files, spec, wxDIR_FILES);
	for ( size_t i=0; i<files.GetCount(); i++ ) {
		wxFileName fn(files.Item(i));
		wxCopyFile(files.Item(i), destDir + wxT("/") + fn.GetFullName());
	}
}

IMPLEMENT_APP(App)
BEGIN_EVENT_TABLE(App, wxApp)
	//EVT_COMMAND(wxID_ANY, HIDE_SPLASH_EVENT_ID, App::OnHideSplash)
END_EVENT_TABLE()

extern void InitXmlResource();
App::App(void)
		: m_pMainFrame(NULL)
		, m_singleInstance(NULL)
#ifdef __WXMSW__
		, m_handler(NULL)
#endif
{
}

App::~App(void)
{
	wxImage::CleanUpHandlers();
#ifdef __WXMSW__
	if (m_handler) {
		FreeLibrary(m_handler);
		m_handler = NULL;
	}
#endif
	if ( m_singleInstance ) {
		delete m_singleInstance;
	}
	wxAppBase::ExitMainLoop();
}

bool App::OnInit()
{
#if defined(__WXGTK__) || defined(__WXMAC__)
	// install signal handlers
	sigset_t mask_set;
	sigfillset( &mask_set );
	sigprocmask(SIG_SETMASK, &mask_set, NULL);
#endif
	
	wxSocketBase::Initialize();
	
#if wxUSE_STACKWALKER
	//trun on fatal exceptions handler
	wxHandleFatalExceptions(true);
#endif

#ifdef __WXMSW__
	// as described in http://jrfonseca.dyndns.org/projects/gnu-win32/software/drmingw/
	// load the exception handler dll so we will get Dr MinGW at runtime
	m_handler = LoadLibrary(wxT("exchndl.dll"));
#endif

	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(_T("msw.remap"), 0);
	wxXmlResource::Get()->InitAllHandlers();
	wxImage::AddHandler( new wxPNGHandler );
	wxImage::AddHandler( new wxCURHandler );
	wxImage::AddHandler( new wxICOHandler );
	wxImage::AddHandler( new wxXPMHandler );
	wxImage::AddHandler( new wxGIFHandler );
	InitXmlResource();
	//wxLog::EnableLogging(false);
	wxString homeDir(wxEmptyString);

	//parse command line
	wxCmdLineParser parser;
	parser.SetDesc(cmdLineDesc);
	parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
	if (parser.Parse() != 0) {
		return false;
	}

	wxString newBaseDir(wxEmptyString);
	if (parser.Found(wxT("b"), &newBaseDir)) {
		homeDir = newBaseDir;
	}

#if defined (__WXGTK__)
	if (homeDir.IsEmpty()) {
		SetAppName(wxT("codelite"));
		homeDir = wxStandardPaths::Get().GetUserDataDir(); // ~/Library/Application Support/codelite or ~/.codelite

		//Create the directory structure
		wxMkDir(homeDir.ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/lexers/")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/lexers/Default")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/lexers/BlackTheme")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/rc/")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/images/")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/templates/")).ToAscii(), 0777);
		wxMkDir((homeDir + wxT("/config/")).ToAscii(), 0777);

		//copy the settings from the global location if needed
		wxString installPath( wxStandardPaths::Get().GetDataDir() );
		if ( ! CopySettings(homeDir, installPath ) ) return false;
		ManagerST::Get()->SetInstallDir( installPath );

	} else {
		wxFileName fn(homeDir);
		fn.MakeAbsolute();
		ManagerST::Get()->SetInstallDir( fn.GetFullPath() );
	}

#elif defined (__WXMAC__)
	SetAppName(wxT("codelite"));
	homeDir = wxGetHomeDir() + wxT("/.codelite/");
	//Create the directory structure
	wxMkDir(homeDir.ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/lexers/")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/lexers/Default")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/lexers/BlackTheme")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/rc/")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/images/")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/templates/")).ToAscii(), 0777);
	wxMkDir((homeDir + wxT("/config/")).ToAscii(), 0777);

	wxString installPath( MacGetBasePath() );
	ManagerST::Get()->SetInstallDir( installPath );
	//copy the settings from the global location if needed
	CopySettings(homeDir, installPath);

#else //__WXMSW__
	if (homeDir.IsEmpty()) { //did we got a basedir from user?
		homeDir = ::wxGetCwd();
	}
	wxFileName fnHomdDir(homeDir + wxT("/"));
	
	// try to locate the menu/rc.xrc file
	wxFileName fn(homeDir + wxT("/rc"), wxT("menu.xrc"));
	if(!fn.FileExists()){
		// we got wrong home directory
		wxFileName appFn( wxAppBase::argv[0] );
		homeDir = appFn.GetPath();
	}
	
	if(fnHomdDir.IsRelative()){
		fnHomdDir.MakeAbsolute();
		homeDir = fnHomdDir.GetPath();
	}
	
	// updatre PATH environment variable with the install directory
	wxString pathEnv;
	wxGetEnv(wxT("PATH"), &pathEnv);
	pathEnv << wxT(";") << homeDir << wxT(";");
	wxSetEnv(wxT("PATH"), pathEnv);
	
	ManagerST::Get()->SetInstallDir( homeDir );
	EditorConfig::Init( SvnRevision );
#endif

	wxString curdir = wxGetCwd();
	::wxSetWorkingDirectory(homeDir);
	// Load all of the XRC files that will be used. You can put everything
	// into one giant XRC file if you wanted, but then they become more
	// diffcult to manage, and harder to reuse in later projects.
	// The menubar
	if (!wxXmlResource::Get()->Load(wxT("rc/menu.xrc")))
		return false;

	// keep the startup directory
	ManagerST::Get()->SetStarupDirectory(::wxGetCwd());
	
	// Initialize the configuration file locater
	ConfFileLocator::Instance()->Initialize(ManagerST::Get()->GetInstallDir(), ManagerST::Get()->GetStarupDirectory());
	
	Manager *mgr = ManagerST::Get();
	
	// set the CTAGS_REPLACEMENT environment variable
	wxSetEnv(wxT("CTAGS_REPLACEMENTS"), ManagerST::Get()->GetStarupDirectory() + wxT("/ctags.replacements"));
	
	//show splashscreen here
	long style = wxSIMPLE_BORDER;
#if defined (__WXMSW__) || defined (__WXGTK__)
	style |= wxFRAME_NO_TASKBAR;
#endif

	//read the last frame size from the configuration file
	// Initialise editor configuration files
	EditorConfig *cfg = EditorConfigST::Get();
	if ( !cfg->Load() ) {
		wxLogMessage(wxT("Failed to load configuration file: ") + wxGetCwd() + wxT("/config/codelite.xml"), wxT("CodeLite"), wxICON_ERROR | wxOK);
		return false;
	}

	// check for single instance
	if ( !CheckSingularity(parser, curdir) ) {
		return false;
	}

	GeneralInfo inf;
	cfg->ReadObject(wxT("GeneralInfo"), &inf);

	bool showSplash = inf.GetFlags() & CL_SHOW_SPLASH ? true : false;
	if (showSplash) {
		wxBitmap bitmap;
		wxString splashName(mgr->GetStarupDirectory() + wxT("/images/splashscreen.png")); 
		if (bitmap.LoadFile(splashName, wxBITMAP_TYPE_PNG)) {
			wxString mainTitle;
			mainTitle << wxT("v1.0.") << SvnRevision;
			m_splash = new SplashScreen(bitmap, mainTitle, wxEmptyString,
			                            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			                            3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
			                            style);
		}
		wxYield();
	}
	
	// redirect stderr
	PERF_INIT();

	// Create the main application window (a dialog in this case)
	// NOTE: Vertical dimension comprises the caption bar.
	//       Horizontal dimension has to take into account the thin
	//       hilighting border around the dialog (2 points in
	//       Win 95).
	Frame::Initialize( parser.GetParamCount() == 0 );
	m_pMainFrame = Frame::Get();

	// Center the dialog when first shown
//	m_pMainFrame->Centre();

	// update the accelerators table
	ManagerST::Get()->UpdateMenuAccelerators();

//	//if the application started with a given file name,
//	//which is not a workspace
//	//hide the output &workspace panes
//	if (parser.GetParamCount() > 0) {
//		ManagerST::Get()->HidePane(wxT("Debugger"));
//		ManagerST::Get()->HidePane(wxT("Workspace View"));
//		ManagerST::Get()->HidePane(wxT("Output View"));
//	}

	m_pMainFrame->Show(TRUE);
	SetTopWindow(m_pMainFrame);

	if ( showSplash ) {
		m_splash->Close(true);
		m_splash->Destroy();
	}

	for (size_t i=0; i< parser.GetParamCount(); i++) {
		wxString argument = parser.GetParam(i);

		//convert to full path and open it
		wxFileName fn(argument);
		fn.MakeAbsolute(curdir);

		if (fn.GetExt() == wxT("workspace")) {
			ManagerST::Get()->OpenWorkspace(fn.GetFullPath());
		} else {
			ManagerST::Get()->OpenFile(fn.GetFullPath(), wxEmptyString);
		}
	}
	
	wxLogMessage(wxString::Format(wxT("Install path: %s"), ManagerST::Get()->GetInstallDir().c_str()));
	wxLogMessage(wxString::Format(wxT("Startup Path: %s"), ManagerST::Get()->GetStarupDirectory().c_str()));
	return TRUE;
}

int App::OnExit()
{
	ConfFileLocator::Release();
	return 0;
}

bool App::CopySettings(const wxString &destDir, wxString& installPath)
{
#if defined (__WXGTK__)
	// By default everything's been installed to /usr/local/*
	// However check, as --prefix= might have put things elsewhere
	if ( ! LocateConfPath( installPath ) ) {
		wxLogMessage( wxT("Help, I couldn't find CodeLite's resource files!\nPlease check your installation") ); 
		return false;
	}
#endif

	bool fileExist = wxFileName::FileExists( destDir + wxT("/config/codelite.xml") );
	bool copyAnyways(true);

	if (fileExist) {
		if (CheckRevision(destDir + wxT("/config/codelite.xml")) == true) {
			//revision is ok
			copyAnyways = false;
		}
	}

	if ( !fileExist || copyAnyways ) {
		///////////////////////////////////////////////////////////////////////////////////////////
		// copy new settings from the global installation location which is currently located at
		// /usr/local/share/codelite/ (Linux) or at codelite.app/Contents/SharedSupport
		///////////////////////////////////////////////////////////////////////////////////////////
		CopyDir(installPath + wxT("/templates/"), destDir + wxT("/templates/"));
		CopyDir(installPath + wxT("/lexers/"), destDir + wxT("/lexers/"));
		massCopy  (installPath + wxT("/images/"), wxT("*.png"), destDir + wxT("/images/"));
		massCopy  (installPath + wxT("/"), wxT("*.tags"), destDir + wxT("/"));
		wxCopyFile(installPath + wxT("/config/codelite.xml.default"), destDir + wxT("/config/codelite.xml.default"));
		wxCopyFile(installPath + wxT("/rc/menu.xrc"), destDir + wxT("/rc/menu.xrc"));
		wxCopyFile(installPath + wxT("/index.html"), destDir + wxT("/index.html"));
		wxCopyFile(installPath + wxT("/svnreport.html"), destDir + wxT("/svnreport.html"));
		wxCopyFile(installPath + wxT("/astyle.sample"), destDir + wxT("/astyle.sample"));
		wxCopyFile(installPath + wxT("/config/accelerators.conf.default"), destDir + wxT("/config/accelerators.conf.default"));
	}
	return true;
}

#if defined (__WXGTK__)
bool App::LocateConfPath( wxString& installPath )
{
  // By default, installPath will be /usr/local/share/codelite/
  if ( wxFileName::DirExists( installPath ) ) return true;
  installPath = wxT("/usr/share/codelite");  // Next likeliest: this is where distros will put it
  if ( wxFileName::DirExists( installPath ) ) return true;
  installPath = wxT("/etc/codelite");        // OK, try the officially-correct place (though it should really be a foo.conf file)
  if ( wxFileName::DirExists( installPath ) ) return true;
  if ( wxFileName::FileExists( wxT("./config/accelerators.conf.default") ) )  // Last chance. If we're running from an "uninstalled" CL binary...
    { installPath = wxGetCwd();	return true; }
  return false;
}
#endif

void App::OnFatalException()
{
#if wxUSE_STACKWALKER
	Manager *mgr = ManagerST::Get();
	wxString startdir = mgr->GetStarupDirectory();
	startdir << wxT("/crash.log");

	wxFileOutputStream outfile(startdir);
	wxTextOutputStream out(outfile);
	out.WriteString(wxDateTime::Now().FormatTime() + wxT("\n"));
	StackWalker walker(&out);
	walker.Walk();
	wxAppBase::ExitMainLoop();
#endif
}

void App::OnIdle(wxIdleEvent &e)
{
	//delegate the event to the manager class
	Frame::Get()->ProcessEvent(e);
	e.Skip();
}

bool App::ReadControlFile(wxString &installPath, long &installRev)
{
	wxString content;
	if (!ReadFileWithConversion(wxT("/usr/local/share/codelite/codelite.control"), content)) {
		return false;
	}

	wxStringTokenizer tkz(content, wxT("\n"), wxTOKEN_STRTOK);
	wxString revName = tkz.NextToken();
	wxString tmprev;
	if (revName.StartsWith(wxT("Revision:"), &tmprev)) {
		revName = tmprev;
	}
	revName.ToLong( &installRev );
	installPath = tkz.NextToken();
	return true;
}

void App::OnHideSplash(wxCommandEvent &e)
{
	m_splash->Close(true);
	m_splash->Destroy();
	e.Skip();
}

bool App::CheckRevision(const wxString &fileName)
{
	wxXmlDocument doc;
	doc.Load(fileName);
	if (doc.IsOk()) {
		wxXmlNode *root = doc.GetRoot();
		if (root) {
			wxString configRevision = XmlUtils::ReadString(root, wxT("Revision"));
			wxString curRevision(SvnRevision);
			if (configRevision.Trim().Trim(false) == curRevision.Trim().Trim(false)) {
				return true;
			}
		}
	}
	return false;
}

bool App::CheckSingularity(const wxCmdLineParser &parser, const wxString &curdir)
{
	// check for single instance
	long singleInstance(1);
	EditorConfigST::Get()->GetLongValue(wxT("SingleInstance"), singleInstance);
	if ( singleInstance ) {
		const wxString name = wxString::Format(wxT("CodeLite-%s"), wxGetUserId().c_str());

		m_singleInstance = new wxSingleInstanceChecker(name);
		if (m_singleInstance->IsAnotherRunning()) {
//			wxMessageBox(wxT("Another is running!"));
//			wxMessageBox(wxT("CodeLite is configured to allow only single instance running.\nThis can be modified from the 'Settings -> Editor' menu"), wxT("CodeLite"), wxOK | wxICON_ERROR);
			// prepare commands file for the running instance
			wxString files;
			for (size_t i=0; i< parser.GetParamCount(); i++) {
				wxString argument = parser.GetParam(i);

				//convert to full path and open it
				wxFileName fn(argument);
				fn.MakeAbsolute(curdir);
				files << fn.GetFullPath() << wxT("\n");
			}

			if (files.IsEmpty() == false) {
				Mkdir(ManagerST::Get()->GetStarupDirectory() + wxT("/ipc"));
				
				wxString file_name, tmp_file;
				tmp_file 	<< ManagerST::Get()->GetStarupDirectory()
							<< wxT("/ipc/command.msg.tmp");
							
				file_name 	<< ManagerST::Get()->GetStarupDirectory()
							<< wxT("/ipc/command.msg");
				
				// write the content to a temporary file, once completed, 
				// rename the file to the actual file name
				WriteFileUTF8(tmp_file, files);
				wxRenameFile(tmp_file, file_name);
			}
			return false;
		} else {
//			wxMessageBox(wxT("No other instances!"));
		}
	}
	return true;
}
