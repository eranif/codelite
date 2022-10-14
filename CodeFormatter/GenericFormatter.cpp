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
#include "macromanager.h"
#include "procutils.h"
#include "workspace.h"

#include <sstream>
#include <thread>

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

void sync_format(const wxString& cmd, const wxString& wd, bool inplace_formatter, wxString* output)
{
    // save the current directory
    clDirChanger cd{ wd };
    *output = ::wxShellExec(cmd, wxEmptyString);

    // notify the sink that formatting is done
    if(inplace_formatter) {
        output->clear();
    }
}

} // namespace

GenericFormatter::GenericFormatter()
{
    SetWorkingDirectory("$(WorkspacePath)");
    m_concurrent.set_pool_size(5);
}

GenericFormatter::~GenericFormatter() { m_concurrent.shutdown(); }

wxString GenericFormatter::GetCommandAsString() const { return join_array(m_command); }

/**
 * @brief format source file using background thread, when the formatting is done, fire an event to the sink object
 */
void GenericFormatter::thread_format(const wxString& cmd, const wxString& wd, const wxString& filepath,
                                     bool inplace_formatter, wxEvtHandler* sink)
{
    // ensure the pool is running
    if(!m_concurrent.is_running()) {
        m_concurrent.run();
    }

    auto format_callback = [cmd, wd, filepath, inplace_formatter, sink]() {
        // save the current directory
        std::stringstream ss;
        ss << std::this_thread::get_id();

        clDirChanger cd{ wd };
        clDEBUG() << "worker thread #" << ss.str() << "foramtting file:" << filepath << endl;
        wxString output = ::wxShellExec(cmd, wxEmptyString);
        clDEBUG() << "worker thread #" << ss.str() << "foramtting file:" << filepath << ".. done" << endl;

        // notify the sink that formatting is done
        clSourceFormatEvent event{ inplace_formatter ? wxEVT_FORMAT_INPLACE_COMPELTED : wxEVT_FORMAT_COMPELTED };
        event.SetFormattedString(inplace_formatter ? wxEmptyString : output);
        event.SetFileName(filepath);
        sink->QueueEvent(event.Clone());
    };
    m_concurrent.queue(std::move(format_callback));
}

bool GenericFormatter::DoFormatFile(const wxString& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink,
                                    wxString* output)
{
    if(!CanHandle(file_type)) {
        return false;
    }

    // Create a copy
    wxString cmd = GetCommandAsString();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    wxBusyCursor bc;
    if(sink) {
        thread_format(cmd, wd, filepath, IsInplaceFormatter(), sink);
    } else {
        sync_format(cmd, wd, IsInplaceFormatter(), output);
    }
    return true;
}

bool GenericFormatter::FormatFile(const wxFileName& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink)
{
    return FormatFile(filepath.GetFullPath(), file_type, sink);
}

bool GenericFormatter::FormatRemoteFile(const wxString& filepath, FileExtManager::FileType file_type,
                                        wxEvtHandler* sink)
{
    if(!CanHandle(file_type)) {
        return false;
    }

    if(!CanHandleRemoteFile()) {
        return false;
    }

    // Create a copy
    wxString cmd = GetRemoteCommand();

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    thread_format(cmd, wd, filepath, IsInplaceFormatter(), sink);
    return true;
}

bool GenericFormatter::FormatFile(const wxString& filepath, FileExtManager::FileType file_type, wxEvtHandler* sink)
{
    return DoFormatFile(filepath, file_type, sink, nullptr);
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

    if(!DoFormatFile(tmpfile.GetFullPath(), file_type, nullptr, output)) {
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