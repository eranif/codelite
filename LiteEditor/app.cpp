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

#include "app.h"

#include "ColoursAndFontsManager.h"
#include "CompilerLocatorCygwin.h"
#include "SideBar.hpp"
#include "SocketAPI/clSocketClient.h"
#include "autoversion.h"
#include "clKeyboardManager.h"
#include "clSystemSettings.h"
#include "cl_config.h"
#include "cl_registry.h"
#include "conffilelocator.h"
#include "editor_config.h"
#include "environmentconfig.h"
#include "evnvarlist.h"
#include "file_logger.h"
#include "fileexplorer.h"
#include "fileextmanager.h"
#include "frame.h"
#include "globals.h"
#include "manager.h"
#include "pluginmanager.h"
#include "procutils.h"
#include "singleinstancethreadjob.h"
#include "exelocator.h"
#include "stack_walker.h"

#include <wx/imagjpeg.h>
#include <wx/persist.h>
#include <wx/regex.h>

// #define __PERFORMANCE
#include "performance.h"

//////////////////////////////////////////////
// Define the version string for this codelite
//////////////////////////////////////////////

#if defined(__WXMAC__) || defined(__WXGTK__)
#include <signal.h> // sigprocmask
#include <sys/wait.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/registry.h> //registry keys
void EnableDebugPriv()
{
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

    CloseHandle(hToken);
}
#endif

#ifdef __WXMAC__
#include <mach-o/dyld.h>

// On Mac we determine the base path using system call
// _NSGetExecutablePath(path, &path_len);
wxString MacGetBasePath()
{
    char path[257];
    uint32_t path_len = 256;
    _NSGetExecutablePath(path, &path_len);

    // path now contains
    // CodeLite.app/Contents/MacOS/
    wxFileName fname(wxString(path, wxConvUTF8));

    // remove he MacOS part of the exe path
    wxString file_name = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    wxString rest;
    file_name.EndsWith(wxT("MacOS/"), &rest);
    rest.Append(wxT("SharedSupport/"));

    return rest;
}
#endif

///////////////////////////////////////////////////////////////////

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "n", "no-plugins", "Start CodeLite without any plugins", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "g", "dbg", "Start CodeLite's GDB debugger to debug an executable", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "l", "line", "Open the file at a given line number", wxCMD_LINE_VAL_NUMBER,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "b", "basedir", "Use this path as the CodeLite installation path (Windows only)",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "d", "datadir", "Use this path as the CodeLite data path", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "w", "dbg-dir", "When started with --dbg, set the working directory for the debugger",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "p", "with-plugins", "Comma separated list of plugins to load", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, NULL, NULL, "Input file", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

static void massCopy(const wxString& sourceDir, const wxString& spec, const wxString& destDir)
{
    wxArrayString files;
    wxDir::GetAllFiles(sourceDir, &files, spec, wxDIR_FILES);
    for (size_t i = 0; i < files.GetCount(); i++) {
        wxFileName fn(files.Item(i));
        wxCopyFile(files.Item(i), destDir + wxT("/") + fn.GetFullName());
    }
}
#ifndef __WXMSW__
namespace
{
void ChildTerminatedSingalHandler(int signo)
{
    int status;
    while (true) {
        pid_t pid = ::waitpid(-1, &status, WNOHANG);
        if (pid > 0) {
            // waitpid succeeded
            IProcess::SetProcessExitCode(pid, WEXITSTATUS(status));

        } else {
            break;
        }
    }
}

// Block/Restore sigchild
struct sigaction old_behvior;
struct sigaction new_behvior;
void CodeLiteBlockSigChild()
{
    sigfillset(&new_behvior.sa_mask);
    new_behvior.sa_handler = ChildTerminatedSingalHandler;
    new_behvior.sa_flags = 0;
    sigaction(SIGCHLD, &new_behvior, &old_behvior);
}

void on_sigpipe(int sig)
{
    clERROR() << "Received SIGPIPE!" << endl;
    signal(SIGPIPE, on_sigpipe);
}
} // namespace
#endif

