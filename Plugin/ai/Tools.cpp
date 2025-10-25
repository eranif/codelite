#include "Tools.hpp"

#include "FileManager.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "ai/LLMManager.hpp"
#include "assistant/function.hpp"
#include "clWorkspaceManager.h"
#include "globals.h"

#include <future>
#include <wx/msgdlg.h>
#include <wx/string.h>

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
    table.Add(FunctionBuilder("Write_file_content_to_disk_at_a_given_path")
                  .SetDescription("Write the content 'file_content' to the file "
                                  "system at the given path identified by 'filepath'")
                  .AddRequiredParam("file_content", "The content of the file to be written to the disk.", "string")
                  .AddRequiredParam("filepath", "The path file path.", "string")
                  .SetCallback(WriteFileContent)
                  .Build());

    table.Add(FunctionBuilder("Read_file_from_the_file_system")
                  .SetDescription("Reads the entire content of the file 'filepath' from "
                                  "the disk. On success, this function returns the "
                                  "entire file's content.")
                  .AddRequiredParam("filepath", "The path of the file to read.", "string")
                  .SetCallback(ReadFileContent)
                  .Build());

    table.Add(FunctionBuilder("Open_a_file_in_an_editor")
                  .SetDescription("Try to open file 'filepath' and load it into the "
                                  "editor for editing or viewing.")
                  .AddRequiredParam("filepath", "The path of the file to open inside the editor.", "string")
                  .SetCallback(OpenFileInEditor)
                  .Build());

    table.Add(FunctionBuilder("Read_the_compiler_build_output")
                  .SetDescription("Read and fetches the compiler build log output of "
                                  "the most recent build command executed by "
                                  "the user and return it to the caller. Use this "
                                  "method to read the compiler output. This is "
                                  "useful for helping explaining and resolving build "
                                  "issues. On success read, this function return "
                                  "the complete build log output.")
                  .SetCallback(GetCompilerOutput)
                  .Build());

    table.Add(FunctionBuilder("Get_the_text_of_the_active_tab_inside_the_editor")
                  .SetDescription("Return the text of the active tab inside the editor.")
                  .SetCallback(GetCurrentEditorText)
                  .Build());
}

/// Implementation details

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
FunctionResult WriteFileContent(const assistant::json& args)
{
    if (args.size() != 2) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(std::string filepath, ::assistant::GetFunctionArg<std::string>(args, "filepath"));
    ASSIGN_FUNC_ARG_OR_RETURN(std::string file_content, ::assistant::GetFunctionArg<std::string>(args, "file_content"));

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetFullPath(wxString::FromUTF8(filepath));
        if (FileManager::FileExists(fullpath)) {
            msg << _("The file: ") << fullpath << _(" already exists.\nWould you like to overwrite it?");
            if (::wxMessageBox(msg, "CodeLite", wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
                msg.clear();
                msg << _("The file: ") << fullpath << _(" already exists");
                return Err(msg);
            }
        }

        if (!FileManager::WriteContent(filepath, wxString::FromUTF8(file_content), true)) {
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
FunctionResult GetCompilerOutput(const assistant::json& args)
{
    auto cb = [=]() -> FunctionResult { return Ok(clGetManager()->GetBuildOutput()); };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

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
FunctionResult GetCurrentEditorText([[maybe_unused]] const assistant::json& args)
{
    auto cb = [=]() -> FunctionResult {
        auto active_editor = clGetManager()->GetActiveEditor();
        if (!active_editor) {
            return Err("There is no editor opened");
        }
        return Ok(active_editor->GetEditorText());
    };
    return RunOnMain(std::move(cb), __PRETTY_FUNCTION__);
}

} // namespace llm
