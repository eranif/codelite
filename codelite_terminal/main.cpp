#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>
#include <wx/cmdline.h>
#include "terminal_options.h"
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/crt.h>
#include "commandlineparser.h"

#ifdef __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif

#ifdef __WXMSW__
typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
#endif

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    void PrintUsage()
    {
        wxPrintf("%s [-t <title>] [-e] [-w] [-d <working directory>] [--cmd <command to execute>]\n", wxApp::argv[0]);
        wxPrintf("-t | --title             Set the console title\n");
        wxPrintf("-e | --exit              Exit when execution of command terminates\n");
        wxPrintf("-w | --wait              Wait for any key to be pressed before exiting\n");
        wxPrintf("-d | --working-directory Set the working directory\n");
        wxPrintf("-p | --print-info        Print terminal info to stdout\n");
        wxPrintf("-z | --always-on-top     The terminal is always on top of all windows\n");
        wxPrintf("-g | --dbg-terminal      The terminal is for debugging redirection purposes\n");
        wxPrintf("\n");
        exit(1);
    }

    virtual bool OnInit()
    {

#ifdef __WXMSW__
        HINSTANCE m_user32Dll = LoadLibrary(L"User32.dll");
        if(m_user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(m_user32Dll, "SetProcessDPIAware");
            if(pFunc) {
                pFunc();
            }
            FreeLibrary(m_user32Dll);
            m_user32Dll = NULL;
        }
#endif

        SetAppName("codelite-terminal");

        CommandLineParser parser(wxApp::argc, wxApp::argv);
        parser.AddOption("t", "title", CommandLineParser::kOptionWithValue | CommandLineParser::kOptional);
        parser.AddOption("d", "working-directory", CommandLineParser::kOptionWithValue | CommandLineParser::kOptional);
        parser.AddOption("e", "exit");          // optional, no value
        parser.AddOption("w", "wait");          // optional, no value
        parser.AddOption("p", "print-info");    // optional
        parser.AddOption("z", "always-on-top"); // optional
        parser.AddOption("g", "dbg-terminal");  // optional
        parser.Parse();

        {
            wxLogNull noLog;
            wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            ::wxMkdir(wxStandardPaths::Get().GetUserDataDir());
        }

        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        TerminalOptions options;
        wxString commandToRun, title, workingDirectory;
        commandToRun = parser.GetCommand();
        workingDirectory = parser.GetArg("d", "working-directory");

        if(parser.HasOption("t", "title")) {
            options.SetTitle(parser.GetArg("t", "title"));

        } else if(!parser.GetCommand().IsEmpty()) {
            options.SetTitle(parser.GetCommand());
        }

        if(!workingDirectory.IsEmpty()) {
            ::wxSetWorkingDirectory(workingDirectory);
        }

        options.EnableFlag(TerminalOptions::kExitWhenInfiriorTerminates, parser.HasOption("e", "exit"));
        options.EnableFlag(TerminalOptions::kPauseBeforeExit, parser.HasOption("w", "wait"));
        options.EnableFlag(TerminalOptions::kPrintInfo, parser.HasOption("p", "print-info"));
        options.EnableFlag(TerminalOptions::kAlwaysOnTop, parser.HasOption("z", "always-on-top"));
        options.EnableFlag(TerminalOptions::kDebuggerTerminal, parser.HasOption("g", "dbg-terminal"));
        options.SetCommand(commandToRun);

        long style = wxDEFAULT_FRAME_STYLE;
        if(options.HasFlag(TerminalOptions::kAlwaysOnTop)) {
            style = wxSTAY_ON_TOP;
        }

        MainFrame* mainFrame = new MainFrame(NULL, options, style);
        SetTopWindow(mainFrame);

#ifdef __WXMAC__
        ProcessSerialNumber PSN;
        GetCurrentProcess(&PSN);
        TransformProcessType(&PSN, kProcessTransformToForegroundApplication);
#endif
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
