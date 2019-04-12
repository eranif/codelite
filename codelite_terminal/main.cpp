#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>
#include <iostream>
#include <thread>
#include <wx/cmdline.h>
#include "wxTerminalOptions.h"
#include <wx/persist.h>
#include "clPersistenceManager.h"

//#ifdef __WXMSW__
// void RedirectIOToConsole()
//{
//    AllocConsole();
//    freopen("CON", "wb", stdout);
//    freopen("CON", "wb", stderr);
//    freopen("CON", "r", stdin); // Note: "r", not "w"
//}
//#endif

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    // Switches
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "w", "wait", "Wait for any key to be pressed before exiting", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    // Options
    { wxCMD_LINE_OPTION, "p", "print-tty", "Print the terminals tty (*nix only) into file", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "t", "title", "Set the console title", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "d", "working-directory", "Set the working directory", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "c", "command", "Command to execute", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "f", "file", "File contains command to execute", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

class MyPersistenceManager : public wxPersistenceManager
{
    wxFileConfig* m_config = nullptr;

public:
    MyPersistenceManager()
    {
        wxFileName localFile(wxStandardPaths::Get().GetUserDataDir(), "persistency.ini");
        m_config = new wxFileConfig("codelite-terminal", "CodeLite", localFile.GetFullPath());
    }

    ~MyPersistenceManager() { wxDELETE(m_config); }
    wxConfigBase* GetConfig() const { return m_config; }
};

// Define the MainApp
class MainApp : public wxApp
{
    MyPersistenceManager* m_persistency = nullptr;

public:
    MainApp() {}
    virtual ~MainApp() { wxDELETE(m_persistency); }

    virtual bool OnInit()
    {
        SetAppName("codelite-terminal");
        wxFileName configDir(wxStandardPaths::Get().GetUserDataDir(), "");
        configDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        m_persistency = new MyPersistenceManager();
        wxPersistenceManager::Set(*m_persistency);
        wxTerminalOptions& m_options = wxTerminalOptions::Get();

        // m_persistencManager = new clPersistenceManager();
        // wxPersistenceManager::Set(*m_persistencManager);
#ifdef __WXMSW__
        typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
        HINSTANCE user32Dll = LoadLibrary(L"User32.dll");
        if(user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
            if(pFunc) { pFunc(); }
            FreeLibrary(user32Dll);
        }
#endif
        wxCmdLineParser parser(wxApp::argc, wxApp::argv);
        parser.SetDesc(cmdLineDesc);
        const wxArrayString& argv = wxApp::argv.GetArguments();

        for(const wxString& arg : argv) {
            if(arg.StartsWith("--wait")) {
                m_options.SetWaitOnExit(true);
            } else if(arg.StartsWith("--help")) {
                // Print usage and exit
                std::cout << parser.GetUsageString() << std::endl;
                wxExit();
            } else if(arg.StartsWith("--title")) {
                wxString title = arg.AfterFirst('=');
                m_options.SetTitle(title);
            } else if(arg.StartsWith("--print-tty")) {
                m_options.SetPrintTTY(true);
                wxString ttyfile = arg.AfterFirst('=');
                m_options.SetTtyfile(ttyfile);
            } else if(arg.StartsWith("--working-directory")) {
                wxString wd = arg.AfterFirst('=');
                m_options.SetWorkingDirectory(wd);
            } else if(arg.StartsWith("--command")) {
                wxString cmd = arg.AfterFirst('=');
                m_options.SetCommand(cmd);
            } else if(arg.StartsWith("--file")) {
                wxString cmdfile = arg.AfterFirst('=');
                m_options.SetCommandFromFile(cmdfile);
            } else if(arg.StartsWith("--log")) {
                wxString logfile = arg.AfterFirst('=');
                m_options.SetLogfile(logfile);
            }
        }

        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        MainFrame* mainFrame = new MainFrame(NULL);
        SetTopWindow(mainFrame);
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)

// int main(int argc, char** argv)
//{
//    // MyWxApp derives from wxApp
//    wxApp::SetInstance(new MainApp());
//    wxEntryStart(argc, argv);
//    wxTheApp->OnInit();
//    wxTheApp->OnRun();
//    wxTheApp->OnExit();
//    wxEntryCleanup();
//    return 0;
//}
