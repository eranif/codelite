#pragma once

// clang-format off
#include <wx/app.h>
#include "assistant/function_base.hpp"
// clang-format on

#include "codelite_exports.h"

namespace llm
{
using assistant::FunctionResult;
using assistant::FunctionTable;

inline WXDLLIMPEXP_SDK FunctionResult Ok(std::optional<wxString> text)
{
    FunctionResult result{.isError = false};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

inline WXDLLIMPEXP_SDK FunctionResult Err(std::optional<wxString> text)
{
    FunctionResult result{.isError = true};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

/// Available API that CodeLite exposes to the model.

/**
 * @brief Creates a new file at the specified path with optional content.
 *
 * @details This function creates a new file on disk, optionally populating it with the provided content.
 * If the file already exists, the operation fails. When executed within a filesystem workspace context,
 * the workspace view is automatically refreshed after successful file creation. The actual file operation
 * is performed on the main thread via RunOnMain.
 *
 * @param args A JSON object containing exactly two arguments:
 *             - "filepath" (string, required): The path where the new file should be created.
 *             - "file_content" (string, optional): The content to write to the file. Defaults to empty string if not
 * provided.
 *
 * @return FunctionResult Returns Ok with a success message if the file was created successfully,
 *         or Err with an error message if:
 *         - The number of arguments is not exactly 2
 *         - The "filepath" argument is missing or invalid
 *         - The file already exists at the specified path
 *         - Writing the content to disk fails
 *
 * @see FileManager::GetFullPath
 * @see FileManager::WriteContent
 * @see clFileSystemWorkspace
 * @see RunOnMain
 */
FunctionResult CreateNewFile(const assistant::json& args);

/**
 * @brief Reads the content of a file specified by the "filepath" argument.
 *
 * @param args JSON object that must contain exactly one entry:
 *             - "filepath" (std::string): the path to the file to be read.
 *
 * @return FunctionResult containing the file's content as a {@code wxString} on success,
 *         or an error message encapsulated in {@code Err} on failure.
 *
 * @note The file reading operation is dispatched to the main thread using {@code RunOnMain}.
 */
FunctionResult ReadFileContent(const assistant::json& args);

/**
 * @brief Opens a file in the editor.
 *
 * Expects a JSON object containing exactly one entry: the key `"filepath"` with a string value specifying the file path
 * to open. The function loads the file either via the current workspace editor (if a workspace is opened) or via the
 * global editor manager. The operation is executed on the main thread and returns a result indicating success or
 * failure.
 *
 * @param args JSON object with a single `"filepath"` entry.
 *
 * @return FunctionResult containing an error message if the file could not be loaded, or a success message confirming
 * the file was opened in an editor.
 */
FunctionResult OpenFileInEditor(const assistant::json& args);

/**
 * @brief Retrieves the compiler/build output.
 *
 * This function schedules a task on the main thread to fetch the build output from the
 * compiler manager, wraps it in a successful {@code FunctionResult}, and returns it.
 *
 * @param args JSON arguments (currently unused).
 *
 * @return A {@code FunctionResult} containing the compiler output on success,
 *         or an error result if the operation fails.
 */
FunctionResult GetCompilerOutput(const assistant::json& args);

/**
 * @brief Retrieves the text of the currently active editor.
 *
 * This function schedules a callback to run on the main thread, obtains the
 * active editor via the editor manager, and returns its full text. If there
 * is no active editor, an error result is returned.
 *
 * @param args JSON arguments (currently unused).
 *
 * @return A {@link FunctionResult} containing the editor text on success,
 *         or an error message if no editor is open.
 */
FunctionResult GetCurrentEditorText(const assistant::json& args);

/**
 * @brief Creates a new workspace (either local or remote) with the specified name and path.
 *
 * This function attempts to create a new workspace at the given path with the provided name.
 * If a host is specified, it creates a remote workspace using SSH/SFTP; otherwise, it creates
 * a local filesystem workspace. The function ensures no workspace is currently open before
 * proceeding and validates SSH account information for remote workspaces.
 *
 * @param args A JSON object containing the function arguments with the following fields:
 *             - "path" (string, required): The directory path where the workspace should be created
 *             - "name" (string, optional): The name of the workspace to create
 *             - "host" (string, optional): The SSH host for creating a remote workspace
 *
 * @return FunctionResult A result object containing either:
 *         - Ok(wxEmptyString) on successful workspace creation
 *         - Err(message) with an error description if creation fails
 *
 * @throws May return errors for the following conditions:
 *         - A workspace is already open
 *         - Required arguments ("path" or "name") are missing or invalid
 *         - Multiple or no SSH accounts match the specified host
 *         - Remote workspace is requested but SFTP support is not compiled
 *         - A workspace file already exists at the specified path
 *
 * @see clWorkspaceManager
 * @see SSHAccountInfo
 * @see clFileSystemWorkspace
 * @see RunOnMain
 */
FunctionResult CreateWorkspace([[maybe_unused]] const assistant::json& args);

/**
 * @brief Searches for text occurrences in files within a specified directory tree.
 *
 * Recursively scans the given root directory for files matching the provided pattern,
 * then searches each file for the specified text. Results are returned as a JSON array
 * of match objects. This function is thread-safe but issues a warning if called from
 * the main thread due to potential UI freezing.
 *
 * @param args JSON object containing the following required fields:
 *   - @b root_folder (string): The directory path to begin searching from.
 *   - @b find_what (string): The text to search for in files.
 *   - @b file_pattern (string): File wildcard pattern (e.g., "*.cpp") to filter files.
 *   Optional fields:
 *   - @b whole_word (bool, default=true): If true, only match whole words.
 *   - @b case_sensitive (bool, default=true): If true, perform case-sensitive search.
 *
 * @return FunctionResult containing either:
 *   - On success: A UTF-8 encoded JSON string (as wxString) with an array of match objects.
 *   - On failure: An error message string if arguments are invalid or search fails.
 *
 * @throws None directly, but may propagate exceptions from filesystem operations or JSON parsing.
 *
 * @see clFilesFinder::Search()
 * @see assistant::GetFunctionArg()
 */
FunctionResult FindInFiles([[maybe_unused]] const assistant::json& args);

/**
 * @brief Applies a patch to a specified file.
 *
 * @details This function parses the provided patch content and applies it to the target file.
 * By default, the operation runs in dry-run mode, which validates the patch without modifying
 * the file. The function extracts required arguments from a JSON object and delegates the
 * actual patching logic to PatchApplier::ApplyPatch.
 *
 * @param args A JSON object containing the following keys:
 *             - "patch_content" (std::string, required): The unified diff or patch content to apply.
 *             - "file_path" (std::string, required): The path to the file to be patched.
 *             - "dry_run" (bool, optional): If true, validates the patch without applying it.
 *               Defaults to true.
 *
 * @return FunctionResult A result object where:
 *         - isError is false and text contains any output message on success.
 *         - isError is true and text contains the error message on failure.
 *
 * @throws None This function does not throw exceptions; errors are returned via FunctionResult.
 *
 * @see PatchApplier::ApplyPatch
 * @see PatchOptions
 */
FunctionResult ApplyPatch([[maybe_unused]] const assistant::json& args);

/**
 * @brief Retrieves the file path of the currently active editor.
 *
 * @details This function executes on the main thread to safely access the active editor
 * and obtain its file path. If the editor is connected to a remote source, the remote
 * path is returned; otherwise, the local file path is returned.
 *
 * @param args JSON arguments object (currently unused).
 *
 * @return FunctionResult containing the editor's remote path or local path as a string
 *         on success, or an error result if no editor is currently open.
 *
 * @throws None directly, but returns an error FunctionResult with the message
 *         "There is no editor opened" if no active editor exists.
 *
 * @see clGetManager()
 * @see IEditor::GetRemotePathOrLocal()
 * @see RunOnMain()
 */
FunctionResult GetCurrentEditorPath(const assistant::json& args);

/**
 * @brief Executes a shell command.
 */
FunctionResult ToolShellExecute(const assistant::json& args);

/// Populate the function table with the built-in functions provided by CodeLite
/// to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

} // namespace llm
