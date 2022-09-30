#include "GenericFormatter.hpp"

#include "StringUtils.h"
#include "asyncprocess.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clTempFile.hpp"
#include "clWorkspaceManager.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macromanager.h"
#include "workspace.h"

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
} // namespace

GenericFormatter::GenericFormatter() { SetWorkingDirectory("$(WorkspacePath)"); }

GenericFormatter::~GenericFormatter() {}

wxString GenericFormatter::GetRemoteCommandAsString() const { return join_array(m_remote_command); }
wxString GenericFormatter::GetCommandAsString() const { return join_array(m_command); }

bool GenericFormatter::FormatFile(const wxFileName& filepath, FileExtManager::FileType file_type,
                                  wxString* output) const
{
    return FormatFile(filepath.GetFullPath(), file_type, output);
}

bool GenericFormatter::FormatRemoteFile(const wxString& filepath, FileExtManager::FileType file_type,
                                        wxString* output) const
{
    if(!CanHandle(file_type)) {
        return false;
    }

    if(!CanHandleRemoteFile()) {
        return false;
    }

    // Create a copy
    wxString cmd = GetRemoteCommandAsString();

    wxString file = filepath;
    cmd.Replace("$(CurrentFileFullPath)", WrapWithQuotes(file));

    // apply the environment
    EnvSetter env;
    cmd = MacroManager::Instance()->Expand(cmd, clGetManager(), wxEmptyString, wxEmptyString);
    wxString wd = MacroManager::Instance()->Expand(GetWorkingDirectory(), clGetManager(), wxEmptyString, wxEmptyString);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;
    IProcess::Ptr_t p(::CreateSyncProcess(cmd, IProcessCreateDefault, wd));
    if(!p) {
        clWARNING() << "failed to start formatter process:" << cmd << endl;
        return false;
    }

    p->WaitForTerminate(*output);
    clDEBUG1() << "Formatter output (remote):" << endl;
    clDEBUG1() << *output << endl;

    if(IsInlineFormatter()) {
        // the formatted output is not written to stdout, so clear it
        output->clear();
    }
    return true;
}

bool GenericFormatter::FormatFile(const wxString& filepath, FileExtManager::FileType file_type, wxString* output) const
{
    if(!CanHandle(file_type)) {
        return false;
    }

    // Create a copy
    wxString cmd = GetCommandAsString();

    wxString file = filepath;
    cmd.Replace("$(CurrentFileFullPath)", WrapWithQuotes(file));

    // apply the environment
    EnvSetter env;
    cmd = MacroManager::Instance()->Expand(cmd, clGetManager(), wxEmptyString, wxEmptyString);
    wxString wd = MacroManager::Instance()->Expand(GetWorkingDirectory(), clGetManager(), wxEmptyString, wxEmptyString);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;
    IProcess::Ptr_t p(::CreateSyncProcess(cmd, IProcessCreateDefault, wd));
    if(!p) {
        clWARNING() << "failed to start formatter process:" << cmd << endl;
        return false;
    }

    p->WaitForTerminate(*output);
    clDEBUG1() << "Formatter output:" << endl;
    clDEBUG1() << *output << endl;

    if(IsInlineFormatter()) {
        // the formatted output is not written to stdout, so clear it
        output->clear();
    }
    return true;
}

bool GenericFormatter::FormatString(const wxString& content, const wxString& fullpath, wxString* output) const
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

    if(!FormatFile(tmpfile.GetFullPath(), file_type, output)) {
        return false;
    }

    if(IsInlineFormatter()) {
        // read the content of the temp file and return it
        output->clear();
        return FileUtils::ReadFileContent(tmpfile.GetFullPath(), *output);
    }
    return true;
}

void GenericFormatter::FromJSON(const JSONItem& json)
{
    SourceFormatterBase::FromJSON(json);
    m_command = json["command"].toArrayString();
    m_remote_command = json["remote_command"].toArrayString();
    m_workingDirectory = json["working_directory"].toString();
}

JSONItem GenericFormatter::ToJSON() const
{
    auto json = SourceFormatterBase::ToJSON();
    json.addProperty("command", m_command);
    json.addProperty("remote_command", m_remote_command);
    json.addProperty("working_directory", m_workingDirectory);
    return json;
}

void GenericFormatter::SetCommand(const std::vector<wxString>& command) { SetCommand(to_wx_array(command)); }

void GenericFormatter::SetRemoteCommand(const std::vector<wxString>& command)
{
    SetRemoteCommand(to_wx_array(command));
}
