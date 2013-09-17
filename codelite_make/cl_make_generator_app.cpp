#include "cl_make_generator_app.h"
#include <workspace.h>
#include <wx/filename.h>
#include <builder_gnumake.h>

IMPLEMENT_APP_CONSOLE(clMakeGeneratorApp)

static const wxCmdLineEntryDesc g_cmdDesc[] = {
    { wxCMD_LINE_SWITCH, "h", "help", "show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
    { wxCMD_LINE_OPTION, "w", "workspace", "codelite workspace file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_OPTION, "c", "config", "configuration name to generate", wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_OPTION, "p", "project",   "project to build, if non given codelite will build the active project", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_NONE }
};

clMakeGeneratorApp::clMakeGeneratorApp()
{
}

clMakeGeneratorApp::~clMakeGeneratorApp()
{
}

int clMakeGeneratorApp::OnExit()
{
    return TRUE;
}

bool clMakeGeneratorApp::OnInit()
{
    wxCmdLineParser parser(wxAppConsole::argc, wxAppConsole::argv);
    if ( !DoParseCommandLine( parser ) ) 
        return false;
    
    wxFileName fnWorkspace(m_workspaceFile);
    if ( fnWorkspace.IsRelative() ) {
        fnWorkspace.MakeAbsolute(m_workingDirectory);
    }
    
    wxString errmsg;
    WorkspaceST::Get()->OpenWorkspace(fnWorkspace.GetFullPath(), errmsg);
    
    bool projectOnly = true;
    if ( m_project.IsEmpty() ) {
        projectOnly = false;
        m_project = WorkspaceST::Get()->GetActiveProjectName();
    }
    
    // Which makefile should we create?
    BuilderGnuMake builder;
    if ( !builder.Export(m_project, m_configuration, projectOnly, true, errmsg) ) {
        wxFprintf(stderr, "Error while exporting makefile: %s\n", errmsg);
        return false;
    }
    
    wxString commandToRun;
    if ( projectOnly ) {
        commandToRun = builder.GetPOBuildCommand(m_project, m_configuration);
        
    } else {
        commandToRun = builder.GetBuildCommand(m_project, m_configuration);
        
    }
    
    wxFprintf(stdout, "-- Makefile generation completed successfully!\n");
    wxFprintf(stdout, "-- To use the makefile, run the following commands from a terminal:\n");
    wxFprintf(stdout, "    > cd %s\n", fnWorkspace.GetPath());
    wxFprintf(stdout, "    > %s\n", commandToRun);
    
    CallAfter( &clMakeGeneratorApp::DoExitApp);
    return true;
}

bool clMakeGeneratorApp::DoParseCommandLine(wxCmdLineParser& parser)
{
    parser.SetDesc( g_cmdDesc );
    int res = parser.Parse(false);
    if ( res == wxNOT_FOUND )
        return false;
    
    if ( !parser.Found("w", &m_workspaceFile) ) {
        parser.Usage();
        return false;
    }
    
    if ( !parser.Found("c", &m_configuration) ) {
        parser.Usage();
        return false;
    }
    
    parser.Found("p", &m_project);
    m_workingDirectory = ::wxGetCwd();
    return true;
}

void clMakeGeneratorApp::DoExitApp()
{
    ExitMainLoop();
}
