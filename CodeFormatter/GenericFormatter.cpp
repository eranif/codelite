#include "GenericFormatter.hpp"

#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "StringUtils.h"
#include "clDirChanger.hpp"
#include "clRemoteHost.hpp"
#include "clSFTPManager.hpp"
#include "clTempFile.hpp"
#include "environmentconfig.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macromanager.h"
#include "procutils.h"

#include <wx/msgdlg.h>

namespace
{
wxString join_array(const wxArrayString& arr)
{
    wxString cmd;
    for (auto c : arr) {
        cmd << WrapWithQuotes(c) << " ";
    }

    if (!cmd.IsEmpty()) {
        cmd.RemoveLast();
    }
    return cmd;
}

wxArrayString to_wx_array(const std::vector<wxString>& arr)
{
    wxArrayString wxarr;
    wxarr.reserve(arr.size());

    for (const auto& s : arr) {
        wxarr.push_back(s);
    }
    return wxarr;
}

wxString replace_macros(const wxString& expression, const wxString& filepath)
{
    wxString expand_stage1 = MacroManager::Instance()->ExpandFileMacros(expression, filepath);
    wxString resolved = MacroManager::Instance()->Expand(expand_stage1, clGetManager(), wxEmptyString, wxEmptyString);
    return resolved;
}

wxString get_command_with_desc(const wxArrayString& command_arr, const wxString& desc)
{
    wxString command = StringUtils::BuildCommandStringFromArray(command_arr, StringUtils::WITH_COMMENT_PREFIX);
    command.Prepend(wxString() << "# " << desc << "\n");
    return command;
}

} // namespace

GenericFormatter::GenericFormatter()
{
    SetWorkingDirectory("$(WorkspacePath)");
    Bind(wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, &GenericFormatter::OnAsyncShellProcessTerminated, this);
#if USE_SFTP
    Bind(wxEVT_SFTP_ASYNC_EXEC_STDERR, &GenericFormatter::OnRemoteCommandStderr, this);
    Bind(wxEVT_SFTP_ASYNC_EXEC_STDOUT, &GenericFormatter::OnRemoteCommandStdout, this);
    Bind(wxEVT_SFTP_ASYNC_EXEC_DONE, &GenericFormatter::OnRemoteCommandDone, this);
    Bind(wxEVT_SFTP_ASYNC_EXEC_ERROR, &GenericFormatter::OnRemoteCommandError, this);
#endif
}

GenericFormatter::~GenericFormatter()
{
    Unbind(wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, &GenericFormatter::OnAsyncShellProcessTerminated, this);
#if USE_SFTP
    Unbind(wxEVT_SFTP_ASYNC_EXEC_STDERR, &GenericFormatter::OnRemoteCommandStderr, this);
    Unbind(wxEVT_SFTP_ASYNC_EXEC_STDOUT, &GenericFormatter::OnRemoteCommandStdout, this);
    Unbind(wxEVT_SFTP_ASYNC_EXEC_DONE, &GenericFormatter::OnRemoteCommandDone, this);
    Unbind(wxEVT_SFTP_ASYNC_EXEC_ERROR, &GenericFormatter::OnRemoteCommandError, this);
#endif
}

wxString GenericFormatter::GetCommandAsString() const { return join_array(m_command); }

/**
 * @brief synchronously format a file, return true if the process exited with return code `0`
 * @param cmd command to execute
 * @param wd working directory
 * @param inplace_formatter set to true if the formatter tool modifies the file directly
 * @param output contains the formatter output. If the `inplace_formatter` is `true`, output is always empty string
 */
bool GenericFormatter::SyncFormat(const wxString& cmd, const wxString& wd, bool inplace_formatter, wxString* output)
{
    // save the current directory
    clDirChanger cd{ wd };
    auto envlist = CreateLocalEnv();
    EnvSetter setter{ envlist.get() };
    bool res = ProcUtils::ShellExecSync(cmd, output) == 0;
    if (inplace_formatter) {
        output->clear();
    }
    return res;
}

