#include "phpexecutor.h"

#include "AsyncProcess/asyncprocess.h"
#include "Console/clConsoleBase.h"
#include "StringUtils.h"
#include "TerminalEmulator/TerminalEmulatorFrame.h"
#include "clplatform.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "php_configuration_data.h"
#include "php_event.h"
#include "php_project_settings_data.h"

#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/uri.h>

bool PHPExecutor::Exec(const PHPProjectSettingsData& settings,
                       const wxString& urlOrFilePath,
                       const wxString& xdebugSessionName,
                       bool neverPauseOnExit)
{
    if (settings.GetRunAs() == PHPProjectSettingsData::kRunAsWebsite) {
        return RunRUL(settings, urlOrFilePath, xdebugSessionName);

    } else {
        return DoRunCLI(urlOrFilePath, &settings, xdebugSessionName, neverPauseOnExit);
    }
}

bool PHPExecutor::IsRunning() const { return m_terminal.IsRunning(); }

void PHPExecutor::Stop() { m_terminal.Terminate(); }

bool PHPExecutor::RunRUL(const PHPProjectSettingsData& settings,
                         const wxString& urlToRun,
                         const wxString& xdebugSessionName)
{
    wxURI uri(urlToRun);

    wxString url;
    wxString queryStrnig = uri.GetQuery();
    if (queryStrnig.IsEmpty() && !xdebugSessionName.IsEmpty()) {
        // no query string was provided by the user
        url << uri.BuildURI() << "?XDEBUG_SESSION_START=" << xdebugSessionName;

    } else {
        url << uri.BuildURI();
    }

    PHPEvent evtLoadURL(wxEVT_PHP_LOAD_URL);
    evtLoadURL.SetUrl(url);
    evtLoadURL.SetUseDefaultBrowser(settings.IsUseSystemBrowser());
    EventNotifier::Get()->AddPendingEvent(evtLoadURL);
    return true;
}

bool PHPExecutor::DoRunCLI(const wxString& script,
                           const PHPProjectSettingsData* settings,
                           const wxString& xdebugSessionName,
                           bool neverPauseOnExit)
{
    if (IsRunning()) {
        ::wxMessageBox(_("Another process is already running"),
                       wxT("CodeLite"),
                       wxOK | wxICON_INFORMATION,
                       wxTheApp->GetTopWindow());
        return false;
    }

    wxString errmsg;
    auto [php, cmd] = DoGetCLICommand(script, settings, errmsg);
    if (php.empty() || cmd.empty()) {
        ::wxMessageBox(errmsg, wxT("CodeLite"), wxOK | wxICON_INFORMATION, wxTheApp->GetTopWindow());
        return false;
    }

    wxString wd;
    if (settings) {
        wd = settings->GetWorkingDirectory();
    }

    clDEBUG() << "Php:" << php << endl;
    clDEBUG() << "Arguments:" << cmd << endl;

    // Apply the environment variables
    // export XDEBUG_CONFIG="idekey=session_name remote_host=localhost profiler_enable=1"
    wxStringMap_t om;
    if (!xdebugSessionName.IsEmpty()) {

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
    if (!xdebugSessionName.IsEmpty()) {
        // debugging
        return m_terminal.ExecuteNoConsole(cmd, wd);
    } else {
        // Launch the terminal UI
        auto console = clConsoleBase::GetTerminal();
        console->SetTerminalNeeded(true);
        console->SetWorkingDirectory(wd);
        console->SetWaitWhenDone(true);
        console->SetCommand(php, cmd);
        return console->Start();
    }
}

bool PHPExecutor::RunScript(const wxString& script, wxString& php_output)
{
    wxString errmsg;
    auto [php, cmd] = DoGetCLICommand(script, nullptr, errmsg);
    if (cmd.IsEmpty()) {
        ::wxMessageBox(errmsg, wxT("CodeLite"), wxOK | wxICON_INFORMATION, wxTheApp->GetTopWindow());
        return false;
    }

    IProcess::Ptr_t phpcli(
        ::CreateSyncProcess(php + " " + cmd, IProcessCreateDefault | IProcessCreateWithHiddenConsole));
    CHECK_PTR_RET_FALSE(phpcli);

    phpcli->WaitForTerminate(php_output);
    return true;
}

std::pair<wxString, wxString>
PHPExecutor::DoGetCLICommand(const wxString& script, const PHPProjectSettingsData* settings, wxString& errmsg)
{
    wxArrayString args;
    wxString php;
    wxArrayString includePath;
    wxString index;
    wxString ini;

    PHPConfigurationData globalConf;
    globalConf.Load();

    if (settings) {
        args = ::wxStringTokenize(settings->GetArgs(), wxT("\n\r"), wxTOKEN_STRTOK);
        includePath = settings->GetIncludePathAsArray();
        php = settings->GetPhpExe();
        index = script;
        ini = settings->GetPhpIniFile();

    } else {

        index = script;
        php = globalConf.GetPhpExe();
        includePath = globalConf.GetIncludePaths();
    }

    ini.Trim().Trim(false);
    if (ini.Contains(" ")) {
        ini.Prepend("\"").Append("\"");
    }

    if (index.empty()) {
        errmsg = _("Please set an index file to execute in the project settings");
        return {};
    }

    if (php.empty()) {
        php = globalConf.GetPhpExe();
        if (php.empty()) {
            errmsg = _("Could not find any PHP binary to execute. Please set one in from: 'PHP | Settings'");
            return {};
        }
    }

    // An example of execution:
    // php.exe -c /path/to/ini.ini -d "display_errors=On" -d include_path="C:\php\includes" file2.php

    // Build the command for execution
    wxString cmd;
    php = StringUtils::WrapWithDoubleQuotes(php);

    if (!ini.empty()) {
        cmd << " -c " << ini << " ";
    }

    cmd << wxT(" -d display_errors=On "); // Enable error reporting
    cmd << wxT(" -d html_errors=Off ");

    // add the include path
    if (includePath.empty() == false) {
        cmd << wxT("-d include_path=\"");
        for (size_t i = 0; i < includePath.GetCount(); i++) {
            cmd << includePath.Item(i) << clPlatform::PathSeparator;
        }
        cmd << wxT("\" ");
    }

    StringUtils::WrapWithQuotes(index);
    cmd << index;

    if (!args.empty()) {
        cmd << " ";
    }

    // set the program arguments to run (after the index file is set)
    if (!args.empty()) {
        for (const wxString& arg : args) {
            cmd << StringUtils::WrapWithDoubleQuotes(arg) << " ";
        }
        cmd.RemoveLast();
    }
    return {php, cmd};
}
