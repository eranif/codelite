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
#include "cl_registry.h"
#include "fileextmanager.h"
#include <wx/splash.h>
#include <wx/fileconf.h>
#include "evnvarlist.h"
#include "environmentconfig.h"
#include "conffilelocator.h"
#include "app.h"
#include <wx/snglinst.h>
#include <wx/image.h>
#include <wx/filefn.h>
#include "dirsaver.h"


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
#include <wx/stdpaths.h>
#include "frame.h"
#include "buidltab.h"

#define __PERFORMANCE
#include "performance.h"

//////////////////////////////////////////////
// Define the version string for this codelite
//////////////////////////////////////////////
extern wxChar *SvnRevision;
wxString CODELITE_VERSION_STR = wxString::Format(wxT("v2.5.0.%s"), SvnRevision);

#if defined(__WXMAC__)||defined(__WXGTK__)
#include <signal.h> // sigprocmask
#endif

#ifdef __WXMSW__
#include <wx/msw/registry.h> //registry keys
#endif

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
//-------------------------------------------------
// helper method to draw the revision + version
// on our splash screen
//-------------------------------------------------
static wxBitmap clDrawSplashBitmap(const wxBitmap& bitmap, const wxString &mainTitle)
{
	wxBitmap bmp ( bitmap.GetWidth(), bitmap.GetHeight()  );
	wxMemoryDC dcMem;

	dcMem.SelectObject( bmp );
	dcMem.DrawBitmap  ( bitmap, 0, 0, true);

	//write the main title & subtitle
	wxCoord w, h;
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont smallfont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetPointSize(12);
	smallfont.SetPointSize(10);
	dcMem.SetFont(font);
	dcMem.GetMultiLineTextExtent(mainTitle, &w, &h);
	wxCoord bmpW = bitmap.GetWidth();

	//draw shadow
	dcMem.SetTextForeground(wxT("WHITE"));

	wxCoord textX = (bmpW - w)/2;
	dcMem.DrawText(mainTitle, textX, 11);
	dcMem.SelectObject(wxNullBitmap);
	return bmp;
}

//-----------------------------------------------------
// Splashscreen
//-----------------------------------------------------
class clSplashScreen : public wxSplashScreen
{
public:
	clSplashScreen(const wxBitmap& bmp) : wxSplashScreen(bmp, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT, 5000, NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE| wxFRAME_NO_TASKBAR| wxSTAY_ON_TOP) {
	}
};