/**
 * @brief format source file using background thread, when the formatting is done, fire an event to the sink object
 */
void GenericFormatter::AsyncFormat(const wxString& cmd, const wxString& wd, const wxString& filepath,
                                   bool inplace_formatter, wxEvtHandler* sink)
{
    clDirChanger cd{ wd };

    // Apply environment
    auto envlist = CreateLocalEnv();
    EnvSetter setter{ envlist.get() };

    long pid = wxNOT_FOUND;
    if (ProcUtils::ShellExecAsync(cmd, &pid, this)) {
        m_pid_commands.insert({ pid, CommandMetadata{ cmd, filepath, sink } });
    }
}

bool GenericFormatter::DoFormatFile(const wxString& filepath, wxEvtHandler* sink, wxString* output)
{
    // Create a copy
    wxString cmd = GetCommandAsString();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    wxBusyCursor bc;
    if (sink) {
        clDEBUG() << "Formatting file (async):" << filepath << "Working dir:" << wd << "Calling:" << cmd << endl;
        AsyncFormat(cmd, wd, filepath, IsInplaceFormatter(), sink);
        return true;
    } else {
        clDEBUG() << "Formatting file (sync):" << filepath << "Working dir:" << wd << "Calling:" << cmd << endl;
        return SyncFormat(cmd, wd, IsInplaceFormatter(), output);
    }
}

bool GenericFormatter::FormatFile(const wxFileName& filepath, wxEvtHandler* sink)
{
    return FormatFile(filepath.GetFullPath(), sink);
}

bool GenericFormatter::FormatRemoteFile(const wxString& filepath, wxEvtHandler* sink)
{
#if USE_SFTP
    if (!CanHandleRemoteFile()) {
        return false;
    }

    // Create a copy
    wxString cmd = GetRemoteCommand();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    clSFTPManager::Get().AsyncExecute(this, clRemoteHost::Instance()->GetActiveAccount(), cmd, wd, nullptr);
    m_inFlightFiles.push_back({ filepath, sink });
#else
    wxUnusedVar(filepath);
    wxUnusedVar(sink);
#endif
    return true;
}

bool GenericFormatter::FormatFile(const wxString& filepath, wxEvtHandler* sink)
{
    return DoFormatFile(filepath, sink, nullptr);
}

bool GenericFormatter::FormatString(const wxString& content, const wxString& fullpath, wxString* output)
{
    auto file_type = FileExtManager::GetType(fullpath);
    if (!CanHandle(file_type)) {
        return false;
    }

    wxString fullpath_linux_style = fullpath;
    fullpath_linux_style.Replace("\\", "/");

    wxString dirpart = fullpath_linux_style.BeforeLast('/');
    clTempFile tmpfile{ dirpart, "txt" };

    if (!tmpfile.Write(content)) {
        clWARNING() << "failed to write content to temp file:" << tmpfile.GetFullPath() << endl;
        return false;
    }

    if (!DoFormatFile(tmpfile.GetFullPath(), nullptr, output)) {
        return false;
    }

    if (IsInplaceFormatter()) {
        // read the content of the temp file and return it
        return FileUtils::ReadFileContent(tmpfile.GetFullPath(), *output);
    }
    return true;
}

void GenericFormatter::FromJSON(const JSONItem& json)
{
    SourceFormatterBase::FromJSON(json);
    m_command = json["command"].toArrayString();
    m_workingDirectory = json["working_directory"].toString();
}

JSONItem GenericFormatter::ToJSON() const
{
    auto json = SourceFormatterBase::ToJSON();
    json.addProperty("command", m_command);
    json.addProperty("working_directory", m_workingDirectory);
    return json;
}

void GenericFormatter::SetCommand(const std::vector<wxString>& command) { SetCommand(to_wx_array(command)); }

void GenericFormatter::SetCommandFromString(const wxString& command)
{
    SetCommand(StringUtils::BuildCommandArrayFromString(command));
}

wxString GenericFormatter::GetCommandWithComments() const { return get_command_with_desc(m_command, m_description); }

