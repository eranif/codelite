#include "Tools.hpp"

#include "FileManager.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "ai/LLMManager.hpp"
#include "assistant/function.hpp"
#include "clFilesCollector.h"
#include "clFilesFinder.h"
#include "clPatchApplier.hpp"
#include "clWorkspaceManager.h"
#include "globals.h"
#include "ssh/ssh_account_info.h"

#include <future>
#include <wx/msgdlg.h>
#include <wx/string.h>

#if USE_SFTP
#include "codelite_events.h"
#include "event_notifier.h"
#endif

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

namespace llm
{

/// Run `callback` in the main thread

/**
 * @brief Executes the provided callback on the main (GUI) thread and returns its result.
 *
 * If the current thread is already the main thread, the callback is invoked directly.
 * Otherwise, the callback is scheduled to run in the next event loop iteration via
 * `llm::Manager::CallAfter`, and this function blocks until the callback completes.
 *
 * @param callback A `std::function<FunctionResult()>` representing the work to be performed on the main thread.
 * @param tool_name A `wxString` identifying the tool; used for debugging and logging purposes.
 *
 * @return The `FunctionResult` produced by the callback.
 */
assistant::FunctionResult RunOnMain(std::function<FunctionResult()> callback, const wxString& tool_name)
{
    auto promise_ptr = std::make_shared<std::promise<FunctionResult>>();
    auto f = promise_ptr->get_future();
    if (wxThread::IsMain()) {
        clDEBUG() << "Processing function:" << tool_name << "in the main thread directly" << endl;
        promise_ptr->set_value(callback());
    } else {
        auto wrapped_cb = [callback = std::move(callback), promise_ptr]() {
            // return the result using the promise.
            promise_ptr->set_value(callback());
        };

        // Run the callback in the next event loop
        llm::Manager::GetInstance().CallAfter(&llm::Manager::RunTool, std::move(wrapped_cb));
    }
    return f.get();
}

// Register CodeLite tools with the model.
void PopulateBuiltInFunctions(FunctionTable& table)
{
    table.Add(FunctionBuilder("ReadFileContent")
                  .SetDescription("Reads the entire content of the file 'filepath' from "
                                  "the disk. On success, this function returns the "
                                  "entire file's content.")
                  .AddRequiredParam("filepath", "The path of the file to read.", "string")
                  .SetCallback(ReadFileContent)
                  .Build());

    table.Add(FunctionBuilder("OpenFileInEditor")
                  .SetDescription("Try to open file 'filepath' and load it into the "
                                  "editor for editing or viewing.")
                  .AddRequiredParam("filepath", "The path of the file to open inside the editor.", "string")
                  .SetCallback(OpenFileInEditor)
                  .Build());

    table.Add(FunctionBuilder("GetActiveEditorFilePath")
                  .SetDescription(R"#(Retrieves the file path of the currently active editor.
Details:
This function executes on the main thread to safely access the active editor
and obtain its file path. If the editor is connected to a remote source, the remote
path is returned; otherwise, the local file path is returned.)#")
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
            .SetDescription(R"(Search for a given pattern within files in a directory.
Description:
This function searches for a specified text pattern or regular expression across files within a directory structure.
Parameters:
- Search String (string, required): The text pattern to search for (literal string or regex pattern).
- File Pattern (string, required): The file pattern to match, such as "*.txt" or "*.py".
- Root Folder (string, required): The root directory where the search begins.
- Whole Word (boolean, optional): When enabled, matches only complete words.
- Case Sensitive (boolean, optional): When enabled, performs case-sensitive matching.
- Is Regex (boolean, optional): When enabled, treats the search string as a regular expression pattern.)")
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
                  .SetDescription(R"(Apply a unified diff patch to a file.
Description:
This function applies a unified diff patch content to a specified file. It can perform a dry run to validate
the patch without modifying the file, or apply the changes directly.
Parameters:
- patch_content (string, required): The unified diff patch content to apply.
- file_path (string, required): The path to the file that should be patched.
- dry_run (boolean, optional): When true, validates the patch without modifying the file. Default is true.)")
                  .SetCallback(ApplyPatch)
                  .AddRequiredParam("patch_content", "The unified diff patch content to apply", "string")
                  .AddRequiredParam("file_path", "The path to the file that should be patched", "string")
                  .AddOptionalParam("dry_run",
                                    "When true, validates the patch without modifying the file. Default is true.",
                                    "boolean")
                  .Build());
    table.Add(FunctionBuilder("CreateNewFile")
                  .SetDescription(R"(Create a new file at the specified path with optional content.
Description:
This function creates a new file at the given filepath. If the file already exists, an error is returned.
Optionally, initial content can be provided to be written to the file.
Parameters:
- filepath (string, required): The path where the new file should be created.
- file_content (string, optional): The initial content to write to the file. Default is empty.)")
                  .SetCallback(CreateNewFile)
                  .AddRequiredParam("filepath", "The path where the new file should be created", "string")
                  .AddOptionalParam("file_content", "The initial content to write to the file", "string")
                  .Build());
}

/// Implementation details

FunctionResult CreateNewFile(const assistant::json& args)
{
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
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult ReadFileContent(const assistant::json& args)
{
    if (args.size() != 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(filepath));
        clDEBUG() << "ReadFileContent is called for:" << fullpath << endl;
        std::optional<wxString> content = FileManager::ReadContent(fullpath);
        if (!content.has_value()) {
            wxString msg;
            msg << "Error occurred while reading the file: '" << filepath << "' from disk.";
            clWARNING() << msg << endl;
            return Err(msg);
        }
        clDEBUG() << "ReadFileContent completed successfully." << endl;
        return Ok(content.value());
    };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult OpenFileInEditor(const assistant::json& args)
{
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
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult GetCompilerOutput([[maybe_unused]] const assistant::json& args)
{
    auto cb = [=]() -> FunctionResult { return Ok(clGetManager()->GetBuildOutput()); };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult GetCurrentEditorText([[maybe_unused]] const assistant::json& args)
{
    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("No editor is currently open");
        }
        return Ok(active_editor->GetEditorText());
    };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult GetCurrentEditorPath([[maybe_unused]] const assistant::json& args)
{
    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("No editor is currently open");
        }
        return Ok(active_editor->GetRemotePathOrLocal());
    };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult CreateWorkspace([[maybe_unused]] const assistant::json& args)
{
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
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

FunctionResult FindInFiles([[maybe_unused]] const assistant::json& args)
{
    if (wxThread::IsMain()) {
        clWARNING() << "Running Find-In-Files on the main thread" << endl;
    }

    if (args.size() < 3) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string root_dir, ::assistant::GetFunctionArg<std::string>(args, "root_folder"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string find_what, ::assistant::GetFunctionArg<std::string>(args, "find_what"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string file_pattern, ::assistant::GetFunctionArg<std::string>(args, "file_pattern"));
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
    assistant::json j = assistant::json::array();
    for (const auto& match : matches) {
        j.push_back(match.ToJson());
    }
    return Ok(wxString::FromUTF8(j.dump()));
}

FunctionResult ApplyPatch([[maybe_unused]] const assistant::json& args)
{
    if (args.size() < 2) {
        return Err("Invalid number of arguments");
    }

    auto cb = [=]() -> FunctionResult {
        ASSIGN_FUNC_ARG_OR_RETURN(
            std::string patch_content, ::assistant::GetFunctionArg<std::string>(args, "patch_content"));
        ASSIGN_FUNC_ARG_OR_RETURN(std::string file_path, ::assistant::GetFunctionArg<std::string>(args, "file_path"));

        bool dry_run = assistant::GetFunctionArg<bool>(args, "dry_run").value_or(true);

        PatchOptions opts{.dryRun = dry_run};
        auto result = PatchApplier::ApplyPatch(wxString::FromUTF8(patch_content), wxString::FromUTF8(file_path), opts);
        return FunctionResult{.isError = !result.success, .text = result.errorMessage.ToStdString(wxConvUTF8)};
    };

    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

} // namespace llm
