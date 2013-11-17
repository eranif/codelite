#include <wx/app.h>
#include <wx/event.h>
#include "MainFrame.h"
#include <wx/image.h>
#include <wx/cmdline.h>
#include "terminal_options.h"
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/stdpaths.h>

#ifdef __WXMAC__
#   include <ApplicationServices/ApplicationServices.h>
#endif

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit() {
        SetAppName("codelite-terminal");
        wxCmdLineParser parser(wxApp::argc, wxApp::argv);
        {
            wxLogNull noLog;
            wxFileName::Mkdir(wxStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            ::wxMkdir(wxStandardPaths::Get().GetUserDataDir());
        }

        parser.AddOption("c", "command", "command line to execute",                       wxCMD_LINE_VAL_STRING);
        parser.AddOption("t", "title",   "set the console title",                         wxCMD_LINE_VAL_STRING);
        parser.AddSwitch("e", "exit",    "exit when execution of command terminates",     wxCMD_LINE_PARAM_OPTIONAL);
        parser.AddSwitch("w", "wait",    "wait for any key to be pressed before exiting", wxCMD_LINE_PARAM_OPTIONAL);

        if ( !DoParseCommandLine( parser ) ) {
            return false;
        }

        // Add the common image handlers
        wxImage::AddHandler( new wxPNGHandler );
        wxImage::AddHandler( new wxJPEGHandler );

        TerminalOptions options;
        wxString commandToRun, title;
        parser.Found("c", &commandToRun);
        if ( parser.Found("t", &title) ) {
            options.SetTitle( title );
        }

        options.EnableFlag( TerminalOptions::kExitWhenInfiriorTerminates,   parser.Found("e") );
        options.EnableFlag( TerminalOptions::kPauseBeforeExit,              parser.Found("w") );
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

    bool DoParseCommandLine(wxCmdLineParser& parser) {
        parser.Parse(false);
        return true;
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
