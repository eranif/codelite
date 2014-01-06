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
#   include <ApplicationServices/ApplicationServices.h>
#endif

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    void PrintUsage() {
        wxPrintf("%s [-t <title>] [-e] [-w] [-d <working directory>] [--cmd <command to execute>]\n", wxApp::argv[0]);
        wxPrintf("-t | --title             Set the console title\n");
        wxPrintf("-e | --exit              Exit when execution of command terminates\n");
        wxPrintf("-w | --wait              Wait for any key to be pressed before exiting\n");
        wxPrintf("-d | --working-directory Set the working directory\n");
        wxPrintf("\n");
        exit(1);
    }

    virtual bool OnInit() {

        SetAppName("codelite-terminal");

        CommandLineParser parser(wxApp::argc, wxApp::argv);
        parser.AddOption("t", "title", CommandLineParser::kOptionWithValue|CommandLineParser::kOptional);
        parser.AddOption("d", "working-directory", CommandLineParser::kOptionWithValue|CommandLineParser::kOptional);
        parser.AddOption("e", "exit"); // optional, no value
        parser.AddOption("w", "wait"); // optional, no value
        parser.Parse();

        {
            wxLogNull noLog;
            wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            ::wxMkdir(wxStandardPaths::Get().GetUserDataDir());
        }

        // Add the common image handlers
        wxImage::AddHandler( new wxPNGHandler );
        wxImage::AddHandler( new wxJPEGHandler );

        TerminalOptions options;
        wxString commandToRun, title, workingDirectory;
        commandToRun = parser.GetCommand();
        workingDirectory = parser.GetArg("d", "working-directory");

        if ( parser.HasOption("t", "title") ) {
            options.SetTitle( parser.GetArg("t", "title") );

        } else if ( !parser.GetCommand().IsEmpty() ) {
            options.SetTitle( parser.GetCommand() );
        }

        if ( !workingDirectory.IsEmpty() ) {
            ::wxSetWorkingDirectory( workingDirectory );
        }

        options.EnableFlag( TerminalOptions::kExitWhenInfiriorTerminates,   parser.HasOption("e", "exit") );
        options.EnableFlag( TerminalOptions::kPauseBeforeExit,              parser.HasOption("w", "wait") );
        options.SetCommand( commandToRun );

        MainFrame *mainFrame = new MainFrame(NULL, options);
        SetTopWindow(mainFrame);

#ifdef __WXMAC__
        ProcessSerialNumber PSN;
        GetCurrentProcess(&PSN);
        TransformProcessType(&PSN,kProcessTransformToForegroundApplication);
#endif
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