#ifdef __WXGTK__
//-------------------------------------------
// Signal Handlers for GTK
//-------------------------------------------
static void WaitForDebugger(int signo)
{
    wxString msg;
    wxString where;

    msg << wxT("codelite crashed: you may attach to it using gdb\n") << wxT("or let it crash silently..\n")
        << wxT("Attach debugger?\n");

    int rc = wxMessageBox(msg, wxT("CodeLite Crash Handler"), wxYES_NO | wxCENTER | wxICON_ERROR);
    if (rc == wxYES) {

        // Launch a shell command with the following command:
        // gdb -p <PID>

        char command[256];
        memset(command, 0, sizeof(command));

        if (ExeLocator::Locate(wxT("gnome-terminal"), where)) {
            sprintf(command, "gnome-terminal -t 'gdb' -e 'gdb -p %d'", getpid());
        } else if (ExeLocator::Locate(wxT("konsole"), where)) {
            sprintf(command, "konsole -T 'gdb' -e 'gdb -p %d'", getpid());
        } else if (ExeLocator::Locate(wxT("terminal"), where)) {
            sprintf(command, "terminal -T 'gdb' -e 'gdb -p %d'", getpid());
        } else if (ExeLocator::Locate(wxT("lxterminal"), where)) {
            sprintf(command, "lxterminal -T 'gdb' -e 'gdb -p %d'", getpid());
        } else {
            sprintf(command, "xterm -T 'gdb' -e 'gdb -p %d'", getpid());
        }

        if (system(command) == 0) {
            signal(signo, SIG_DFL);
            raise(signo);
        } else {
            // Go down without launching the debugger, ask the user to do it manually
            wxMessageBox(wxString::Format(wxT("Failed to launch the debugger\nYou may still attach to codelite "
                                              "manually by typing this command in a terminal:\ngdb -p %d"),
                                          getpid()),
                         wxT("CodeLite Crash Handler"), wxOK | wxCENTER | wxICON_ERROR);
            pause();
        }
    }

    signal(signo, SIG_DFL);
    raise(signo);
}
#endif

IMPLEMENT_APP(CodeLiteApp)

#ifdef __WXMAC__
#define MENU_XRC "menu.macos.xrc"
#else
#define MENU_XRC "menu.xrc"
#endif

extern void InitXmlResource();
CodeLiteApp::CodeLiteApp(void)
    : m_pMainFrame(NULL)
    , m_singleInstance(NULL)
    , m_pluginLoadPolicy(PP_All)
    , m_persistencManager(NULL)
    , m_startedInDebuggerMode(false)
{
}

CodeLiteApp::~CodeLiteApp(void)
{
    wxImage::CleanUpHandlers();
    if (m_singleInstance) {
        delete m_singleInstance;
    }
    wxDELETE(m_persistencManager);
}

static wxLogNull NO_LOG;

