#include "Tools.hpp"

#include "Diff/clPatchApplier.hpp"
#include "FileManager.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "Platform/Platform.hpp"
#include "ai/LLMManager.hpp"
#include "ai/ToolsUtils.hpp"
#include "assistant/function.hpp"
#include "clSFTPManager.hpp"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "procutils.h"
#include "ssh/ssh_account_info.h"

#include <wx/msgdlg.h>
#include <wx/string.h>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

namespace llm
{

using assistant::CanInvokeToolResult;
constexpr int kMaxLinesToRead = 1000;

CanInvokeToolResult FileSystemWriteConfirm(const std::string& tool_name, assistant::json args)
{
    CONFIRM_ARG(std::string,
                path,
                "",
                ::assistant::GetFunctionArg<std::string>(args, "path"),
                "Missing or empty mandatory field 'path'");

    auto action = ::assistant::GetFunctionArg<std::string>(args, "action");
    if (!action.has_value() || action->empty()) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Missing mandatory field 'action'",
        };
    }

    static constexpr const char kFileSystemWrite[] = "FileSystemWrite";
    if (action.value() == "create_dir") {
        return ConfirmPathTool(
            kFileSystemWrite, _("The model wants to create the directory:"), _("Creating new directory:"), path);
    }
    if (action.value() == "create_file") {
        return ConfirmPathTool(
            kFileSystemWrite, _("The model wants to create the file:"), _("Creating new file:"), path);
    }
    if (action.value() == "append_file") {
        return ConfirmPathTool(
            kFileSystemWrite, _("The model wants to append to the file:"), _("Appending file:"), path);
    }

    return CanInvokeToolResult{
        .can_invoke = false,
        .reason = "Invalid value for mandatory field 'action'. Expected one of: 'create_file', "
                  "'create_dir', 'append_file'",
    };
}

FunctionResult FileSystemWrite(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() < 2) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string path, ::assistant::GetFunctionArg<std::string>(args, "path"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string action, ::assistant::GetFunctionArg<std::string>(args, "action"));

    std::string file_content = ::assistant::GetFunctionArg<std::string>(args, "content").value_or("");

    auto fs_create_file = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(path));
        if (FileManager::FileExists(fullpath)) {
            wxString prompt_message;
            prompt_message << _("The file '") << fullpath << _("' already exists.\nOverride it?");
            if (::clMessageBox(prompt_message, "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_QUESTION) !=
                wxYES) {
                msg << _("The file: ") << fullpath << _(" already exists");
                return Err(msg);
            }
        }

        // Always write to disk (even empty content) so the file physically exists after this call.
        if (!FileManager::WriteContent(path, wxString::FromUTF8(file_content), true)) {
            msg << "Error while writing file: '" << path << "' to disk.";
            return Err(msg);
        }

        msg << "file '" << path << "' successfully written to disk!.";

        if (clFileSystemWorkspace::Get().IsOpen()) {
            clFileSystemWorkspace::Get().GetView()->RefreshTree();
            clFileSystemWorkspace::Get().FileSystemUpdated();
        }

        auto editor = clGetManager()->FindEditor(fullpath);
        if (editor) {
            editor->ReloadFromDisk();
        }
        return Ok(msg);
    };

    auto fs_append_file = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(path));
        if (!FileManager::FileExists(fullpath)) {
            msg << "The file: " << fullpath << " does not exist";
            return Err(msg);
        }

        wxString existing_content;
        {
            auto content = FileManager::ReadContent(fullpath);
            if (content.has_value()) {
                existing_content = content.value();
            }
        }

        if (!FileManager::WriteContent(path, existing_content + wxString::FromUTF8(file_content), true)) {
            msg << "Error while appending file: '" << path << "' to disk.";
            return Err(msg);
        }

        msg << "file '" << path << "' successfully appended to disk!.";
        if (clFileSystemWorkspace::Get().IsOpen()) {
            clFileSystemWorkspace::Get().GetView()->RefreshTree();
            clFileSystemWorkspace::Get().FileSystemUpdated();
        }

        auto editor = clGetManager()->FindEditor(fullpath);
        if (editor) {
            editor->ReloadFromDisk();
        }
        return Ok(msg);
    };

    auto fs_new_dir = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetDirectoryFullPath(wxString::FromUTF8(path));
        if (FileManager::DirExists(fullpath)) {
            return Ok(std::nullopt);
        }

        if (!FileManager::CreateDir(fullpath)) {
            msg << "Failed to create directory: " << fullpath
                << ". Make sure you have enough permissions and that the parent folder(s) exist";
            return Err(msg);
        }

        if (clFileSystemWorkspace::Get().IsOpen()) {
            clFileSystemWorkspace::Get().GetView()->RefreshTree();
            clFileSystemWorkspace::Get().FileSystemUpdated();
        }

        msg << "Successfully created directory '" << path << "'";
        return Ok(msg);
    };

    if (action == "create_dir") {
        return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(fs_new_dir));
    } else if (action == "create_file") {
        return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(fs_create_file));
    } else if (action == "append_file") {
        return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(fs_append_file));
    } else {
        return Err("Invalid value for 'action'. Expected one of: create_file, create_dir, append_file");
    }
}

