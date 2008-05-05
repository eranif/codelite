#include "app.h"
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

static const wxCmdLineEntryDesc cmdLineDesc[] = {
	{wxCMD_LINE_SWITCH, wxT("v"), wxT("version"), wxT("Print current version"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_OPTION, wxT("b"), wxT("basedir"),  wxT("The base directory of CodeLite"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
	{wxCMD_LINE_NONE }
};

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
{
}

App::~App(void)
{
	wxImage::CleanUpHandlers();
	wxAppBase::ExitMainLoop();
}

bool App::OnInit()
{
#if wxUSE_STACKWALKER
	//trun on fatal exceptions handler
	wxHandleFatalExceptions(true);
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
		ManagerST::Get()->SetInstallDir( wxStandardPaths::Get().GetDataDir() );
		CopySettings(homeDir, wxStandardPaths::Get().GetDataDir());
		
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

	ManagerST::Get()->SetInstallDir( MacGetBasePath() );
	//copy the settings from the global location if needed
	CopySettings(homeDir, MacGetBasePath());

#else //__WXMSW__
	if (homeDir.IsEmpty()) { //did we got a basedir from user?
		// On windows, we use the InstallPath from the registry
		wxRegKey *regKey = new wxRegKey(wxT("HKEY_CURRENT_USER\\Software\\LiteEditor\\LiteEditor"));
		if (regKey->Exists()) {
			regKey->QueryValue(wxT("InstallPath"), homeDir);
			regKey->Close();
		}
		delete regKey;

		if (homeDir.IsEmpty()) {
			homeDir = ::wxGetCwd();
		}
	}
	
	ManagerST::Get()->SetInstallDir( homeDir );
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
	Manager *mgr = ManagerST::Get();

	//show splashscreen here
	long style = wxSIMPLE_BORDER;
#if defined (__WXMSW__) || defined (__WXGTK__)
	style |= wxFRAME_NO_TASKBAR;
#endif

	//read the last frame size from the configuration file
	// Initialise editor configuration files
	EditorConfig *cfg = EditorConfigST::Get();
	if ( !cfg->Load() ) {
		wxLogMessage(wxT("Failed to load configuration file liteeditor.xml"), wxT("CodeLite"), wxICON_ERROR | wxOK);
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
			mainTitle << wxT("CodeLite - SVN build, Revision: ") << SvnRevision;
			m_splash = new SplashScreen(bitmap, mainTitle, wxT("For the Windows(R) & Linux environments"),
			                            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			                            3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
			                            style);
		}
		wxYield();
	}

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

	//if the application started with a given file name,
	//which is not a workspace
	//hide the output &workspace panes
	if (parser.GetParamCount() > 0) {
		ManagerST::Get()->HidePane(wxT("Debugger"));
		ManagerST::Get()->HidePane(wxT("Workspace"));
		ManagerST::Get()->HidePane(wxT("Output"));
	}

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

	return TRUE;
}

int App::OnExit()
{
	return 0;
}

void App::CopySettings(const wxString &destDir, const wxString &installPath)
{
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
		wxCopyFile(installPath + wxT("/config/build_settings.xml"), destDir + wxT("/config/build_settings.xml"));
		wxCopyFile(installPath + wxT("/config/codelite.xml.default"), destDir + wxT("/config/codelite.xml.default"));
		wxCopyFile(installPath + wxT("/config/debuggers.xml"), destDir + wxT("/config/debuggers.xml"));
		wxCopyFile(installPath + wxT("/rc/menu.xrc"), destDir + wxT("/rc/menu.xrc"));
		wxCopyFile(installPath + wxT("/index.html"), destDir + wxT("/index.html"));
		wxCopyFile(installPath + wxT("/svnreport.html"), destDir + wxT("/svnreport.html"));
		wxCopyFile(installPath + wxT("/astyle.sample"), destDir + wxT("/astyle.sample"));
		wxCopyFile(installPath + wxT("/config/accelerators.conf.default"), destDir + wxT("/config/accelerators.conf.default"));
		
	}
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