void GenericFormatter::SetRemoteCommand(const wxString& cmd, const wxString& remote_wd, const clEnvList_t& env)
{
    m_remote_command = cmd;
    m_remote_env = env;
    m_remote_wd = remote_wd;
}

void GenericFormatter::OnAsyncShellProcessTerminated(clShellProcessEvent& event)
{
    event.Skip();
    if (m_pid_commands.count(event.GetPid()) == 0) {
        clWARNING() << "Could not find command for process:" << event.GetPid() << endl;
        return;
    }

    // remove the entry from the map
    auto command_data = m_pid_commands[event.GetPid()];
    m_pid_commands.erase(event.GetPid());

    if (event.GetExitCode() != 0) {
        wxString errmsg;
        errmsg << wxT("\u26A0") << _(" format error. Process exit code: ") << event.GetExitCode();
        clGetManager()->SetStatusMessage(errmsg, 3);
        return;
    }

    // notify the sink that formatting is done
    if (command_data.m_sink) {
        clSourceFormatEvent format_completed_event{ IsInplaceFormatter() ? wxEVT_FORMAT_INPLACE_COMPELTED
                                                                         : wxEVT_FORMAT_COMPELTED };
        format_completed_event.SetFormattedString(IsInplaceFormatter() ? "" : event.GetOutput());
        format_completed_event.SetFileName(command_data.m_filepath);
        command_data.m_sink->QueueEvent(format_completed_event.Clone());
    }
}

void GenericFormatter::OnRemoteCommandStdout(clCommandEvent& event)
{
    if (m_inFlightFiles.empty()) {
        clERROR() << "GenericFormatter::OnRemoteCommandStdout is called but NO inflight files" << endl;
        return;
    }

    if (!IsInplaceFormatter()) {
        const wxString& filepath = m_inFlightFiles.front().first;
        clSourceFormatEvent format_completed_event{ wxEVT_FORMAT_COMPELTED };
        format_completed_event.SetFormattedString(wxString::FromUTF8(event.GetStringRaw()));
        format_completed_event.SetFileName(filepath);
        m_inFlightFiles.front().second->AddPendingEvent(format_completed_event);
    }
}

void GenericFormatter::OnRemoteCommandStderr(clCommandEvent& event)
{
    wxString message;
    message << wxString::FromUTF8(event.GetStringRaw());
    clGetManager()->AppendOutputTabText(eOutputPaneTab::kOutputTab_Output, message, false);
}

void GenericFormatter::OnRemoteCommandDone(clCommandEvent& event)
{
    wxUnusedVar(event);
    if (m_inFlightFiles.empty()) {
        clERROR() << "GenericFormatter::OnRemoteCommandDone is called but NO inflight files" << endl;
        return;
    }
    auto sink = m_inFlightFiles.front().second;
    if (IsInplaceFormatter()) {
        const wxString& filepath = m_inFlightFiles.front().first;
        clSourceFormatEvent format_completed_event{ wxEVT_FORMAT_INPLACE_COMPELTED };
        format_completed_event.SetFileName(filepath);
        sink->AddPendingEvent(format_completed_event);
    }
    m_inFlightFiles.erase(m_inFlightFiles.begin());
}

void GenericFormatter::OnRemoteCommandError(clCommandEvent& event)
{
    clERROR() << "Code Formatter:" << event.GetString() << endl;
    if (m_inFlightFiles.empty()) {
        clERROR() << "GenericFormatter::OnRemoteCommandError is called but NO inflight files" << endl;
        return;
    }
    m_inFlightFiles.erase(m_inFlightFiles.begin());
}

std::unique_ptr<clEnvList_t> GenericFormatter::CreateLocalEnv()
{
    std::unique_ptr<clEnvList_t> env{ new clEnvList_t };
    if (clFileSystemWorkspace::Get().IsOpen()) {
        env.reset(new clEnvList_t(clFileSystemWorkspace::Get().GetEnvironment()));
    }
    return env;
}