bool CodeLiteApp::OnInit()
{
#if defined(__WXOSX__)
    SetAppName(wxT("CodeLite"));
#else
    SetAppName(wxT("codelite"));
#endif

#ifdef __WXGTK__
    // We need to set the installation prefix on GTK for some reason (mainly debug builds)
    wxString installationDir(INSTALL_DIR);
    installationDir.RemoveLast(sizeof("/share/codelite") - 1);
    wxStandardPaths::Get().SetInstallPrefix(installationDir);
#endif

#if defined(__WXGTK__) || defined(__WXMAC__)
    // block signal pipe
    sigset_t mask_set;
    sigemptyset(&mask_set);
    sigaddset(&mask_set, SIGPIPE);
    sigaddset(&mask_set, SIGTTIN);
    sigprocmask(SIG_SETMASK, &mask_set, nullptr);

    // CodeLite is a MT app, use pthread_sigmask()
    sigset_t thr_mask_set;
    sigemptyset(&thr_mask_set);
    sigaddset(&thr_mask_set, SIGPIPE);
    sigaddset(&thr_mask_set, SIGTTIN);
    pthread_sigmask(SIG_SETMASK, &thr_mask_set, nullptr);

    // Handle sigchld
    CodeLiteBlockSigChild();

#ifdef __WXGTK__
    // Insall signal handlers
    signal(SIGSEGV, WaitForDebugger);
    signal(SIGABRT, WaitForDebugger);
#endif
    signal(SIGPIPE, on_sigpipe);

#endif
    wxSocketBase::Initialize();

#ifdef __WXMAC__
    wxSystemOptions::SetOption("mac.window-plain-transition", "0");
#endif

    // Redirect all error messages to stderr
    wxLog::SetActiveTarget(new wxLogStderr());

#if wxUSE_ON_FATAL_EXCEPTION
    // trun on fatal exceptions handler
    wxHandleFatalExceptions(true);
#endif

#ifdef __WXMSW__
    // HiDPI support
    typedef BOOL(WINAPI * SetProcessDPIAwareFunc)();
    HINSTANCE user32Dll = LoadLibrary(L"User32.dll");
    if (user32Dll) {
        SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
        if (pFunc) {
            pFunc();
        }
        FreeLibrary(user32Dll);
    }
#endif

    wxLog::EnableLogging(false);
    wxString homeDir(wxEmptyString);

    // parse command line
    m_parser.SetDesc(cmdLineDesc);
    m_parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
    if (m_parser.Parse(false) != 0) {
        PrintUsage(m_parser);
        return false;
    }
    wxString newDataDir(wxEmptyString);
    if (m_parser.Found(wxT("d"), &newDataDir)) {
        // ensure that the data dir exists
        wxFileName dd(newDataDir, wxEmptyString);
        if (dd.IsRelative()) {
            dd.MakeAbsolute();
        }
        if (!dd.DirExists()) {
            dd.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        clStandardPaths::Get().SetUserDataDir(dd.GetFullPath());
    }

    // Init resources and add the PNG handler
    wxSystemOptions::SetOption(_T("msw.remap"), 0);
    wxSystemOptions::SetOption("msw.notebook.themed-background", 1);
    wxXmlResource::Get()->InitAllHandlers();

    ::wxInitAllImageHandlers();

#if defined(__WXMSW__) && wxCHECK_VERSION(3, 3, 0)
    if (clConfig::Get().Read("CodeLiteAppearance", 0) == 1) {
        // force dark
        MSWEnableDarkMode(wxApp::DarkMode_Always);
    } else {
        MSWEnableDarkMode(wxApp::DarkMode_Auto);
    }
#endif

#ifndef CL_DEBUG_BUILD
    // dont show the splash in debug builds
    bool show_splash = clConfig::Get().Read("ShowSplashScreen", true);
    if (show_splash) {
        wxBitmap bitmap;
        wxFileName splashscreen_png(clStandardPaths::Get().GetDataDir(), "splashscreen.png");
        splashscreen_png.AppendDir("images");

        if (splashscreen_png.FileExists() && bitmap.LoadFile(splashscreen_png.GetFullPath(), wxBITMAP_TYPE_PNG)) {
            wxSplashScreen* splash =
                new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, nullptr, -1,
                                   wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE | wxSTAY_ON_TOP);
            wxUnusedVar(splash);
        }
        wxYield();
    }
#endif

    InitXmlResource();

    if (m_parser.Found("g", &m_exeToDebug)) {
        SetStartedInDebuggerMode(true);
        // Check to see if the user also passed "--working-directory" switch
        m_parser.Found("w", &m_debuggerWorkingDirectory);
        for (size_t i = 0; i < m_parser.GetParamCount(); ++i) {
            m_debuggerArgs << m_parser.GetParam(i) << " ";
        }
    }

    // check for single instance
    if (!IsSingleInstance(m_parser)) {
        return false;
    }

    if (m_parser.Found(wxT("h"))) {
        // print usage
        PrintUsage(m_parser);
        return false;
    }

    if (m_parser.Found(wxT("v"))) {
// print version
#ifdef __WXMSW__
        ::wxMessageBox(wxString() << "CodeLite IDE v" << CODELITE_VERSION_STRING, "CodeLite");
#else
        wxPrintf("CodeLite IDE v%s\n", CODELITE_VERSION_STRING);
#endif
        return false;
    }

    // When launching CodeLite as a debugger interface, disable the plugins
    if (m_parser.Found(wxT("n")) || IsStartedInDebuggerMode()) {
        // Load codelite without plugins
        SetPluginLoadPolicy(PP_None);
    }

    wxString plugins;
    if (m_parser.Found(wxT("p"), &plugins)) {
        wxArrayString pluginsArr = ::wxStringTokenize(plugins, wxT(","));
        // Trim and make lower case
        for (size_t i = 0; i < pluginsArr.GetCount(); i++) {
            pluginsArr.Item(i).Trim().Trim(false).MakeLower();
        }

        // Load codelite without plugins
        SetAllowedPlugins(pluginsArr);
        SetPluginLoadPolicy(PP_FromList);
    }

#if defined(__WXMSW__)
    wxString newBaseDir;
    if (m_parser.Found(wxT("b"), &newBaseDir)) {
        wxFileName bd(newBaseDir, wxEmptyString);
        if (bd.IsRelative()) {
            bd.MakeAbsolute();
        }
        homeDir = bd.GetPath();
    } else {
        homeDir = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
    }
    clStandardPaths::Get().SetDataDir(homeDir);
#endif

    // Set the global log file verbosity. NB Doing this earlier seems to break wxGTK debug output when debugging
    // CodeLite itself :/
    FileLogger::OpenLog("codelite.log", clConfig::Get().Read(kConfigLogVerbosity, FileLogger::Error));
    clDEBUG() << "Starting codelite..." << endl;

    // Copy gdb pretty printers from the installation folder to a writeable location
    // this is  needed because python complies the files and in most cases the user
    // running codelite has no write permissions to /usr/share/codelite/...
    DoCopyGdbPrinters();

    // Since GCC 4.8.2 gcc has a default colored output
    // which breaks codelite output parsing
    // to disable this, we need to set GCC_COLORS to an empty
    // string.
    // https://sourceforge.net/p/codelite/bugs/946/
    // http://gcc.gnu.org/onlinedocs/gcc/Language-Independent-Options.html
    ::wxSetEnv("GCC_COLORS", "");

#if defined(__WXGTK__)
    if (homeDir.IsEmpty()) {
        homeDir = clStandardPaths::Get()
                      .GetUserDataDir(); // By default, ~/Library/Application Support/codelite or ~/.codelite
        if (!wxFileName::Exists(homeDir)) {
            wxLogNull noLog;
            wxFileName::Mkdir(homeDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            wxCHECK_MSG(wxFileName::DirExists(homeDir), false, "Failed to create the requested data dir");
        }

        // Create the directory structure
        wxLogNull noLog;
        wxMkdir(homeDir);
        wxMkdir(homeDir + wxT("/lexers/"));
        wxMkdir(homeDir + wxT("/rc/"));
        wxMkdir(homeDir + wxT("/images/"));
        wxMkdir(homeDir + wxT("/templates/"));
        wxMkdir(homeDir + wxT("/config/"));
        wxMkdir(homeDir + wxT("/tabgroups/"));

        // copy the settings from the global location if needed
        wxString installPath(INSTALL_DIR, wxConvUTF8);
        if (!CopySettings(homeDir, installPath))
            return false;
        ManagerST::Get()->SetInstallDir(installPath);

    } else {
        wxFileName fn(homeDir);
        fn.MakeAbsolute();
        ManagerST::Get()->SetInstallDir(fn.GetFullPath());
    }

#elif defined(__WXMAC__)
    homeDir = clStandardPaths::Get().GetUserDataDir();
    if (!wxFileName::Exists(homeDir)) {
        wxLogNull noLog;
        wxFileName::Mkdir(homeDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        wxCHECK_MSG(wxFileName::DirExists(homeDir), false, "Failed to create the requested data dir");
    }

    {
        wxLogNull noLog;

        // Create the directory structure
        wxMkdir(homeDir);
        wxMkdir(homeDir + wxT("/lexers/"));
        wxMkdir(homeDir + wxT("/rc/"));
        wxMkdir(homeDir + wxT("/images/"));
        wxMkdir(homeDir + wxT("/templates/"));
        wxMkdir(homeDir + wxT("/config/"));
        wxMkdir(homeDir + wxT("/tabgroups/"));
    }

    wxString installPath(MacGetBasePath());
    ManagerST::Get()->SetInstallDir(installPath);
    // copy the settings from the global location if needed
    CopySettings(homeDir, installPath);

#else //__WXMSW__
    if (homeDir.IsEmpty()) { // did we got a basedir from user?
#ifdef USE_POSIX_LAYOUT
        homeDir = clStandardPaths::Get().GetDataDir();
#else
        homeDir = ::wxGetCwd();
#endif
    }
    wxFileName fnHomdDir(homeDir, "");

    //    // Set the standard path with the new data dir
    //    clStandardPaths::Get().SetDataDir(fnHomdDir.GetPath());

    // try to locate the menu/rc.xrc file
    wxFileName fn(homeDir + wxT("/rc"), MENU_XRC);
    if (!fn.FileExists()) {
        // we got wrong home directory
        wxFileName appFn(wxAppBase::argv[0]);
        homeDir = appFn.GetPath();
    }

    if (fnHomdDir.IsRelative()) {
        fnHomdDir.MakeAbsolute();
        homeDir = fnHomdDir.GetPath();
    }

    ManagerST::Get()->SetInstallDir(homeDir);
#endif

    // Use our persistence manager (which uses wxFileConfig instead of the registry...)
    m_persistencManager = new clPersistenceManager();
    wxPersistenceManager::Set(*m_persistencManager);

    // Make sure we have an instance if the keyboard manager allocated before we create the main frame class
    // (the keyboard manager needs to connect to the main frame events)
    clKeyboardManager::Get();
    PluginManager::Get();
    ManagerST::Get()->SetOriginalCwd(wxGetCwd());
    ::wxSetWorkingDirectory(homeDir);
    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // diffcult to manage, and harder to reuse in later projects.
    // The menubar
    if (!wxXmlResource::Get()->Load(DoFindMenuFile(ManagerST::Get()->GetInstallDir(), wxT("2.0"))))
        return false;

    // keep the startup directory
    ManagerST::Get()->SetStartupDirectory(::wxGetCwd());

    // set the performance output file name
    PERF_OUTPUT(wxString::Format(wxT("%s/codelite.perf"), wxGetCwd().c_str()).mb_str(wxConvUTF8));

    // Initialize the configuration file locater
    ConfFileLocator::Instance()->Initialize(ManagerST::Get()->GetInstallDir(), ManagerST::Get()->GetStartupDirectory());

    // set the CTAGS_REPLACEMENT environment variable
    wxFileName ctagsReplacements(clStandardPaths::Get().GetUserDataDir(), "ctags.replacements");
    wxSetEnv(wxT("CTAGS_REPLACEMENTS"), ctagsReplacements.GetFullPath());

// read the last frame size from the configuration file
// Initialise editor configuration files
#ifdef __WXMSW__
    {
        wxLogNull noLog;
        wxFileName::Mkdir(clStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
#endif

    clSystemSettings::Get(); // Initialise the custom settings _before_ we start constructing the main frame

    Manager* mgr = ManagerST::Get();
    EditorConfig* cfg = EditorConfigST::Get();
    cfg->SetInstallDir(mgr->GetInstallDir());

    // Update codelite revision and Version
    wxString strVersion = CODELITE_VERSION_STRING;
    cfg->Init(strVersion, wxT("2.0.2"));
    if (!cfg->Load()) {
        clERROR() << "Failed to load configuration file: config/codelite.xml. Workding directory:" << wxGetCwd()
                  << endl;
        return false;
    }

#ifdef __WXGTK__
    bool redirect = clConfig::Get().Read(kConfigRedirectLogOutput, true);
    if (redirect) {
        // Redirect stdout/error to a file
        wxFileName stdout_err(clStandardPaths::Get().GetUserDataDir(), "codelite-stdout-stderr.log");
        FILE* new_stdout = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stdout);
        FILE* new_stderr = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stderr);
        wxUnusedVar(new_stderr);
        wxUnusedVar(new_stdout);
    }
#endif

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

    // Set up the locale if appropriate
    if (EditorConfigST::Get()->GetOptions()->GetUseLocale()) {
        int preferredLocale = wxLANGUAGE_ENGLISH;
        // The locale had to be saved as the canonical locale name, as the wxLanguage enum wasn't consistent between wx
        // versions
        wxString preferredLocalename = EditorConfigST::Get()->GetOptions()->GetPreferredLocale();
        if (!preferredLocalename.IsEmpty()) {
            wxString localeToFind = preferredLocalename.BeforeFirst(':');
            if (localeToFind.IsEmpty()) {
                localeToFind << preferredLocalename;
            }
            const wxLanguageInfo* info = wxLocale::FindLanguageInfo(localeToFind);
            if (info) {
                preferredLocale = info->Language;
                if (preferredLocale == wxLANGUAGE_UNKNOWN) {
                    preferredLocale = wxLANGUAGE_ENGLISH;
                }
            }
        }

#if defined(__WXGTK__)
        // Cater for a --prefix= build. This gets added automatically to the search path for catalogues.
        // So hack in the standard ones too, otherwise wxstd.mo will be missed
        wxLocale::AddCatalogLookupPathPrefix(wxT("/usr/share/locale"));
        wxLocale::AddCatalogLookupPathPrefix(wxT("/usr/local/share/locale"));

#elif defined(__WXMSW__)
#ifdef USE_POSIX_LAYOUT
        wxLocale::AddCatalogLookupPathPrefix(clStandardPaths::Get().GetInstallDir() + wxT("\\share\\locale"));
#else
        wxLocale::AddCatalogLookupPathPrefix(ManagerST::Get()->GetInstallDir() + wxT("\\locale"));
#endif
#endif

        // This has to be done before the catalogues are added, as otherwise the wrong one (or none) will be found
        m_locale.Init(preferredLocale);

        bool codelitemo_found = m_locale.AddCatalog(wxT("codelite"));
        if (!codelitemo_found) {
            m_locale.AddCatalog(wxT("CodeLite")); // Hedge bets re our spelling
        }

        if (!codelitemo_found) {
            // I wanted to 'un-init' the locale if no translations were found
            // as otherwise, in a RTL locale, menus, dialogs etc will be displayed RTL, in English...
            // However I couldn't find a way to do this
        }
    } else {
        // For proper encoding handling by system libraries it's needed to inialize locale even if UI translation is
        // turned off
        m_locale.Init(wxLANGUAGE_ENGLISH, wxLOCALE_DONT_LOAD_DEFAULT);
    }

// Append the binary's dir to $PATH. This makes codelite-cc available even for a --prefix= installation
#if defined(__WXMSW__)
    wxChar pathsep(wxT(';'));
#else
    wxChar pathsep(wxT(':'));
#endif
    wxString oldpath;
    wxGetEnv(wxT("PATH"), &oldpath);
    wxFileName execfpath(clStandardPaths::Get().GetExecutablePath());
    wxSetEnv(wxT("PATH"), oldpath + pathsep + execfpath.GetPath());
    wxString newpath;
    wxGetEnv(wxT("PATH"), &newpath);

    // If running under Cygwin terminal, adjust the environment variables
    AdjustPathForCygwinIfNeeded();

    // If running under Cygwin terminal, adjust the environment variables
    AdjustPathForMSYSIfNeeded();

    // Make sure that the colours and fonts manager is instantiated
    ColoursAndFontsManager::Get().Load();

    // Create the main application window
    clMainFrame::Initialize((m_parser.GetParamCount() == 0) && !IsStartedInDebuggerMode());
    m_pMainFrame = clMainFrame::Get();
    m_pMainFrame->Show(TRUE);
    SetTopWindow(m_pMainFrame);

    // Especially with the OutputView open, CodeLite was consuming 50% of a cpu, mostly in updateui
    // The next line limits the frequency of UpdateUI events to every 100ms
#ifdef __WXGTK__
    wxUpdateUIEvent::SetUpdateInterval(500);
#elif defined(__WXMSW__)
    wxUpdateUIEvent::SetUpdateInterval(100);
#endif

    // don't show the tooltip too fast
    wxToolTip::SetDelay(500);
    wxToolTip::SetReshow(500);
    return TRUE;
}

void CodeLiteApp::ProcessCommandLineParams()
{
    long lineNumber(0);
    m_parser.Found(wxT("l"), &lineNumber);
    if (lineNumber > 0) {
        lineNumber--;
    } else {
        lineNumber = 0;
    }

    if (!IsStartedInDebuggerMode()) {
        for (size_t i = 0; i < m_parser.GetParamCount(); i++) {
            OpenItem(m_parser.GetParam(i), lineNumber);
        }
    }
}

int CodeLiteApp::OnExit()
{
    clDEBUG() << "Bye" << endl;
    EditorConfigST::Free();
    ConfFileLocator::Release();

    // flush any saved changes to the configuration file
    clConfig::Get().Save();

    if (IsRestartCodeLite()) {
        // Execute new CodeLite instance
        clSYSTEM() << "Restarting CodeLite:" << GetRestartCommand();
        if (!this->m_restartWD.empty()) {
            ::wxSetWorkingDirectory(this->m_restartWD);
        }
        wxExecute(GetRestartCommand(), wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
    }

    // Delete the temp folder
    wxFileName::Rmdir(clStandardPaths::Get().GetTempDir(), wxPATH_RMDIR_RECURSIVE);
    return 0;
}

bool CodeLiteApp::CopySettings(const wxString& destDir, wxString& installPath)
{
    wxLogNull noLog;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // copy new settings from the global installation location which is currently located at
    // /usr/local/share/codelite/ (Linux) or at codelite.app/Contents/SharedSupport
    ///////////////////////////////////////////////////////////////////////////////////////////
    CopyDir(installPath + wxT("/templates/"), destDir + wxT("/templates/"));
    massCopy(installPath + wxT("/images/"), wxT("*.png"), destDir + wxT("/images/"));
    wxCopyFile(installPath + wxT("/index.html"), destDir + wxT("/index.html"));
    wxCopyFile(installPath + wxT("/svnreport.html"), destDir + wxT("/svnreport.html"));
    wxCopyFile(installPath + wxT("/astyle.sample"), destDir + wxT("/astyle.sample"));
    wxCopyFile(installPath + wxT("/php.sample"), destDir + wxT("/php.sample"));
    return true;
}

void CodeLiteApp::OnFatalException()
{
#if wxUSE_STACKWALKER
    wxString startdir;
    startdir << clStandardPaths::Get().GetUserDataDir() << wxT("/crash.log");

    wxFileOutputStream outfile(startdir);
    wxTextOutputStream out(outfile);
    out.WriteString(wxDateTime::Now().FormatDate() + wxT(" - ") + wxDateTime::Now().FormatTime() + wxT("\n"));
    StackWalker walker(&out);
    walker.Walk();
    wxAppBase::ExitMainLoop();
#endif
}

bool CodeLiteApp::IsSingleInstance(const wxCmdLineParser& m_parser)
{
    // check for single instance
    if (clConfig::Get().Read(kConfigSingleInstance, false)) {
        wxString name = wxString::Format(wxT("CodeLite-%s"), wxGetUserId().c_str());

        wxString path;
#ifndef __WXMSW__
        path = "/tmp";
#endif
        m_singleInstance = new wxSingleInstanceChecker(name, path);
        if (m_singleInstance->IsAnotherRunning()) {
            // prepare commands file for the running instance
            wxArrayString files;
            for (size_t i = 0; i < m_parser.GetParamCount(); i++) {
                wxString argument = m_parser.GetParam(i);

                // convert to full path and open it
                wxFileName fn(argument);
                fn.MakeAbsolute();
                files.Add(fn.GetFullPath());
            }

            try {
                // Send the request
                clSocketClient client;
                bool dummy;
                client.ConnectRemote("127.0.0.1", SINGLE_INSTANCE_PORT, dummy);

                JSON json(cJSON_Object);
                json.toElement().addProperty("args", files);
                client.WriteMessage(json.toElement().format());
                return false;

            } catch (clSocketException& e) {
                clERROR() << "Failed to send single instance request" << e.what() << endl;
            }
        }
    }
    return true;
}

void CodeLiteApp::MacOpenFile(const wxString& fileName)
{
    switch (FileExtManager::GetType(fileName)) {
    case FileExtManager::TypeWorkspace:
        ManagerST::Get()->OpenWorkspace(fileName);
        break;
    default:
        clMainFrame::Get()->GetMainBook()->OpenFile(fileName);
        break;
    }
}

void CodeLiteApp::MSWReadRegistry()
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

    wxString codeliteInstallDir;
    codeliteInstallDir << ManagerST::Get()->GetInstallDir() << wxT(";");
    pathEnv.Prepend(codeliteInstallDir);

    // Load the registry file
    wxString iniFile;
    iniFile << ManagerST::Get()->GetInstallDir() << wxFileName::GetPathSeparator() << wxT("registry.ini");

    if (wxFileName::FileExists(iniFile)) {
        clRegistry::SetFilename(iniFile);
        clRegistry registry;

        m_parserPaths.Clear();
        wxString strWx, strMingw, strUnitTestPP;
        // registry.Read(wxT("wx"),         strWx);
        registry.Read(wxT("mingw"), strMingw);
        registry.Read(wxT("unittestpp"), strUnitTestPP);

        // Supprot for wxWidgets
        if (strWx.IsEmpty() == false) {
            // we have WX installed on this machine, set the path of WXWIN & WXCFG to point to it
            EnvMap envs = vars.GetVariables(wxT("Default"), false, wxEmptyString, wxEmptyString);

            if (!envs.Contains(wxT("WXWIN"))) {
                vars.AddVariable(wxT("Default"), wxT("WXWIN"), strWx);
                vars.AddVariable(wxT("Default"), wxT("PATH"), wxT("$(WXWIN)\\lib\\gcc_dll;$(PATH)"));
            }

            if (!envs.Contains(wxT("WXCFG")))
                vars.AddVariable(wxT("Default"), wxT("WXCFG"), wxT("gcc_dll\\mswu"));

            EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
            wxSetEnv(wxT("WX_INCL_HOME"), strWx + wxT("\\include"));
        }

        // Support for UnitTest++
        if (strUnitTestPP.IsEmpty() == false) {
            // we have UnitTest++ installed on this machine
            EnvMap envs = vars.GetVariables(wxT("Default"), false, wxEmptyString, wxEmptyString);

            if (!envs.Contains(wxT("UNIT_TEST_PP_SRC_DIR"))) {
                vars.AddVariable(wxT("Default"), wxT("UNIT_TEST_PP_SRC_DIR"), strUnitTestPP);
            }

            EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
        }

        // Support for MinGW
        if (strMingw.IsEmpty() == false) {
            // Make sure that codelite's MinGW comes first before any other
            // MinGW installation that might exist on the machine
            strMingw << wxFileName::GetPathSeparator() << wxT("bin;");
            pathEnv.Prepend(strMingw);
        }
        wxSetEnv(wxT("PATH"), pathEnv);
    }
#endif
}

wxString CodeLiteApp::DoFindMenuFile(const wxString& installDirectory, const wxString& requiredVersion)
{
    wxUnusedVar(requiredVersion);
    wxFileName menu_xrc{ installDirectory, MENU_XRC };
    menu_xrc.AppendDir("rc");
    return menu_xrc.GetFullPath();
}

void CodeLiteApp::DoCopyGdbPrinters()
{
    wxFileName printersInstallDir;
#ifdef __WXGTK__
    printersInstallDir = wxFileName(wxString(INSTALL_DIR, wxConvUTF8), "gdb_printers");
#else
    printersInstallDir = wxFileName(clStandardPaths::Get().GetDataDir(), "gdb_printers");
#endif

    // copy the files to ~/.codelite/gdb_printers
    wxLogNull nolog;
    wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "gdb_printers");
    ::wxMkdir(targetDir.GetFullPath());
    ::CopyDir(printersInstallDir.GetFullPath(), targetDir.GetFullPath());
}

void CodeLiteApp::AdjustPathForCygwinIfNeeded()
{
#ifdef __WXMSW__
    clDEBUG() << "AdjustPathForCygwinIfNeeded called" << endl;
    if (!::clIsCygwinEnvironment()) {
        clDEBUG() << "Not running under Cygwin - nothing be done" << endl;
        return;
    }

    clSYSTEM() << "Cygwin environment detected" << endl;

    wxString cygwinRootDir;
    CompilerLocatorCygwin cygwin;
    if (cygwin.Locate()) {
        // this will return the base folder for cygwin (e.g. D:\cygwin)
        cygwinRootDir = (*cygwin.GetCompilers().begin())->GetInstallationPath();
    }

    // Running under Cygwin
    // Adjust the PATH environment variable
    wxString pathEnv;
    ::wxGetEnv("PATH", &pathEnv);

    // Always add the default paths
    wxArrayString paths;
    if (!cygwinRootDir.IsEmpty()) {
        clSYSTEM() << "Cygwin root folder is:" << cygwinRootDir << endl;
        wxFileName cygwinBinFolder(cygwinRootDir, "");
        cygwinBinFolder.AppendDir("bin");
        paths.Add(cygwinBinFolder.GetPath());
    }

    paths.Add("/usr/local/bin");
    paths.Add("/usr/bin");
    paths.Add("/usr/sbin");
    paths.Add("/bin");
    paths.Add("/sbin");

    // Append the paths from the environment variables
    wxArrayString userPaths = ::wxStringTokenize(pathEnv, ";", wxTOKEN_STRTOK);
    paths.insert(paths.end(), userPaths.begin(), userPaths.end());

    wxString fixedPath;
    for (size_t i = 0; i < paths.GetCount(); ++i) {
        wxString& curpath = paths.Item(i);
        static wxRegEx reCygdrive("/cygdrive/([A-Za-z])");
        if (reCygdrive.Matches(curpath)) {
            // Get the drive letter
            wxString volume = reCygdrive.GetMatch(curpath, 1);
            volume << ":";
            reCygdrive.Replace(&curpath, volume);
        }

        fixedPath << curpath << ";";
    }

    clDEBUG() << "Setting PATH environment variable to:" << fixedPath << endl;
    ::wxSetEnv("PATH", fixedPath);
#endif
}

void CodeLiteApp::AdjustPathForMSYSIfNeeded()
{
#ifdef __WXMSW__
    clDEBUG() << "AdjustPathForMSYSIfNeeded called" << endl;
    if (!::clIsMSYSEnvironment()) {
        clDEBUG() << "Not running under MSYS - nothing be done" << endl;
        return;
    }

    clSYSTEM() << "MSYS environment detected" << endl;

    // Running under Cygwin
    // Adjust the PATH environment variable
    wxString pathEnv;
    ::wxGetEnv("PATH", &pathEnv);

    // Always add the default paths
    wxArrayString paths;

    wxString rootDir = "/"; // Default
    // determine the baseroot of the MSYS installation
    wxString msysRoot = ProcUtils::SafeExecuteCommand("sh -c 'cd / && pwd -W'");
    if (!msysRoot.IsEmpty()) {
        clSYSTEM() << "MSYS Root folder is set to:" << msysRoot << endl;
        msysRoot.Trim().Trim(false);
        rootDir.Clear();
        rootDir << msysRoot << "/";
    }

    paths.Add(rootDir + "usr/local/bin");
    paths.Add(rootDir + "usr/bin");
    paths.Add(rootDir + "usr/sbin");
    paths.Add(rootDir + "bin");
    paths.Add(rootDir + "sbin");

    // Append the paths from the environment variables
    wxArrayString userPaths = ::wxStringTokenize(pathEnv, ";", wxTOKEN_STRTOK);
    paths.insert(paths.end(), userPaths.begin(), userPaths.end());

    wxString fixedPath = ::wxJoin(paths, ';');
    clDEBUG() << "Setting PATH environment variable to:" << fixedPath << endl;
    ::wxSetEnv("PATH", fixedPath);
#endif
}

void CodeLiteApp::PrintUsage(const wxCmdLineParser& m_parser)
{
#ifdef __WXMSW__
    m_parser.Usage();
#else
    wxString usageString = m_parser.GetUsageString();
    std::cout << usageString.mb_str(wxConvUTF8).data() << std::endl;
#endif
}

void CodeLiteApp::OpenFolder(const wxString& path)
{
    wxArrayString files;
    if (wxDir::GetAllFiles(path, &files, "*.workspace", wxDIR_FILES) == 1) {
        ManagerST::Get()->OpenWorkspace(files.Item(0));
    } else {
        clMainFrame::Get()->GetFileExplorer()->OpenFolder(path);
        clMainFrame::Get()->GetWorkspacePane()->SelectTab(_("Explorer"));
    }
}

void CodeLiteApp::OpenFile(const wxString& path, long lineNumber)
{
    wxFileName fn(path);
    if (fn.GetExt() == wxT("workspace")) {
        ManagerST::Get()->OpenWorkspace(fn.GetFullPath());
    } else {
        clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineNumber);
    }
}

void CodeLiteApp::OpenItem(const wxString& path, long lineNumber)
{
    // convert to full path and open it
    if (path == ".") {
        // Open the current folder
        OpenFolder(ManagerST::Get()->GetOriginalCwd());
    } else {
        wxFileName fn(path);
        fn.MakeAbsolute(ManagerST::Get()->GetOriginalCwd());
        if (wxFileName::DirExists(fn.GetFullPath())) {
            OpenFolder(fn.GetFullPath());

        } else {
            OpenFile(fn.GetFullPath(), lineNumber);
        }
    }
}
