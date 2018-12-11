#include "PHPTerminal.h"
#include "TerminalEmulatorFrame.h"
#include "asyncprocess.h"
#include "clplatform.h"
#include "console_frame.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "php_configuration_data.h"
#include "php_event.h"
#include "php_project_settings_data.h"
#include "php_workspace.h"
#include "phpexecutor.h"
#include <cl_config.h>
#include <dirsaver.h>
#include <event_notifier.h>
#include <globals.h>
#include <processreaderthread.h>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/tokenzr.h>
#include <wx/uri.h>

PHPExecutor::PHPExecutor() {}

PHPExecutor::~PHPExecutor() {}

bool PHPExecutor::Exec(const wxString& projectName, const wxString& urlOrFilePath, const wxString& xdebugSessionName,
                       bool neverPauseOnExit)
{
    PHPProject::Ptr_t proj = PHPWorkspace::Get()->GetProject(projectName);
    CHECK_PTR_RET_FALSE(proj);

    if(proj->GetSettings().GetRunAs() == PHPProjectSettingsData::kRunAsWebsite) {
        return RunRUL(proj, urlOrFilePath, xdebugSessionName);

    } else {
        return DoRunCLI(urlOrFilePath, proj, xdebugSessionName, neverPauseOnExit);
    }
}

bool PHPExecutor::IsRunning() const { return m_terminal.IsRunning(); }

void PHPExecutor::Stop() { m_terminal.Terminate(); }

bool PHPExecutor::RunRUL(PHPProject::Ptr_t pProject, const wxString& urlToRun, const wxString& xdebugSessionName)
{
    const PHPProjectSettingsData& data = pProject->GetSettings();
    wxURI uri(urlToRun);

    wxString url;
    wxString queryStrnig = uri.GetQuery();
    if(queryStrnig.IsEmpty() && !xdebugSessionName.IsEmpty()) {
        // no query string was provided by the user
        url << uri.BuildURI() << "?XDEBUG_SESSION_START=" << xdebugSessionName;

    } else {
        url << uri.BuildURI();
    }

    CL_DEBUG("CodeLite: Calling URL: " + url);
    PHPEvent evtLoadURL(wxEVT_PHP_LOAD_URL);
    evtLoadURL.SetUrl(url);
    evtLoadURL.SetUseDefaultBrowser(data.IsUseSystemBrowser());
    EventNotifier::Get()->AddPendingEvent(evtLoadURL);
    return true;
}

bool PHPExecutor::DoRunCLI(const wxString& script, PHPProject::Ptr_t proj, const wxString& xdebugSessionName,
                           bool neverPauseOnExit)
{
    if(IsRunning()) {
        ::wxMessageBox(_("Another process is already running"), wxT("CodeLite"), wxOK | wxICON_INFORMATION,
                       wxTheApp->GetTopWindow());
        return false;
    }

    wxString errmsg;
    wxString cmd = DoGetCLICommand(script, proj, errmsg);
    if(cmd.IsEmpty()) {
        ::wxMessageBox(errmsg, wxT("CodeLite"), wxOK | wxICON_INFORMATION, wxTheApp->GetTopWindow());
        return false;
    }
    wxString wd;
    if(proj) {
        const PHPProjectSettingsData& data = proj->GetSettings();
        wd = data.GetWorkingDirectory();
    }

    clLogMessage(cmd);

    // Apply the environment variables
    // export XDEBUG_CONFIG="idekey=session_name remote_host=localhost profiler_enable=1"
    wxStringMap_t om;
    if(!xdebugSessionName.IsEmpty()) {

        PHPConfigurationData phpGlobalSettings;
        phpGlobalSettings.Load();
        int port = phpGlobalSettings.GetXdebugPort();

        wxString envname = "XDEBUG_CONFIG";
        wxString envvalue;
        envvalue << "idekey=" << xdebugSessionName << " remote_port=" << port;
        om.insert(std::make_pair(envname, envvalue));
    }

    EnvSetter serrter(&om);

    // Execute the command
    if(!xdebugSessionName.IsEmpty()) {
        // debugging
        return m_terminal.ExecuteNoConsole(cmd, wd);
    } else {
        // Launch the terminal UI
        ConsoleFrame* console = new ConsoleFrame(EventNotifier::Get()->TopFrame());
        console->Show();
        console->Execute(cmd, wd);
        return true;
    }
}

bool PHPExecutor::RunScript(const wxString& script, wxString& php_output)
{
    wxString errmsg;
    wxString cmd = DoGetCLICommand(script, PHPProject::Ptr_t(NULL), errmsg);
    if(cmd.IsEmpty()) {
        ::wxMessageBox(errmsg, wxT("CodeLite"), wxOK | wxICON_INFORMATION, wxTheApp->GetTopWindow());
        return false;
    }

    IProcess::Ptr_t phpcli(::CreateSyncProcess(cmd, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    CHECK_PTR_RET_FALSE(phpcli);

    phpcli->WaitForTerminate(php_output);
    return true;
}

wxString PHPExecutor::DoGetCLICommand(const wxString& script, PHPProject::Ptr_t proj, wxString& errmsg)
{
    wxArrayString args;
    wxString php;
    wxArrayString includePath;
    wxString index;
    wxString ini;

    PHPConfigurationData globalConf;
    globalConf.Load();

    if(proj) {
        const PHPProjectSettingsData& data = proj->GetSettings();
        args = ::wxStringTokenize(data.GetArgs(), wxT("\n\r"), wxTOKEN_STRTOK);
        includePath = data.GetIncludePathAsArray();
        php = data.GetPhpExe();
        index = script;
        ini = data.GetPhpIniFile();

    } else {

        index = script;
        php = globalConf.GetPhpExe();
        includePath = globalConf.GetIncludePaths();
    }

    ini.Trim().Trim(false);
    if(ini.Contains(" ")) {
        ini.Prepend("\"").Append("\"");
    }
    if(index.IsEmpty()) {
        errmsg = _("Please set an index file to execute in the project settings");
        return "";
    }

    if(php.IsEmpty()) {
        php = globalConf.GetPhpExe();
        if(php.IsEmpty()) {
            errmsg = _("Could not find any PHP binary to execute. Please set one in from: 'PHP | Settings'");
            return "";
        }
    }

    // An example of execution:
    // php.exe -c /path/to/ini.ini -d "display_errors=On" -d include_path="C:\php\includes" file2.php

    // Build the command for execution
    wxString cmd;

    cmd << php; // Wrap the php exe with qoutes
#ifdef __WXMSW__
    ::WrapWithQuotes(cmd);
#else
    cmd.Replace(" ", "\\ "); // escape spaces
#endif
    if(!ini.IsEmpty()) {
        cmd << " -c " << ini << " ";
    }

    cmd << wxT(" -d display_errors=On "); // Enable error reporting
    cmd << wxT(" -d html_errors=Off ");

    // add the include path
    if(includePath.IsEmpty() == false) {
        cmd << wxT("-d include_path=\"");
        for(size_t i = 0; i < includePath.GetCount(); i++) {
            cmd << includePath.Item(i) << clPlatform::PathSeparator;
        }
        cmd << wxT("\" ");
    }

    ::WrapWithQuotes(index);
    cmd << index;

    if(!args.IsEmpty()) {
        cmd << " ";
    }

    // set the program arguments to run (after the index file is set)
    for(size_t i = 0; i < args.GetCount(); ++i) {
        cmd << args.Item(i) << " ";
    }
    return cmd;
}
