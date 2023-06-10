#include "MainFrame.h"

#include <iostream>
#include <thread>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/event.h>
#include <wx/fileconf.h>
#include <wx/image.h>
#include <wx/persist.h>
#include <wx/stdpaths.h>

// #ifdef __WXMSW__
//  void RedirectIOToConsole()
//{
//     AllocConsole();
//     freopen("CON", "wb", stdout);
//     freopen("CON", "wb", stderr);
//     freopen("CON", "r", stdin); // Note: "r", not "w"
// }
// #endif

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

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
        SetAppName("codelite-terminal");
        wxFileName configDir(wxStandardPaths::Get().GetUserDataDir(), "");
        configDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        // m_persistencManager = new clPersistenceManager();
        // wxPersistenceManager::Set(*m_persistencManager);
#ifdef __WXMSW__
        typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
        HINSTANCE user32Dll = LoadLibrary(L"User32.dll");
        if(user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
            if(pFunc) {
                pFunc();
            }
            FreeLibrary(user32Dll);
        }
#endif
        wxCmdLineParser parser(wxApp::argc, wxApp::argv);
        parser.SetDesc(cmdLineDesc);
        const wxArrayString& argv = wxApp::argv.GetArguments();

        for(const wxString& arg : argv) {
            if(arg.StartsWith("--help")) {
                // Print usage and exit
                std::cout << parser.GetUsageString() << std::endl;
                wxExit();
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
