#include "GenericFormatter.hpp"

#include "Platform.hpp"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clDirChanger.hpp"
#include "clTempFile.hpp"
#include "clWorkspaceManager.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macromanager.h"
#include "procutils.h"
#include "workspace.h"

#include <sstream>
#include <thread>
#include <wx/msgdlg.h>

namespace
{
wxString join_array(const wxArrayString& arr)
{
    wxString cmd;
    for(auto c : arr) {
        cmd << WrapWithQuotes(c) << " ";
    }

    if(!cmd.IsEmpty()) {
        cmd.RemoveLast();
    }
    return cmd;
}

wxArrayString to_wx_array(const std::vector<wxString>& arr)
{
    wxArrayString wxarr;
    wxarr.reserve(arr.size());

    for(const auto& s : arr) {
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

bool sync_format(const wxString& cmd, const wxString& wd, bool inplace_formatter, wxString* output)
{
    // save the current directory
    clDirChanger cd{ wd };
    EnvSetter es(NULL, NULL, wxEmptyString, wxEmptyString);
    bool res = ProcUtils::ShellExecSync(cmd, output) == 0;
    if(inplace_formatter) {
        output->clear();
    }
    return res;
}
} // namespace

GenericFormatter::GenericFormatter()
{
    SetWorkingDirectory("$(WorkspacePath)");
    Bind(wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, &GenericFormatter::OnAsyncShellProcessTerminated, this);
}

GenericFormatter::~GenericFormatter()
{
    Unbind(wxEVT_SHELL_ASYNC_PROCESS_TERMINATED, &GenericFormatter::OnAsyncShellProcessTerminated, this);
}

wxString GenericFormatter::GetCommandAsString() const { return join_array(m_command); }

/**
 * @brief format source file using background thread, when the formatting is done, fire an event to the sink object
 */
void GenericFormatter::async_format(const wxString& cmd, const wxString& wd, const wxString& filepath,
                                    bool inplace_formatter, wxEvtHandler* sink)
{
    clDirChanger cd{ wd };
    long pid = wxNOT_FOUND;
    if(ProcUtils::ShellExecAsync(cmd, &pid, this)) {
        m_pid_commands.insert({ pid, CommandMetadata{ cmd, filepath, sink } });
    }
}

bool GenericFormatter::DoFormatFile(const wxString& filepath, wxEvtHandler* sink, wxString* output)
{
    // Create a copy
    wxString cmd = GetCommandAsString();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    wxBusyCursor bc;
    if(sink) {
        async_format(cmd, wd, filepath, IsInplaceFormatter(), sink);
        return true;
    } else {
        return sync_format(cmd, wd, IsInplaceFormatter(), output);
    }
}

bool GenericFormatter::FormatFile(const wxFileName& filepath, wxEvtHandler* sink)
{
    return FormatFile(filepath.GetFullPath(), sink);
}

bool GenericFormatter::FormatRemoteFile(const wxString& filepath, wxEvtHandler* sink)
{
    if(!CanHandleRemoteFile()) {
        return false;
    }

    // Create a copy
    wxString cmd = GetRemoteCommand();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    async_format(cmd, wd, filepath, IsInplaceFormatter(), sink);
    return true;
}

bool GenericFormatter::FormatFile(const wxString& filepath, wxEvtHandler* sink)
{
    return DoFormatFile(filepath, sink, nullptr);
}

bool GenericFormatter::FormatString(const wxString& content, const wxString& fullpath, wxString* output)
{
    auto file_type = FileExtManager::GetType(fullpath);
    if(!CanHandle(file_type)) {
        return false;
    }

    wxString fullpath_linux_style = fullpath;
    fullpath_linux_style.Replace("\\", "/");

    wxString dirpart = fullpath_linux_style.BeforeLast('/');
    clTempFile tmpfile{ dirpart, "txt" };

    if(!tmpfile.Write(content)) {
        clWARNING() << "failed to write content to temp file:" << tmpfile.GetFullPath() << endl;
        return false;
    }

    if(!DoFormatFile(tmpfile.GetFullPath(), nullptr, output)) {
        return false;
    }

    if(IsInplaceFormatter()) {
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

void GenericFormatter::SetRemoteCommand(const wxString& cmd) { m_remote_command = cmd; }

void GenericFormatter::OnAsyncShellProcessTerminated(clShellProcessEvent& event)
{
    event.Skip();
    if(m_pid_commands.count(event.GetPid()) == 0) {
        clWARNING() << "Could not find command for process:" << event.GetPid() << endl;
        return;
    }

    // remove the entry from the map
    auto command_data = m_pid_commands[event.GetPid()];
    m_pid_commands.erase(event.GetPid());

    if(event.GetExitCode() != 0) {
        wxString errmsg;
        errmsg << wxT("\u26A0") << _(" format error. Process exit code: ") << event.GetExitCode();
        clGetManager()->SetStatusMessage(errmsg, 3);
        return;
    }

    // notify the sink that formatting is done
    if(command_data.m_sink) {
        clSourceFormatEvent format_completed_event{ IsInplaceFormatter() ? wxEVT_FORMAT_INPLACE_COMPELTED
                                                                         : wxEVT_FORMAT_COMPELTED };
        format_completed_event.SetFormattedString(IsInplaceFormatter() ? "" : event.GetOutput());
        format_completed_event.SetFileName(command_data.m_filepath);
        command_data.m_sink->QueueEvent(format_completed_event.Clone());
    }
}
