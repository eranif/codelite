#include "Tools.hpp"

#include "Diff/clPatchApplier.hpp"
#include "FileManager.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "TextViewerDlg.h"
#include "ai/LLMManager.hpp"
#include "assistant/function.hpp"
#include "clFilesFinder.h"
#include "clSFTPManager.hpp"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
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
// Register CodeLite tools with the model.
void PopulateBuiltInFunctions(FunctionTable& table)
{
    table.Add(FunctionBuilder("ReadFileContent")
                  .SetDescription("Reads the content of the file 'filepath' from the disk. "
                                  "By default, returns the entire file's content. "
                                  "If 'from_line' and 'line_count' are provided, reads only the specified "
                                  "range of lines (1-based line numbering). "
                                  "Both optional parameters must be provided together or omitted entirely. "
                                  "On success, this function returns the requested file content.")
                  .AddRequiredParam("filepath", "The path of the file to read.", "string")
                  .AddOptionalParam("from_line", "Starting line number (1-based) to read from", "number")
                  .AddOptionalParam("line_count", "Number of lines to read", "number")
                  .SetCallback(ReadFileContent)
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

    table.Add(FunctionBuilder("GetActiveEditorText")
                  .SetDescription("Return the text of the active tab inside the editor.")
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
            .SetCallback(FindInFiles)
            .AddRequiredParam("root_folder", "The root directory where the search begins", "string")
            .AddRequiredParam("find_what", "The text pattern to search fore", "string")
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
5. Use sufficient context to make the location unique within the file)")
                  .SetCallback(ApplyPatch)
                  .AddRequiredParam("patch_content", "The git style diff patch content to apply", "string")
                  .AddRequiredParam("file_path", "The path to the file that should be patched", "string")
                  .Build());
    table.Add(FunctionBuilder("CreateNewFile")
                  .SetDescription(R"(Create a new file at the specified path with optional content)")
                  .SetCallback(CreateNewFile)
                  .AddRequiredParam("filepath", "The path where the new file should be created", "string")
                  .AddOptionalParam("file_content", "The initial content to write to the file", "string")
                  .Build());
    table.Add(FunctionBuilder("ShellExecute")
                  .SetDescription(R"(Execute a shell command and return its output.
IMPORTANT: Before using this tool, you should call the GetOS tool first to determine the host operating system.
This ensures you can construct OS-appropriate commands (e.g., 'dir' vs 'ls', backslash vs forward slash paths).)")
                  .SetCallback(ToolShellExecute)
                  .AddRequiredParam("command", "The shell command to execute", "string")
                  .Build());
    table.Add(
        FunctionBuilder("GetOS")
            .SetDescription(
                R"(Return the current active OS. The purpose of this tool is to suggest the LLM on which host the MCP server is running so it can
adjust the commands it runs)")
            .SetCallback(GetOS)
            .Build());
}

/// Implementation details

#define VERIFY_WORKER_THREAD()                                                                   \
    if (wxIsMainThread()) {                                                                      \
        wxString message;                                                                        \
        message << _("Tool ") << __PRETTY_FUNCTION__ << _(" can not be run on the main thread"); \
        return Err(message);                                                                     \
    }

FunctionResult CreateNewFile(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() != 2) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));
    std::string file_content = ::assistant::GetFunctionArg<std::string>(args, "file_content").value_or("");

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(filepath));
        if (FileManager::FileExists(fullpath)) {
            msg << _("The file: ") << fullpath << _(" already exists");
            return Err(msg);
        }

        if (!file_content.empty() && !FileManager::WriteContent(filepath, wxString::FromUTF8(file_content), false)) {
            msg << "Error while writing file: '" << filepath << "' to disk.";
            return Err(msg);
        }

        msg << "file '" << filepath << "' successfully written to disk!.";

        if (clFileSystemWorkspace::Get().IsOpen()) {
            // refresh the entire view
            clFileSystemWorkspace::Get().GetView()->RefreshTree();
            // notify update
            clFileSystemWorkspace::Get().FileSystemUpdated();
        }
        return Ok(msg);
    };
    return EventNotifier::Get()->RunOnMain<FunctionResult>(std::move(cb));
}

