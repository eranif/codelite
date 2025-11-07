#pragma once

// clang-format off
#include <wx/app.h>
#include "assistant/function_base.hpp"
// clang-format on

namespace llm
{
using assistant::FunctionResult;
using assistant::FunctionTable;

inline FunctionResult Ok(std::optional<wxString> text)
{
    FunctionResult result{.isError = false};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

inline FunctionResult Err(std::optional<wxString> text)
{
    FunctionResult result{.isError = true};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

/// Available API that CodeLite exposes to the model.

/**
 * @brief Writes the specified content to a file on disk.
 *
 * Expects a JSON object with exactly two entries:
 * - <code>"filepath"</code>: the target file path.
 * - <code>"file_content"</code>: the text to be written to the file.
 *
 * The function checks for the correct number of arguments, extracts the
 * parameters, and then executes the write operation on the main UI thread.
 *
 * If the file already exists, the user is prompted to confirm overwriting.
 * Upon successful write, the workspace view is refreshed and a success
 * message is returned. Errors during argument extraction, file writing,
 * or user cancellation result in an error message.
 *
 * @param args JSON object containing the required arguments.
 *
 * @return <code>FunctionResult</code> – an <code>Ok</code> result with a
 *         success message, or an <code>Err</code> result with an error
 *         description.
 */
FunctionResult WriteFileContent(const assistant::json& args);

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
 * @brief Lists all subdirectories within a specified directory path (non-recursive).
 *
 * This function scans the given directory and returns a JSON array containing the full paths
 * of all immediate subdirectories. The scan is non-recursive and filters results to include
 * only directories, excluding files. The operation is executed on the main thread.
 *
 * @param args A JSON object containing the function arguments. Must contain exactly one argument:
 *             - "path" (string): The directory path to scan for subdirectories. The path will be
 *               normalized to a full directory path via FileManager::GetDirectoryFullPath().
 *
 * @return FunctionResult containing either:
 *         - On success: A JSON-serialized string array of full directory paths (as UTF-8 strings).
 *         - On error: An error result with a descriptive message.
 *
 * @throws Returns an error FunctionResult (via Err()) if:
 *         - The args parameter does not contain exactly one argument.
 *         - The "path" argument is missing or cannot be parsed as a string.
 *
 * @note The [[maybe_unused]] attribute indicates that args may not be used in certain build configurations.
 * @note This function delegates its core logic to RunOnMain() to ensure thread-safe execution on the main thread.
 * @note Debugging output is logged via clDEBUG() showing the resolved directory path being scanned.
 *
 * @code
 * assistant::json args;
 * args["path"] = "/home/user/projects";
 * FunctionResult result = ListDirectories(args);
 * if (result.IsOk()) {
 *     std::string json_output = result.GetValue();
 *     // json_output contains: ["/home/user/projects/dir1", "/home/user/projects/dir2", ...]
 * }
 * @endcode
 *
 * @see RunOnMain
 * @see FileManager::GetDirectoryFullPath
 * @see clFilesScanner::ScanNoRecurse
 */
FunctionResult ListDirectories(const assistant::json& args);

/**
 * @brief Lists all files in a specified directory matching a given pattern.
 *
 * This function scans a directory non-recursively for files matching the provided
 * pattern and returns their full paths as a JSON array. The operation is executed
 * on the main thread for thread safety.
 *
 * @param args A JSON object containing the function arguments. Must have exactly 2 elements:
 *             - "dir" (std::string): The directory path to scan. Can be relative or absolute.
 *             - "pattern" (std::string): File pattern to match (e.g., "*.cpp", "*.txt").
 *
 * @return FunctionResult containing either:
 *         - On success: Ok() with a JSON string representing an array of file paths (std::string).
 *         - On error: Err() with an error message describing the failure.
 *
 * @throws Does not throw exceptions directly, but returns error FunctionResult for:
 *         - Invalid number of arguments (not exactly 2)
 *         - Missing or invalid "dir" argument
 *         - Missing or invalid "pattern" argument
 *
 * @example
 * @code
 * assistant::json args;
 * args["dir"] = "/home/user/project";
 * args["pattern"] = "*.cpp";
 * FunctionResult result = ListFiles(args);
 * if (result.IsOk()) {
 *     std::cout << "Files: " << result.GetValue() << std::endl;
 * } else {
 *     std::cerr << "Error: " << result.GetError() << std::endl;
 * }
 * @endcode
 *
 * @see RunOnMain, FileManager::GetDirectoryFullPath, clFilesScanner
 */
FunctionResult ListFiles([[maybe_unused]] const assistant::json& args);

/// Populate the function table with the built-in functions provided by CodeLite
/// to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

} // namespace llm
