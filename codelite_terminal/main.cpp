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
    { wxCMD_LINE_SWITCH, "p", "print-tty", "Print the terminals tty (*nix only)", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    // Options
    { wxCMD_LINE_OPTION, "t", "title", "Set the console title", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "d", "working-directory", "Set the working directory", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "c", "command", "Command to execute", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

// Define the MainApp
class MainApp : public wxApp
{
    wxTerminalOptions m_options;
    clPersistenceManager* m_persistencManager = nullptr;

public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
        SetAppName("codelite-terminal");

        m_persistencManager = new clPersistenceManager();
        wxPersistenceManager::Set(*m_persistencManager);
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
        if(parser.Parse() != 0) { return false; }

        if(parser.Found("w")) { m_options.SetWaitOnExit(true); }
        if(parser.Found("p")) { m_options.SetPrintTTY(true); }

        wxString workingDirectory;
        if(parser.Found("d", &workingDirectory)) { m_options.SetWorkingDirectory(workingDirectory); }
        
        wxString title = "codelite-terminal";
        if(parser.Found("t", &title)) { m_options.SetTitle(title); }
        
        wxString command;
        if(parser.Found("c", &command)) { m_options.SetCommand(command); }
        
        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        MainFrame* mainFrame = new MainFrame(NULL, m_options);
        SetTopWindow(mainFrame);
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