FunctionResult ReadFileContent(const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() < 1 || args.size() > 3) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));

    // Check for optional parameters
    auto from_line_opt = ::assistant::GetFunctionArg<int>(args, "from_line");
    auto line_count_opt = ::assistant::GetFunctionArg<int>(args, "line_count");

    // Validate that both optional params are provided or none of them
    if (from_line_opt.has_value() != line_count_opt.has_value()) {
        return Err("Both 'from_line' and 'line_count' must be provided together, or none of them");
    }

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

        // If partial read is requested
        if (from_line_opt.has_value() && line_count_opt.has_value()) {
            int from_line = from_line_opt.value();
            int line_count = line_count_opt.value();

            // Validate parameters
            if (from_line < 1) {
                return Err("'from_line' must be >= 1");
            }
            if (line_count < 1) {
                return Err("'line_count' must be >= 1");
            }

            // Split content into lines
            wxArrayString lines = wxStringTokenize(content.value(), "\n", wxTOKEN_RET_EMPTY_ALL);

            // Check if from_line is within bounds
            if (from_line > (int)lines.size()) {
                return Err(
                    wxString::Format("'from_line' (%d) exceeds total file lines (%zu)", from_line, lines.size()));
            }

            // Extract the requested lines (from_line is 1-based)
            int start_idx = from_line - 1;
            int end_idx = wxMin(start_idx + line_count, (int)lines.size());
            wxString partial_content;
            for (int i = start_idx; i < end_idx; ++i) {
                partial_content << lines[i] << "\n";
            }

            wxString logmsg;
            logmsg << "Successfully read file: " << filepath << " (lines " << from_line << "-" << (end_idx) << "). "
                   << partial_content.size() << " bytes.";
            llm.PrintMessage(logmsg, IconType::kSuccess);
            return Ok(partial_content);
        }

        wxString logmsg;
        logmsg << "Successfully read file: " << filepath << ". " << content.value().size() << " bytes.";
        llm.PrintMessage(logmsg, IconType::kSuccess);
        return Ok(content.value());
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
    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("No editor is currently open");
        }
        return Ok(active_editor->GetEditorText());
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

    if (args.size() < 3) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string root_dir, ::assistant::GetFunctionArg<std::string>(args, "root_folder"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string find_what, ::assistant::GetFunctionArg<std::string>(args, "find_what"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string file_pattern, ::assistant::GetFunctionArg<std::string>(args, "file_pattern"));

    if (file_pattern == "*") {
        return Err("'file_pattern' can not be only '*', it must be more specific. For example '*.cpp;*.h;*.rs'");
    }

    bool whole_word = assistant::GetFunctionArg<bool>(args, "whole_word").value_or(true);
    bool case_sensitive = assistant::GetFunctionArg<bool>(args, "case_sensitive").value_or(true);
    bool is_regex = assistant::GetFunctionArg<bool>(args, "is_regex").value_or(false);

    clFilesFinder finder;
    auto matches = finder.Search(root_dir,
                                 clFilesFinderOptions{
                                     .find_what = find_what,
                                     .is_case_sensitive = case_sensitive,
                                     .is_whole_match = whole_word,
                                     .is_regex = is_regex,
                                     .file_spec = file_pattern,
                                 });
    OrderedJSON j = OrderedJSON::array();
    for (const auto& match : matches) {
        j.push_back(match.ToJson());
    }
    return Ok(wxString::FromUTF8(j.dump()));
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
    clDEBUG() << "Applying the patch:\n" << patch << endl;

    auto response = llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(
        _("The model wants to apply the following patch, allow it?"), patch, "patch");

    if (!response.ok()) {
        clDEBUG() << "Permission to apply the patch declined." << response.error_message() << endl;
        return Err("Permission denied");
    }

    switch (response.value()) {
    case llm::UserAnswer::kNo:
        return Err("Permission denied");
    case llm::UserAnswer::kTrust:
    case llm::UserAnswer::kYes:
        break;
    }

    // ApplyPatchLoose must be called from the main thread only (it manipulates GUI).
    return EventNotifier::Get()->RunOnMain<FunctionResult>([file_path, patch]() -> FunctionResult {
        auto result = PatchApplier::ApplyPatchLoose(wxString::FromUTF8(file_path), patch, true);
        if (!result.success) {
            clDEBUG() << "Failed to apply the patch:" << result.errorMessage.ToStdString(wxConvUTF8) << endl;
            return Err(result.errorMessage.ToStdString(wxConvUTF8));
        }
        clDEBUG() << "Patch applied successfully" << endl;
        return Ok("Patch applied successfully");
    });
}

FunctionResult ToolShellExecute([[maybe_unused]] const assistant::json& args)
{
    VERIFY_WORKER_THREAD();
    if (args.size() < 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string command, ::assistant::GetFunctionArg<std::string>(args, "command"));
    wxString cmd = wxString::FromUTF8(command);
    auto result = llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(
        _("The model wants to run the following shell command, allow it?"), cmd, "bash");

    if (!result.ok()) {
        return Err(result.error_message());
    }

    switch (result.value()) {
    case llm::UserAnswer::kNo:
        return Err("Permission denied");
    case llm::UserAnswer::kYes:
    case llm::UserAnswer::kTrust:
        break;
    }

    std::string command_output;
    FunctionResult func_result{.isError = false};
    TerminationFlagGuard termination_flag;
#if USE_SFTP
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
    bool is_remote = workspace && workspace->IsRemote();
    if (is_remote) {
        auto result = clSFTPManager::Get().AwaitExecute(workspace->GetSshAccount(), cmd, workspace->GetDir());
        std::string out = std::get<0>(result);
        std::string err = std::get<1>(result);
        func_result.isError = false;
        func_result.text = out;
        if (!err.empty()) {
            func_result.text += "\n" + err;
        }

    } else {
        // Local command.
        ProcUtils::WrapInShell(cmd);
        wxArrayString output_arr;
        EnvSetter env;
        ProcUtils::SafeExecuteCommand(cmd, output_arr, termination_flag.GetFlag());
        if (termination_flag.IsSet()) {
            return FunctionResult{.isError = true, .text = "Command terminated by user"};
        }
        wxString output = StringUtils::Join(output_arr);
        command_output = output.ToStdString(wxConvUTF8);
        func_result.isError = false;
        func_result.text = command_output;
    }
#else
    // Local command.
    ProcUtils::WrapInShell(cmd);
    wxArrayString output_arr;
    EnvSetter env;
    ProcUtils::SafeExecuteCommand(cmd, output_arr, termination_flag.GetFlag());
    llm::Manager::GetInstance().DeleteTerminationFlag(termination_flag.GetFlag());
    if (termination_flag.IsSet()) {
        return FunctionResult{.isError = true, .text = "Command terminated by user"};
    }

    wxString output = StringUtils::Join(output_arr);
    command_output = output.ToStdString(wxConvUTF8);
    func_result.isError = false;
    func_result.text = command_output;
#endif

    llm::Manager::GetInstance().PrintMessage(
        _("Successfully executed the command. Output:\n```bash\n") + func_result.text + "\n```\n", IconType::kInfo);
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
        return Ok("Linux"); // Remote workspace detected, return "Linux"
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

} // namespace llm
