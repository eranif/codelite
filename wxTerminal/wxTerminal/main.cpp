#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>
#include <iostream>
#include <thread>
#include <wx/cmdline.h>
#include "wxTerminalOptions.h"

#ifdef __WXMSW__
void RedirectIOToConsole()
{
    AllocConsole();
    freopen("CON", "wb", stdout);
    freopen("CON", "wb", stderr);
    freopen("CON", "r", stdin); // Note: "r", not "w"
}
#endif

// Define the MainApp
class MainApp : public wxApp
{
    wxTerminalOptions m_options;

public:
    MainApp() {}
    virtual ~MainApp() {}

    void PrintUsage()
    {
        std::cout << wxApp::argv[0] << " [-t <title>] [-e] [-w] [-d <working directory>] [--cmd ...]" << std::endl;
        std::cout << "-t | --title             Set the console title\n" << std::endl;
        std::cout << "-w | --wait              Wait for any key to be pressed before exiting\n" << std::endl;
        std::cout << "-d | --working-directory Set the working directory\n" << std::endl;
        std::cout << "-p | --print-tty         Print terminal info to stdout\n" << std::endl;
        std::cout << std::endl;
        wxExit();
    }

    virtual bool OnInit()
    {
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
        parser.AddLongOption("title");
        parser.AddLongOption("working-directory");
        parser.AddSwitch("wait");
        parser.AddLongOption("print-tty");
        parser.Parse();

        if(parser.FoundSwitch("wait") != wxCMD_SWITCH_NOT_FOUND) { m_options.SetWaitOnExit(true); }
        if(parser.FoundSwitch("print-tty") != wxCMD_SWITCH_NOT_FOUND) { m_options.SetPrintTTY(true); }
        
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