/// Will be invoked by the library before calling to the "ReadFileContent"
CanInvokeToolResult ReadFileContentConfirm(const std::string& tool_name, assistant::json args)
{
    CONFIRM_ARG(std::string,
                filepath,
                "",
                ::assistant::GetFunctionArg<std::string>(args, "filepath"),
                "Missing mandatory field 'filepath'");

    auto line_count = ::assistant::GetFunctionArg<int>(args, "line_count");
    if (line_count.has_value() && line_count.value() > kMaxLinesToRead) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = wxString::Format("The line count must be between 1 and %d, inclusive.", kMaxLinesToRead)
                          .ToStdString(wxConvUTF8),
        };
    }

    static constexpr const char* kReadFileContent = "ReadFileContent";
    return ConfirmPathTool(kReadFileContent, _("The model wants to read the file:"), _("Reading file:"), filepath);
}

FunctionResult ReadFileContent(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() < 1) {
        return Err("Invalid number of arguments: 'filepath' is required");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));
    int from_line = ::assistant::GetFunctionArg<int>(args, "from_line").value_or(0);
    int line_count = ::assistant::GetFunctionArg<int>(args, "line_count").value_or(0);
    bool read_all = (from_line == 0 && line_count == 0);

    auto cb = [=]() -> FunctionResult {
        auto& llm = llm::Manager::GetInstance();
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(filepath));
        std::optional<wxString> content = FileManager::ReadContent(fullpath);
        if (!content.has_value()) {
            wxString logmsg;
            logmsg << "Failed to read file: " << filepath << ". ";
            llm.PrintMessage(logmsg, IconType::kError);
            return Err(logmsg);
        }

        // If both from_line and line_count are omitted, return the entire file
        if (read_all) {
            wxString logmsg;
            logmsg << "Successfully read file: " << filepath << ". " << content->size() << " bytes.";
            llm.PrintMessage(logmsg, IconType::kSuccess);
            return Ok(content.value());
        }

        // Validate provided parameters
        int effective_from_line = (from_line == 0) ? 1 : from_line;
        if (effective_from_line < 1) {
            return Err("'from_line' must be >= 1");
        }
        if (line_count != 0 && line_count < 1) {
            return Err("'line_count' must be >= 1");
        }

        // Split content into lines
        wxArrayString lines = wxStringTokenize(content.value(), "\n", wxTOKEN_RET_DELIMS);

        // Check if from_line is within bounds
        if (effective_from_line > (int)lines.size()) {
            return Err(
                wxString::Format("'from_line' (%d) exceeds total file lines (%zu)", effective_from_line, lines.size()));
        }

        // Extract the requested lines (from_line is 1-based)
        int start_idx = effective_from_line - 1;
        int end_idx = (line_count == 0) ? static_cast<int>(lines.size())
                                        : wxMin(start_idx + line_count, static_cast<int>(lines.size()));
        wxString partial_content;
        for (int i = start_idx; i < end_idx; ++i) {
            partial_content << lines[i];
        }

        wxString logmsg;
        logmsg << "Successfully read file: " << filepath << " (lines " << effective_from_line << "-" << (end_idx)
               << "). " << partial_content.size() << " bytes.";
        llm.PrintMessage(logmsg, IconType::kSuccess);
        return Ok(partial_content);
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

/// Will be invoked by the library before calling to the "ReadFileMetadata"
CanInvokeToolResult ReadFileMetadataConfirm(const std::string& tool_name, assistant::json args)
{
    CONFIRM_ARG(std::string,
                filepath,
                "",
                ::assistant::GetFunctionArg<std::string>(args, "filepath"),
                "Missing mandatory field 'filepath'");

    static constexpr const char* kReadFileMetadata = "ReadFileMetadata";
    return ConfirmPathTool(
        kReadFileMetadata, _("The model wants to read metadata for the file:"), _("Reading file metadata:"), filepath);
}

