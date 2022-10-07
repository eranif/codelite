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
    wxString wsp_dir;
    bool is_remote = false;
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    if(workspace) {
        is_remote = workspace->IsRemote();
        wsp_dir = workspace->GetDir();
    }

    // replace file macros
    wxString filepath_relative;
    wxString fullname;
    wxString filedir;
    wxString fullpath = filepath;

    wxFileName fn{ filepath };
    if(!wsp_dir.empty()) {
        fn.MakeRelativeTo(wsp_dir);
    }

    filepath_relative = fn.GetFullPath(is_remote ? wxPATH_UNIX : wxPATH_NATIVE);
    fullname = fn.GetFullName();
    filedir = fn.GetPath(is_remote ? wxPATH_UNIX : wxPATH_NATIVE);

    EnvSetter env;
    wxString tmp_expr = expression;
    tmp_expr.Replace("$(CurrentFileName)", fn.GetName());
    tmp_expr.Replace("$(CurrentFilePath)", filedir);
    tmp_expr.Replace("$(CurrentFileExt)", fn.GetExt());
    tmp_expr.Replace("$(CurrentFileFullName)", fullname);
    tmp_expr.Replace("$(CurrentFileFullPath)", fullpath);
    tmp_expr.Replace("$(CurrentFileRelPath)", filepath_relative);

    wxString resolved = MacroManager::Instance()->Expand(tmp_expr, clGetManager(), wxEmptyString, wxEmptyString);
    return resolved;
}

wxArrayString command_from_string(const wxString& command)
{
    wxArrayString lines = ::wxStringTokenize(command, "\n", wxTOKEN_STRTOK);

    wxArrayString command_array;
    command_array.reserve(lines.size());

    for(auto& line : lines) {
        line.Trim().Trim(false);
        if(line.StartsWith("#") || line.IsEmpty()) {
            continue;
        }

        int count = 0;
        auto argv = StringUtils::BuildArgv(line, count);
        for(int i = 0; i < count; ++i) {
            command_array.push_back(argv[i]);
        }
        StringUtils::FreeArgv(argv, count);
    }
    return command_array;
}

wxString get_command_with_desc(const wxArrayString& command_arr, const wxString& desc)
{
    wxString command;
    command << "# " << desc << "\n";
    command << "# Command to execute:\n";
    command << "\n";

    for(size_t i = 0; i < command_arr.size(); ++i) {
        if(i > 0) {
            command << "  ";
        }
        command << command_arr[i] << "\n";
    }
    return command;
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

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    clDirChanger changer{ wd };
    *output = ::wxShellExec(cmd, wxEmptyString);

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

    cmd = replace_macros(cmd, filepath);
    wxString wd = replace_macros(GetWorkingDirectory(), filepath);

    clDEBUG() << "Working dir:" << wd << endl;
    clDEBUG() << "Calling:" << cmd << endl;

    clDirChanger changer{ wd };
    *output = ::wxShellExec(cmd, wxEmptyString);

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

bool GenericFormatter::GetSSHCommand(wxString* ssh_exe) const { return PLATFORM::Which("ssh", ssh_exe); }

void GenericFormatter::SetCommandFromString(const wxString& command) { SetCommand(command_from_string(command)); }

void GenericFormatter::SetRemoteCommandFromString(const wxString& command)
{
    SetRemoteCommand(command_from_string(command));
}

wxString GenericFormatter::GetRemoteCommandWithComments() const
{
    return get_command_with_desc(m_remote_command, m_description);
}

wxString GenericFormatter::GetCommandWithComments() const { return get_command_with_desc(m_command, m_description); }