#if wxVERSION_NUMBER < 2900
static const wxCmdLineEntryDesc cmdLineDesc[] = {
	{wxCMD_LINE_SWITCH, wxT("v"), wxT("version"), wxT("Print current version"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("Print usage"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_SWITCH, wxT("p"), wxT("no-plugins"), wxT("Start codelite without any plugins"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, wxT("l"), wxT("line"), wxT("Open the file at a given line number"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, wxT("b"), wxT("basedir"),  wxT("Use this path as CodeLite installation path"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_PARAM,  NULL, NULL, wxT("Input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_NONE }
};
#else
static const wxCmdLineEntryDesc cmdLineDesc[] = {
	{wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_SWITCH, "p", "no-plugins", "Start codelite without any plugins", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, "l", "line", "Open the file at a given line number", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, "b", "basedir",  "The base directory of CodeLite", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_PARAM,  NULL, NULL, "Input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
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

extern void InitXmlResource();
App::App(void)
		: m_pMainFrame(NULL)
		, m_singleInstance(NULL)
		, m_loadPlugins(true)
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
//	block signal pipe
	sigset_t mask_set;
	sigemptyset( &mask_set );
	sigaddset(&mask_set, SIGPIPE);
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

	if (parser.Found(wxT("h"))) {
		// print usage
		parser.Usage();
		return false;
	}

	if (parser.Found(wxT("p"))) {
		// Load codelite without plugins
		SetLoadPlugins(false);
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
		wxLogNull noLog;
		wxMkdir(homeDir);
		wxMkdir(homeDir + wxT("/lexers/"));
		wxMkdir(homeDir + wxT("/lexers/Default"));
		wxMkdir(homeDir + wxT("/lexers/BlackTheme"));
		wxMkdir(homeDir + wxT("/rc/"));
		wxMkdir(homeDir + wxT("/images/"));
		wxMkdir(homeDir + wxT("/templates/"));
		wxMkdir(homeDir + wxT("/config/"));

		//copy the settings from the global location if needed
		wxString installPath( INSTALL_DIR, wxConvUTF8 );
		if ( ! CopySettings(homeDir, installPath ) ) return false;
		ManagerST::Get()->SetInstallDir( installPath );

	} else {
		wxFileName fn(homeDir);
		fn.MakeAbsolute();
		ManagerST::Get()->SetInstallDir( fn.GetFullPath() );
	}

#elif defined (__WXMAC__)
	SetAppName(wxT("codelite"));
	homeDir = wxStandardPaths::Get().GetUserDataDir();

	{
		wxLogNull noLog;

		//Create the directory structure
		wxMkdir(homeDir);
		wxMkdir(homeDir + wxT("/lexers/"));
		wxMkdir(homeDir + wxT("/lexers/Default"));
		wxMkdir(homeDir + wxT("/lexers/BlackTheme"));
		wxMkdir(homeDir + wxT("/rc/"));
		wxMkdir(homeDir + wxT("/images/"));
		wxMkdir(homeDir + wxT("/templates/"));
		wxMkdir(homeDir + wxT("/config/"));
	}

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
	if (!fn.FileExists()) {
		// we got wrong home directory
		wxFileName appFn( wxAppBase::argv[0] );
		homeDir = appFn.GetPath();
	}

	if (fnHomdDir.IsRelative()) {
		fnHomdDir.MakeAbsolute();
		homeDir = fnHomdDir.GetPath();
	}

	ManagerST::Get()->SetInstallDir( homeDir );
#endif

	// Update codelite revision and Version
	EditorConfigST::Get()->Init(SvnRevision, wxT("2.0.2") );

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

	// set the performance output file name
	PERF_OUTPUT(wxString::Format(wxT("%s/codelite.perf"), wxGetCwd().c_str()).mb_str(wxConvUTF8));

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

	//---------------------------------------------------------
	// Set environment variable for CodeLiteDir (make it first
	// on the list so it can be used by other variables)
	//---------------------------------------------------------
	EvnVarList vars;
	EnvironmentConfig::Instance()->Load();
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);

	vars.InsertVariable(wxT("Default"), wxT("CodeLiteDir"), ManagerST::Get()->GetInstallDir());
	EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);

	//---------------------------------------------------------

#ifdef __WXMSW__

	// Read registry values
	MSWReadRegistry();

#endif

	GeneralInfo inf;
	cfg->ReadObject(wxT("GeneralInfo"), &inf);

	bool showSplash(false);
#ifndef __WXDEBUG__
	showSplash = inf.GetFlags() & CL_SHOW_SPLASH ? true : false;
#endif

	m_splash = NULL;
	if (showSplash) {
		wxBitmap bitmap;
		wxString splashName(mgr->GetStarupDirectory() + wxT("/images/splashscreen.png"));
		if (bitmap.LoadFile(splashName, wxBITMAP_TYPE_PNG)) {
			wxString mainTitle = CODELITE_VERSION_STR;
			wxBitmap splash = clDrawSplashBitmap(bitmap, mainTitle);
			m_splash = new clSplashScreen(splash);
		}
	}

	// Create the main application window (a dialog in this case)
	// NOTE: Vertical dimension comprises the caption bar.
	//       Horizontal dimension has to take into account the thin
	//       hilighting border around the dialog (2 points in
	//       Win 95).
	Frame::Initialize( parser.GetParamCount() == 0 );
	m_pMainFrame = Frame::Get();

	// update the accelerators table
	ManagerST::Get()->UpdateMenuAccelerators();
	m_pMainFrame->Show(TRUE);
	SetTopWindow(m_pMainFrame);

	long lineNumber(0);
	parser.Found(wxT("l"), &lineNumber);
	if (lineNumber > 0) {
		lineNumber--;
	} else {
		lineNumber = 0;
	}

	for (size_t i=0; i< parser.GetParamCount(); i++) {
		wxString argument = parser.GetParam(i);

		//convert to full path and open it
		wxFileName fn(argument);
		fn.MakeAbsolute(curdir);

		if (fn.GetExt() == wxT("workspace")) {
			ManagerST::Get()->OpenWorkspace(fn.GetFullPath());
		} else {
			Frame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineNumber);
		}
	}

	wxLogMessage(wxString::Format(wxT("Install path: %s"), ManagerST::Get()->GetInstallDir().c_str()));
	wxLogMessage(wxString::Format(wxT("Startup Path: %s"), ManagerST::Get()->GetStarupDirectory().c_str()));

#ifdef __WXGTK__
	// Needed on GTK
	ManagerST::Get()->UpdateMenuAccelerators();
	if (Frame::Get()->GetMainBook()->GetActiveEditor() == NULL) {
		Frame::Get()->GetOutputPane()->GetBuildTab()->SetFocus();
	}
#endif

	return TRUE;
}

int App::OnExit()
{
	EditorConfigST::Free();
	ConfFileLocator::Release();
	return 0;
}

bool App::CopySettings(const wxString &destDir, wxString& installPath)
{
	wxLogNull noLog;

	///////////////////////////////////////////////////////////////////////////////////////////
	// copy new settings from the global installation location which is currently located at
	// /usr/local/share/codelite/ (Linux) or at codelite.app/Contents/SharedSupport
	///////////////////////////////////////////////////////////////////////////////////////////
	CopyDir(installPath + wxT("/templates/"), destDir + wxT("/templates/"));
	CopyDir(installPath + wxT("/lexers/"), destDir + wxT("/lexers/"));
	massCopy  (installPath + wxT("/images/"), wxT("*.png"), destDir + wxT("/images/"));
	wxCopyFile(installPath + wxT("/config/codelite.xml.default"), destDir + wxT("/config/codelite.xml.default"));
	wxCopyFile(installPath + wxT("/rc/menu.xrc"), destDir + wxT("/rc/menu.xrc"));
	wxCopyFile(installPath + wxT("/index.html"), destDir + wxT("/index.html"));
	wxCopyFile(installPath + wxT("/svnreport.html"), destDir + wxT("/svnreport.html"));
	wxCopyFile(installPath + wxT("/astyle.sample"), destDir + wxT("/astyle.sample"));
	wxCopyFile(installPath + wxT("/config/accelerators.conf.default"), destDir + wxT("/config/accelerators.conf.default"));
	return true;
}

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

bool App::CheckSingularity(const wxCmdLineParser &parser, const wxString &curdir)
{
	// check for single instance
	long singleInstance(1);
	EditorConfigST::Get()->GetLongValue(wxT("SingleInstance"), singleInstance);
	if ( singleInstance ) {
		const wxString name = wxString::Format(wxT("CodeLite-%s"), wxGetUserId().c_str());

		m_singleInstance = new wxSingleInstanceChecker(name);
		if (m_singleInstance->IsAnotherRunning()) {
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
		}
	}
	return true;
}

void App::MacOpenFile(const wxString& fileName)
{
	switch (FileExtManager::GetType(fileName)) {
	case FileExtManager::TypeWorkspace:
		ManagerST::Get()->OpenWorkspace(fileName);
		break;
	default:
		Frame::Get()->GetMainBook()->OpenFile(fileName);
		break;
	}
}

void App::MSWReadRegistry()
{
#ifdef __WXMSW__

	EvnVarList vars;
	EnvironmentConfig::Instance()->Load();
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);

	/////////////////////////////////////////////////////////////////
	// New way of registry:
	// Read the registry INI file
	/////////////////////////////////////////////////////////////////

	// Update PATH environment variable with the install directory and
	// MinGW default installation (if exists)
	wxString pathEnv;
	wxGetEnv(wxT("PATH"), &pathEnv);
	pathEnv << wxT(";") << ManagerST::Get()->GetInstallDir() << wxT(";");

	// Load the registry file
	wxString iniFile;
	iniFile << ManagerST::Get()->GetInstallDir() << wxFileName::GetPathSeparator() << wxT("registry.ini");

	if (wxFileName::FileExists(iniFile)) {
		clRegistry::SetFilename(iniFile);
		clRegistry registry;

		m_parserPaths.Clear();
		wxString strWx, strMingw, strUnitTestPP;
		registry.Read(wxT("wx"),         strWx);
		registry.Read(wxT("mingw"),      strMingw);
		registry.Read(wxT("unittestpp"), strUnitTestPP);

		// Supprot for wxWidgets
		if (strWx.IsEmpty() == false) {
			// we have WX installed on this machine, set the path of WXWIN & WXCFG to point to it
			EnvMap envs = vars.GetVariables(wxT("Default"));

			if (!envs.Contains(wxT("WXWIN"))) {
				vars.AddVariable(wxT("Default"), wxT("WXWIN"), strWx);
				vars.AddVariable(wxT("Default"), wxT("PATH"),  wxT("$(WXWIN)\\lib\\gcc_dll;$(PATH)"));
			}

			if (!envs.Contains(wxT("WXCFG")))
				vars.AddVariable(wxT("Default"), wxT("WXCFG"), wxT("gcc_dll\\mswu"));

			EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
			wxSetEnv(wxT("WX_INCL_HOME"), strWx + wxT("\\include"));
		}

		// Support for UnitTest++
		if (strUnitTestPP.IsEmpty() == false) {
			// we have UnitTest++ installed on this machine
			EnvMap envs = vars.GetVariables(wxT("Default"));

			if (!envs.Contains(wxT("UNIT_TEST_PP_SRC_DIR")) ) {
				vars.AddVariable(wxT("Default"), wxT("UNIT_TEST_PP_SRC_DIR"), strUnitTestPP);
			}

			EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
		}

		// Support for MinGW
		if (strMingw.IsEmpty() == false) {
			// Add the installation include paths
			pathEnv << wxT(";") << strMingw << wxT("\\bin");
			wxSetEnv(wxT("MINGW_INCL_HOME"), strMingw);
		}

		wxSetEnv(wxT("PATH"), pathEnv);

	} else {

		// Use the old way: read values from registry. This code is kept so people using
		// SVN build of codelite can continue working with it until an official build is released
		// Update PATH environment variable with the install directory and
		// MinGW default installation (if exists)
		wxString pathEnv;
		if (wxGetEnv(wxT("PATH"), &pathEnv) == false) {
			wxLogMessage(_("WARNING: Failed to load environment variable PATH!"));
		} else {
			pathEnv << wxT(";") << ManagerST::Get()->GetInstallDir() << wxT(";");

			// read the installation path of MinGW & WX
			wxRegKey rk(wxT("HKEY_CURRENT_USER\\Software\\CodeLite"));
			if (rk.Exists()) {
				m_parserPaths.Clear();
				wxString strWx, strMingw, strUnitTestPP;
				if (rk.HasValue(wxT("wx"))) {
					rk.QueryValue(wxT("wx"), strWx);
				}

				if (rk.HasValue(wxT("mingw"))) {
					rk.QueryValue(wxT("mingw"), strMingw);
				}

				if (rk.HasValue(wxT("unittestpp"))) {
					rk.QueryValue(wxT("unittestpp"), strUnitTestPP);
				}



				if (strWx.IsEmpty() == false) {
					// we have WX installed on this machine, set the path of WXWIN & WXCFG to point to it
					EnvMap envs = vars.GetVariables(wxT("Default"));

					if (envs.Contains(wxT("WXWIN")) == false) {
						vars.AddVariable(wxT("Default"), wxT("WXWIN"), strWx);
						vars.AddVariable(wxT("Default"), wxT("PATH"),  wxT("$(WXWIN)\\lib\\gcc_dll;$(PATH)"));
					}

					if (envs.Contains(wxT("WXCFG")) == false)
						vars.AddVariable(wxT("Default"), wxT("WXCFG"), wxT("gcc_dll\\mswu"));

					EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
					wxSetEnv(wxT("WX_INCL_HOME"), strWx + wxT("\\include"));
				}

				if (strMingw.IsEmpty() == false) {
					// Add the installation include paths
					pathEnv << wxT(";") << strMingw << wxT("\\bin");
					wxSetEnv(wxT("MINGW_INCL_HOME"), strMingw);
				}

				// Support for UnitTest++
				if (strUnitTestPP.IsEmpty() == false) {
					// we have UnitTest++ installed on this machine
					EnvMap envs = vars.GetVariables(wxT("Default"));

					if (!envs.Contains(wxT("UNIT_TEST_PP_SRC_DIR")) ) {
						vars.AddVariable(wxT("Default"), wxT("UNIT_TEST_PP_SRC_DIR"), strUnitTestPP);
					}

					EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
				}
			}

			if (wxSetEnv(wxT("PATH"), pathEnv) == false) {
				wxLogMessage(_("WARNING: Failed to update environment variable PATH"));
			}
		}
	}
#endif
}