FunctionResult ReadFileMetadata(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() != 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        auto& llm = llm::Manager::GetInstance();
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(filepath));

        std::optional<wxString> content = FileManager::ReadContent(fullpath);
        if (!content.has_value()) {
            wxString logmsg;
            logmsg << "Failed to read file: " << filepath << ". ";
            llm.PrintMessage(logmsg, IconType::kError);
            return Err(logmsg);
        }

        // Split content into lines
        wxArrayString lines = wxStringTokenize(content.value(), "\n", wxTOKEN_RET_DELIMS);

        // Prepare a JSON metadata object.
        llm::json md = llm::json::object();
        md["fullpath"] = fullpath.ToStdString(wxConvUTF8);
        md["size"] = content.value().size();
        md["lines_count"] = lines.size();

        wxString logmsg;
        logmsg << "File Metadata:\n```json\n" << md.dump(1) << "\n```\n";
        llm.PrintMessage(logmsg, IconType::kSuccess);
        return Ok(wxString::FromUTF8(md.dump()));
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult OpenFileInEditor(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();

    if (args.size() != 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        wxString file = wxString::FromUTF8(filepath);
        wxString msg;
        IEditor* editor{nullptr};
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            clDEBUG() << "Open file (workspace):" << file << endl;
            editor = clWorkspaceManager::Get().GetWorkspace()->OpenFileInEditor(file, false);
        } else {
            clDEBUG() << "Open file:" << file << endl;
            editor = clGetManager()->OpenFile(file);
        }

        if (!editor) {
            msg << "Error occurred while loading file '" << file << "' into an editor.";
            return Err(msg);
        }
        msg << "File '" << file << "' has been successfully loaded into an editor.";
        return Ok(msg);
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult GetCompilerOutput([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    auto cb = [=]() -> FunctionResult { return Ok(clGetManager()->GetBuildOutput()); };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult GetCurrentEditorText([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();

    // Optional parameters: default to reading the entire editor content (up to kMaxLinesToRead lines from line 1).
    int from_line = ::assistant::GetFunctionArg<int>(args, "from_line").value_or(1);
    int line_count = ::assistant::GetFunctionArg<int>(args, "count").value_or(kMaxLinesToRead);

    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("No editor is currently open");
        }

        wxString full_text = active_editor->GetEditorText();

        // Validate parameters
        if (from_line < 1) {
            return Err("'from_line' must be >= 1");
        }
        if (line_count < 1) {
            return Err("'count' must be >= 1");
        }

        if (line_count > kMaxLinesToRead) {
            return Err(wxString::Format("The line count must be between 1 and %d, inclusive.", kMaxLinesToRead)
                           .ToStdString(wxConvUTF8));
        }

        // Split content into lines
        wxArrayString lines = wxStringTokenize(full_text, "\n", wxTOKEN_RET_DELIMS);

        // Check if from_line is within bounds
        if (from_line > static_cast<int>(lines.size())) {
            return Err(wxString::Format("'from_line' (%d) exceeds total editor lines (%zu)", from_line, lines.size()));
        }

        // Extract the requested lines (from_line is 1-based)
        int start_idx = from_line - 1;
        int end_idx = wxMin(start_idx + line_count, (int)lines.size());
        wxString partial_content;
        for (int i = start_idx; i < end_idx; ++i) {
            partial_content << lines[i];
        }
        return Ok(partial_content);
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult GetCurrentEditorPath([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("No editor is currently open");
        }
        return Ok(active_editor->GetRemotePathOrLocal());
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult CreateWorkspace([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    auto cb = [=]() -> FunctionResult {
        clDEBUG() << args.dump(2) << endl;
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            return Err("Please close the current workspace before creating a new one");
        }

        ASSIGN_FUNC_ARG_OR_RETURN(std::string dir, ::assistant::GetFunctionArg<std::string>(args, "path"));
        auto host = ::assistant::GetFunctionArg<std::string>(args, "host"); // Optional param
        auto name = ::assistant::GetFunctionArg<std::string>(args, "name").value_or("");

        wxString path = wxString::FromUTF8(dir);

        wxString workspace_name = wxString::FromUTF8(name);
        workspace_name = workspace_name.empty() ? wxFileName(path).GetName() : workspace_name;

        std::optional<SSHAccountInfo> account;
        if (host.has_value()) {
            wxString h = wxString::FromUTF8(host.value());
            auto accounts = SSHAccountInfo::Load([h](const SSHAccountInfo& account) -> bool {
                if (account.GetHost() == h) {
                    return true;
                }
                return false;
            });

            if (accounts.size() > 1) {
                return Err("Found multiple accounts that matches the input host");
            } else if (accounts.empty()) {
                return Err("Could not find an account that matches the input host");
            }

            account = accounts[0];
        }

        if (account.has_value()) {
#if USE_SFTP
            clDEBUG() << "Creating remote workspace:" << workspace_name << ". Host:" << account.value().GetAccountName()
                      << "Path:" << path << endl;
            clWorkspaceEvent create_event{wxEVT_WORKSPACE_CREATE_NEW};
            create_event.SetWorkspaceName(workspace_name);
            create_event.SetWorkspacePath(path);
            create_event.SetSshAccount(account.value().GetAccountName());
            create_event.SetIsRemote(true);
            if (EventNotifier::Get()->ProcessEvent(create_event)) {
                return Ok(wxEmptyString);
            }
            return Err("Remote workspace functionality is currently disabled. Please ensure the Remoty plugin is "
                       "properly loaded.");
#else
            return Err("Remote workspace is not supported by this build of CodeLite");
#endif
        } else {
            // local workspace
            wxFileName workspace_file{path, workspace_name};
            workspace_file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            workspace_file.SetExt("workspace");
            if (workspace_file.FileExists()) {
                return Err("A workspace already exists in the given path");
            }

            clFileSystemWorkspace::Get().New(path, workspace_name);
            return Ok(wxEmptyString);
        }
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult FindInFiles([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();

    // Mandatory fields
    ASSIGN_FUNC_ARG_OR_RETURN(std::string root_dir, ::assistant::GetFunctionArg<std::string>(args, "root_folder"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string find_what, ::assistant::GetFunctionArg<std::string>(args, "find_what"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string file_pattern, ::assistant::GetFunctionArg<std::string>(args, "file_pattern"));

    // Parse file patterns (semi-colon separated list)
    wxString file_patterns_str = wxString::FromUTF8(file_pattern);
    if (!FileUtils::ValidateFilePattern(file_patterns_str)) {
        return Err("Invalid file patterns. Pattern must be a semicolon-delimited list. Example: "
                   "`*.cpp;*.h;CMakeLists.txt;.bashrc`");
    }

    wxArrayString file_pattern_arr = wxSplit(file_patterns_str, ';', 0);
    if (file_pattern_arr.IsEmpty()) {
        return Err("No valid file patterns provided");
    }

    for (auto& pat : file_pattern_arr) {
        pat.Trim().Trim(false);
        if (pat == "*" || pat == "*.*") {
            return Err(
                "`file_pattern` can not be only `*` or `*.*`, it must be more specific. For example `*.cpp;*.h;*.rs`");
        }
    }

    bool recursive = assistant::GetFunctionArg<bool>(args, "recursive").value_or(false);
    bool whole_word = assistant::GetFunctionArg<bool>(args, "whole_word").value_or(true);
    bool case_sensitive = assistant::GetFunctionArg<bool>(args, "case_sensitive").value_or(true);
    bool is_regex = assistant::GetFunctionArg<bool>(args, "is_regex").value_or(false);
    int context_before = assistant::GetFunctionArg<int>(args, "context_lines_before").value_or(0);
    int context_after = assistant::GetFunctionArg<int>(args, "context_lines_after").value_or(0);

    // Build the grep command.
    auto grep_command = ThePlatform->Which("grep");
    if (!grep_command.has_value()) {
        return Err("Could not find grep tool!");
    }

    bool is_remote{false};
#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    is_remote = workspace && workspace->IsRemote();
    if (is_remote) {
        grep_command = "/usr/bin/grep";
    }
#endif

    // Build grep command
    wxString cmd;
    cmd << StringUtils::WrapWithDoubleQuotes(grep_command.value());

    // Add flags
    if (recursive) {
        cmd << " -r"; // Recursive search
    }
    cmd << " -n"; // Show line numbers

    if (!case_sensitive) {
        cmd << " -i"; // Case-insensitive
    }

    if (whole_word && !is_regex) {
        cmd << " -w"; // Match whole words
    }

    if (!is_regex) {
        cmd << " -F"; // Fixed string (literal, not regex)
    } else {
        cmd << " -E"; // Regex
    }

    // Add the search pattern (properly escaped)
    wxString search_pattern = FileUtils::NormalizePath(wxString::FromUTF8(find_what));
    cmd << " " << StringUtils::EscapeAndWrapWithDoubleQuotes(search_pattern);

    // Add the root folder
    wxString root_folder = FileUtils::NormalizePath(wxString::FromUTF8(root_dir));
    cmd << " " << StringUtils::EscapeAndWrapWithDoubleQuotes(root_folder);

    // Add include patterns for file types
    for (auto& pattern : file_pattern_arr) {
        if (!pattern.IsEmpty()) {
            cmd << " --include=" << StringUtils::EscapeAndWrapWithDoubleQuotes(pattern);
        }
    }

    // Execute the grep command
    wxArrayString output_arr;
    TerminationFlagGuard termination_flag;

    wxString message;
    message << "Will run the following command:\n```bash\n" << cmd << "\n```\n";
    llm::Manager::GetInstance().PrintMessage(message, IconType::kInfo);

#if USE_SFTP
    if (is_remote) {
        auto result = clSFTPManager::Get().AwaitExecute(workspace->GetSshAccount(), cmd, workspace->GetDir());
        std::string out = std::get<0>(result);
        std::string err = std::get<1>(result);

        if (out.empty() && !err.empty()) {
            return Err("grep error: ```bash\n" + err + "\n```");
        }
        if (out.empty()) {
            return Ok("No matches found");
        }
        // Parse remote output similarly to local
        wxArrayString remote_output_arr = wxStringTokenize(wxString::FromUTF8(out), wxT("\n"), wxTOKEN_RET_EMPTY);
        output_arr = remote_output_arr;
    }
#endif

    // Local command execution. When USE_SFTP is defined and is_remote is true the block above already
    // returned or populated output_arr, so this branch is intentionally skipped for remote workspaces.
    // When USE_SFTP is not defined, is_remote is always false and we always reach this branch.
    if (!is_remote) {
        EnvSetter env;
        ProcUtils::SafeExecuteCommand(cmd, output_arr, termination_flag.GetFlag());
    }

    if (termination_flag.IsSet()) {
        return Err("`grep` terminated by user");
    }

    if (output_arr.empty()) {
        return Ok("No matches found");
    }

    auto results = ParseGrepOutput(output_arr);
    constexpr size_t kMaxGrepResponses = 100;

    // Format the output.
    wxString string_result;
    size_t count{0};
    for (const auto& [file, matches] : results.matches) {
        string_result << file << ":\n";
        for (const auto& m : matches) {
            count++;
            if (count > kMaxGrepResponses) {
                break;
            }
            string_result << "  " << m.line_number << ":" << m.pattern << "\n";
        }

        if (count > kMaxGrepResponses) {
            wxString prefix_message;
            prefix_message << "IMPORTANT NOTE: displaying only " << kMaxGrepResponses
                           << " matches. The total number of matches exceeded the maximum allowed of: "
                           << kMaxGrepResponses << ". Please refine your search query\n\n";
            string_result.Prepend(prefix_message);
            break;
        }
    }

    wxString report_message;
    report_message << _("Found ") << results.count << _(" matches across ") << results.matches.size() << _(" files. ");
    llm::Manager::GetInstance().PrintMessage(report_message, IconType::kSuccess);
    if (results.count > kMaxGrepResponses) {
        report_message.clear();
        report_message << (results.count - kMaxGrepResponses) << _(" results were truncated");
        llm::Manager::GetInstance().PrintMessage(report_message, IconType::kInfo);
    }
    return Ok(string_result);
}

CanInvokeToolResult ApplyPatchConfirm(const std::string& tool_name, assistant::json args)
{
    if (wxIsMainThread()) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Internal Error.",
        };
    }

    if (args.size() < 2) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Invalid number of arguments",
        };
    }

    std::string file_path = ::assistant::GetFunctionArg<std::string>(args, "file_path").value_or("");
    std::string patch_content = ::assistant::GetFunctionArg<std::string>(args, "patch_content").value_or("");

    wxString patch = wxString::FromUTF8(patch_content);
    auto& config = llm::Manager::GetInstance().GetConfig();

    wxString fullpath = FileUtils::NormalizePath(wxString::FromUTF8(file_path));
    static constexpr const char* kApplyPatch = "ApplyPatch";

    bool is_trusted = llm::Manager::GetInstance().CheckIfPathIsAllowedForTool(kApplyPatch, fullpath);

    // Apply patch "Trust" for this session (per path).
    if (is_trusted) {
        // This path is trusted.
        wxString trust_message;
        trust_message << "`" << kApplyPatch << _("` is trusted for path: `") << fullpath << "`";
        llm::Manager::GetInstance().PrintMessage(trust_message, IconType::kInfo);

        wxString message;
        message << _("Will apply the following patch:") << "\n```diff\n" << patch << "\n```\n";
        llm::Manager::GetInstance().PrintMessage(message, IconType::kInfo);
        return CanInvokeToolResult{
            .can_invoke = true,
        };
    }

    wxString message;
    message << _("Will apply the following patch:") << "\n```diff\n" << patch << "\n```\n";
    return llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(message, [&config, &fullpath]() {
        // User trusts the tool for this path,add it to the trust store.
        wxString dirpath = FileUtils::GetPath(fullpath);

        std::vector<std::pair<wxString, wxString>> options{
            {wxString::Format(_("Specific path: %s"), fullpath), fullpath},
            {wxString::Format(_("Entire directory: %s/*"), dirpath), dirpath + "/*"},
            {_("Entire tool"), "*"},
        };

        auto result = llm::Manager::GetInstance().ShowTrustLevelDialog(kApplyPatch, options);
        if (result.has_value()) {
            config.AddTrustedTool(kApplyPatch, result->first, result->second);
            if (result->second) {
                config.Save(false);
            }
        }
    });
}

FunctionResult ApplyPatch([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() < 2) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(
        std::string patch_content, ::assistant::GetFunctionArg<std::string>(args, "patch_content"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string file_path, ::assistant::GetFunctionArg<std::string>(args, "file_path"));
    wxString patch = wxString::FromUTF8(patch_content);

    // ApplyPatchLoose must be called from the main thread only (it manipulates GUI).
    return EventNotifier::Get()->RunOnMain<FunctionResult>([file_path, patch]() -> FunctionResult {
        wxString file = wxString::FromUTF8(file_path);
        auto result = PatchApplier::ApplyPatchLoose(file, patch, true);
        if (!result.success) {
            // revert any changes done to the file.
            auto editor = clGetManager()->FindEditor(file);
            if (editor) {
                editor->ReloadFromDisk();
            }
            return Err(result.errorMessage.ToStdString(wxConvUTF8));
        }
        return Ok("Patch applied successfully");
    });
}

CanInvokeToolResult ToolShellExecuteConfirm(const std::string& tool_name, const assistant::json& args)
{
    if (wxIsMainThread()) {
        return CanInvokeToolResult{.can_invoke = false, .reason = "Internal Error."};
    }

    if (args.size() < 2) {
        return CanInvokeToolResult{.can_invoke = false, .reason = "Invalid number of arguments"};
    }

    auto command = ::assistant::GetFunctionArg<std::string>(args, "command");
    if (!command.has_value()) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Missing mandatory field 'command'",
        };
    }

    auto working_directory = ::assistant::GetFunctionArg<std::string>(args, "working_directory");
    if (!working_directory.has_value()) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Missing mandatory field 'working_directory'",
        };
    }

    // Check if this command was already in the store.
    static constexpr const char* kShellExecute = "ShellExecute";
    wxString command_string = wxString::FromUTF8(command.value());
    command_string.Trim().Trim(false);

    if (command_string.empty()) {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Missing or empty mandatory field 'command'",
        };
    }

    wxString working_dir = wxString::FromUTF8(working_directory.value());
    bool is_trusted =
        llm::Manager::GetInstance().CheckIfShellCommandAllowed(kShellExecute, command_string, working_dir) ||
        llm::Manager::GetInstance().CheckIfPathIsAllowedForTool(
            kShellExecute, command_string, llm::PathMatch::kExactMatchOnly);

    if (is_trusted) {
        wxString trust_message;
        trust_message << "`" << kShellExecute << _("` is trusted for the command: `") << command_string << "`";
        llm::Manager::GetInstance().PrintMessage(trust_message, IconType::kInfo);
        return CanInvokeToolResult{
            .can_invoke = true,
            .reason = "Command is trusted.",
        };
    }

    wxString message;
    message << _("The model wants to run the following shell command:\n```bash\n") << _("# Working directory\n")
            << working_dir << "\n\n"
            << _("# Command\n") << command_string << "\n```\n";

    return llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(message, [command_string]() {
        auto commands_array = StringUtils::SplitShellCommand(command_string);

        // Build the "Trust" options.
        std::vector<std::pair<wxString, wxString>> options = {
            {wxString::Format(_("Exact command: %s"), command_string), command_string}};
        if (!commands_array.empty()) {
            wxString command_list = "[";

            std::unordered_set<wxString> unique_commands;
            for (const auto& commands : commands_array) {
                if (commands.empty())
                    continue;

                if (!unique_commands.insert(commands[0]).second)
                    continue;

                command_list << commands[0] << " *;";
            }

            if (command_list.size() > 1) {
                command_list.RemoveLast();
            }
            command_list << "]";
            options.push_back(std::make_pair(wxString::Format(_("Tools: %s"), command_list), command_list));
        }
        options.push_back(std::make_pair(_("Entire tool"), "*"));

        auto& config = llm::Manager::GetInstance().GetConfig();
        auto result = llm::Manager::GetInstance().ShowTrustLevelDialog(kShellExecute, options);
        if (result.has_value()) {
            wxArrayString commands;
            if (result->first.StartsWith("[") && result->first.EndsWith("]")) {
                wxString trusted_string = result->first.Mid(1, result->first.size() - 2);
                commands = wxStringTokenize(trusted_string, ";", wxTOKEN_STRTOK);
            } else {
                // Keep the exact string.
                commands.Add(result->first);
            }

            for (const wxString& trusted_pattern : commands) {
                config.AddTrustedTool(kShellExecute, trusted_pattern, result->second);
            }

            if (result->second) {
                config.Save(false);
            }
        }
    });
}

FunctionResult ToolShellExecute(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    ASSIGN_FUNC_ARG_OR_RETURN(
        std::string working_directory, ::assistant::GetFunctionArg<std::string>(args, "working_directory"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string command, ::assistant::GetFunctionArg<std::string>(args, "command"));
    wxString cmd = wxString::FromUTF8(command);
    wxString wd = wxString::FromUTF8(working_directory);
    FunctionResult func_result{.isError = false};

    auto commands = StringUtils::SplitShellCommand(cmd);
    if (commands.empty() || commands[0].empty()) {
        return FunctionResult{
            .isError = true,
            .text = "Missing mandatory command",
        };
    }

    wxString main_command = commands[0][0].Lower();
    bool apply_output_check_len = (main_command == "ls" || main_command == "dir" || main_command == "grep" ||
                                   main_command == "find" || main_command == "cat");
#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    bool is_remote = workspace && workspace->IsRemote();
    if (is_remote) {
        auto result = clSFTPManager::Get().AwaitExecute(workspace->GetSshAccount(), cmd, wd);
        wxString out = wxString::FromUTF8(std::get<0>(result));
        wxString err = wxString::FromUTF8(std::get<1>(result));

        wxString combined_output = out + "\n" + err;
        auto lines = wxStringTokenize(combined_output, "\n", wxTOKEN_STRTOK);
        if (apply_output_check_len && lines.size() > kMaxLinesToRead) {
            wxString message;
            message << _("The output for the command: `") << cmd << _("` is too large.");
            llm::Manager::GetInstance().PrintMessage(message, IconType::kError);
            return FunctionResult{
                .isError = true,
                .text = "The tool output is too large. Please refine your command.",
            };
        }
        func_result.isError = false;
        func_result.text = combined_output.ToStdString(wxConvUTF8);
        llm::Manager::GetInstance().PrintMessage(
            _("Command output:\n```bash\n") + func_result.text + "\n```\n", IconType::kInfo);
        return func_result;
    }
#endif

    // Local command.
    std::string command_output;
    TerminationFlagGuard termination_flag;
    wxArrayString output_arr;
    EnvSetter env;
    ProcUtils::SafeExecuteShellCommand(cmd, wd, output_arr, termination_flag.GetFlag());
    if (termination_flag.IsSet()) {
        return FunctionResult{
            .isError = true,
            .text = "Command terminated by user",
        };
    }

    size_t lines_count = output_arr.size();
    if (apply_output_check_len && lines_count > kMaxLinesToRead) {
        wxString message;
        message << _("The output for the command: `") << cmd << _("` is too large.");
        llm::Manager::GetInstance().PrintMessage(message, IconType::kError);
        return FunctionResult{
            .isError = true,
            .text = "The tool output is too large. Please refine your command.",
        };
    }

    wxString output = StringUtils::Join(output_arr);
    func_result.isError = false;
    func_result.text = output.ToStdString(wxConvUTF8);

    llm::Manager::GetInstance().PrintMessage(
        _("Command Output:\n```bash\n") + func_result.text + "\n```\n", IconType::kInfo);
    return func_result;
}

FunctionResult GetOS([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();

    // Check if a remote workspace is opened - if so, always return "Linux"
    auto is_remote_workspace_cb = [=]() -> bool {
        auto workspace = clWorkspaceManager::Get().GetWorkspace();
        return workspace && workspace->IsRemote();
    };

    auto is_remote = EventNotifier::Get()->RunOnMain<bool>(std::move(is_remote_workspace_cb));
    if (is_remote) {
        // Remote workspaces are always accessed over SSH, which means the target host runs Linux.
        return Ok("Linux");
    }

    wxString os_name;
#if defined(__WXMSW__)
    os_name = "Windows";
#elif defined(__WXMAC__)
    os_name = "macOS";
#elif defined(__WXGTK__)
    os_name = "Linux";
#else
    os_name = "Unknown";
#endif

    return Ok(os_name);
}

FunctionResult GetWorkingDirectory([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    auto workspace_path_cb = []() -> std::optional<wxString> {
        auto workspace = clWorkspaceManager::Get().GetWorkspace();
        if (workspace == nullptr) {
            return std::nullopt;
        }
        return workspace->GetDir();
    };

    auto workspace_path = EventNotifier::Get()->RunOnMain<std::optional<wxString>>(std::move(workspace_path_cb));
    return Ok(workspace_path.value_or(wxGetCwd()));
}

// Register CodeLite tools with the model.
void PopulateBuiltInFunctions(FunctionTable& table)
{
    table.Add(
        FunctionBuilder("ReadFileContent")
            .SetDescription(
                wxString::Format("Retrieves a block of text from a file. Requires a filepath. "
                                 "Optionally accepts from_line (1-based starting line, defaults to 1) "
                                 "and line_count (number of lines to read, must be between 1 and %d; "
                                 "omit to read until end of file). "
                                 "If both from_line and line_count are omitted, the entire file is returned.",
                                 kMaxLinesToRead)
                    .ToStdString(wxConvUTF8))
            .AddRequiredParam("filepath", "The path of the file to read.", "string")
            .AddOptionalParam("from_line",
                              "The starting line number to read from (1-based indexing). Defaults to 1 when omitted.",
                              "number")
            .AddOptionalParam(
                "line_count",
                wxString::Format("The number of lines to read from the starting line. Must be between 1 and %d. "
                                 "Omit to read from from_line until the end of the file.",
                                 kMaxLinesToRead)
                    .ToStdString(wxConvUTF8),
                "number")
            // Force the limit using schema validation parameter
            .AddMinMaxValidation("line_count", 1, kMaxLinesToRead)
            .SetCallback(ReadFileContent)
            .SetHumanInTheLoopCallback(ReadFileContentConfirm)
            .Build());

    table.Add(FunctionBuilder("ReadFileMetadata")
                  .SetDescription("Reads metadata (path, size, and line count) of the file 'filepath' from the disk.")
                  .AddRequiredParam("filepath", "The path of the file to read metadata for.", "string")
                  .SetCallback(ReadFileMetadata)
                  .SetHumanInTheLoopCallback(ReadFileMetadataConfirm)
                  .Build());

    table.Add(FunctionBuilder("OpenFileInEditor")
                  .SetDescription("Try to open file 'filepath' and load it into the "
                                  "editor for editing or viewing.")
                  .AddRequiredParam("filepath", "The path of the file to open inside the editor.", "string")
                  .SetCallback(OpenFileInEditor)
                  .Build());

    table.Add(FunctionBuilder("GetActiveEditorFilePath")
                  .SetDescription(R"#(Retrieves the file path of the currently active editor)#")
                  .SetCallback(GetCurrentEditorPath)
                  .Build());

    table.Add(FunctionBuilder("ReadCompilerOutput")
                  .SetDescription("Read and fetches the compiler build log output of "
                                  "the most recent build command executed by "
                                  "the user and return it to the caller. Use this "
                                  "method to read the compiler output. This is "
                                  "useful for helping explaining and resolving build "
                                  "issues. On success read, this function return "
                                  "the complete build log output.")
                  .SetCallback(GetCompilerOutput)
                  .Build());

    table.Add(
        FunctionBuilder("GetActiveEditorText")
            .SetDescription("Return the text of the active tab inside the editor.")
            .AddRequiredParam("from_line", "Starting line to read from (1-based). Defaults to 1.", "number")
            .AddRequiredParam(
                "count",
                wxString::Format("Number of lines to read. Must be between 1 and %d. Defaults to %d.", kMaxLinesToRead)
                    .ToStdString(wxConvUTF8),
                "number")
            .AddMinMaxValidation("count", 1, kMaxLinesToRead)
            .SetCallback(GetCurrentEditorText)
            .Build());
    table.Add(
        FunctionBuilder("CreateWorkspace")
            .SetDescription(" This function attempts to create a new workspace at the given path with the "
                            "provided name. If a host is specified, it creates a remote workspace using SSH/SFTP; "
                            "otherwise, it creates a local filesystem workspace")
            .SetCallback(CreateWorkspace)
            .AddRequiredParam("path", " The directory path where the workspace should be created", "string")
            .AddOptionalParam("name", "The name of the workspace to create", "string")
            .AddOptionalParam("host", "The SSH host for creating a remote workspace", "string")
            .Build());
    table.Add(
        FunctionBuilder("FindInFiles")
            .SetDescription(R"(Search for a given pattern within files in a directory)")
            .SetCallback([](const assistant::json& args) {
                auto result = FindInFiles(args);
                if (result.isError) {
                    llm::Manager::GetInstance().PrintMessage(result.text, IconType::kError);
                }
                return result;
            })
            .AddRequiredParam("root_folder", "The root directory where the search begins", "string")
            .AddRequiredParam("recursive", "Recurse into subdirectories,default is false", "boolean")
            .AddRequiredParam("find_what", "The text pattern to search for", "string")
            .AddRequiredParam("file_pattern",
                              "The file pattern to match, such as \"*.txt\" or \"*.py\". Use semi-colon list to "
                              "pass multiple patterns.",
                              "string")
            .AddOptionalParam("whole_word", "When enabled, matches only complete words. Default is true.", "boolean")
            .AddOptionalParam(
                "case_sensitive", "When enabled, performs case-sensitive matching. Default is true.", "boolean")
            .AddOptionalParam("is_regex",
                              "When enabled, treats find_what as a regular expression pattern. Default is false.",
                              "boolean")
            .Build());
    table.Add(FunctionBuilder("ApplyPatch")
                  .SetDescription(R"(Apply a git style diff patch to a file.
IMPORTANT: Patches fail when the original lines don't exactly match the current file content.
To ensure accuracy:
1. ALWAYS read the target file first using ReadFileContent tool before creating a patch
2. Verify the exact current content, including whitespace, indentation, and line endings
3. Include at least 3-5 lines of unchanged context before and after the modifications
4. Ensure the "---" lines in your patch match the current file state character-for-character
5. Use sufficient context to make the location unique within the file
ALWAYS RESPOND WITH A GIT-STYLE DIFF THAT CAN BE APPLIED DIRECTLY. NEVER PROVIDE PLAIN EXPLANATIONS ALONE; IF YOU NEED TO EXPLAIN, APPEND A BRIEF NOTE AFTER THE DIFF.)")
                  .AddRequiredParam("patch_content", "The git style diff patch content to apply", "string")
                  .AddRequiredParam("file_path", "The path to the file that should be patched", "string")
                  .SetCallback([](const assistant::json& args) {
                      auto result = ApplyPatch(args);
                      if (result.isError) {
                          llm::Manager::GetInstance().PrintMessage(result.text, IconType::kError);
                      }
                      return result;
                  })
                  .SetHumanInTheLoopCallback(ApplyPatchConfirm)
                  .Build());
    table.Add(
        FunctionBuilder("FileSystemWrite")
            .SetDescription(
                R"(Create a new file, append to an existing file, or create a directory at the specified path with optional content)")
            .SetCallback([](const assistant::json& args) {
                auto result = FileSystemWrite(args);
                if (result.isError) {
                    llm::Manager::GetInstance().PrintMessage(result.text, IconType::kError);
                }
                return result;
            })
            .SetHumanInTheLoopCallback(FileSystemWriteConfirm)
            .AddRequiredParam("path", "The path where the new file should be created", "string")
            .AddRequiredParam("action", "The action to perform: create_file, create_dir, append_file", "string")
            .AddStringEnumValidation("action", {"create_file", "create_dir", "append_file"})
            .AddOptionalParam("content", "The content to write or append to the file", "string")
            .Build());
    table.Add(FunctionBuilder("ShellExecute")
                  .SetDescription(R"(Execute a shell command and return its output.
IMPORTANT: Before using this tool, you should call the GetOS tool first to determine the host operating system.
This ensures you can construct OS-appropriate commands (e.g., 'dir' vs 'ls', backslash vs forward slash paths).)")
                  .SetCallback(ToolShellExecute)
                  .SetHumanInTheLoopCallback(ToolShellExecuteConfirm)
                  .AddRequiredParam("command", "The shell command to execute", "string")
                  .AddRequiredParam("working_directory", "The directory where the command should be executed", "string")
                  .Build());
    table.Add(
        FunctionBuilder("GetWorkingDirectory")
            .SetDescription(
                R"(Return the current working directory. If a workspace is open, returns the workspace directory; otherwise returns the process current directory.)")
            .SetCallback(GetWorkingDirectory)
            .Build());
    table.Add(
        FunctionBuilder("GetOS")
            .SetDescription(
                R"(Return the current active OS. The purpose of this tool is to suggest the LLM on which host the MCP server is running so it can
adjust the commands it runs)")
            .SetCallback(GetOS)
            .Build());
}

} // namespace llm
