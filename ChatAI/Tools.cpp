#include "Tools.hpp"

#include "FileManager.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "clWorkspaceManager.h"
#include "globals.h"
#include "ollama/function.hpp"

#include <future>
#include <wx/msgdlg.h>
#include <wx/string.h>

namespace ollama
{
namespace
{
FunctionResult Ok(std::optional<wxString> text)
{
    FunctionResult result{.isError = false};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

FunctionResult Err(std::optional<wxString> text)
{
    FunctionResult result{.isError = true};
    if (text.has_value()) {
        result.text = text.value().ToStdString(wxConvUTF8);
    }
    return result;
}

} // namespace

std::mutex queue_mutex;
std::vector<std::function<void()>> functions_queue;

/// Run `callback` in the main thread
FunctionResult RunOnMain(std::function<FunctionResult()> callback)
{
    auto promise_ptr = std::make_shared<std::promise<FunctionResult>>();
    auto f = promise_ptr->get_future();
    if (wxThread::IsMain()) {
        clDEBUG() << "Processing function in the main thread directly" << endl;
        promise_ptr->set_value(callback());
    } else {
        auto wrapped_cb = [callback = std::move(callback), promise_ptr]() {
            // return the result using the promise.
            promise_ptr->set_value(callback());
        };

        // Queue the function call
        std::unique_lock lock{queue_mutex};
        functions_queue.push_back(std::move(wrapped_cb));
    }
    return f.get();
}

void ProcessFunctionsQueue(wxIdleEvent& event)
{
    event.Skip();
    std::unique_lock lock{queue_mutex};
    if (functions_queue.empty()) {
        return;
    }

    clDEBUG() << "Processing function in the idle event" << endl;
    auto func = functions_queue.front();
    functions_queue.erase(functions_queue.begin());
    func();
}

/// Register CodeLite tools with the model.
void PopulateBuildInFunctions(FunctionTable& table)
{
    table.Add(FunctionBuilder("Write file content to disk at a given path")
                  .SetDescription(
                      "Write the content 'file_content' to the file system at the given path identified by 'filepath'")
                  .AddRequiredParam("file_content", "The content of the file to be written to the disk.", "string")
                  .AddRequiredParam("filepath", "The path file path.", "string")
                  .SetCallback(WriteFileContent)
                  .Build());

    table.Add(
        FunctionBuilder("Read file from the file system")
            .SetDescription(
                "Reads the entire content of the file 'filepath' from the disk. On success, this function returns the "
                "entire file's content.")
            .AddRequiredParam("filepath", "The path of the file to read.", "string")
            .SetCallback(ReadFileContent)
            .Build());

    table.Add(FunctionBuilder("Open a file in an editor")
                  .SetDescription("Try to open file 'filepath' and load it into the editor for editing or viewing.")
                  .AddRequiredParam("filepath", "The path of the file to open inside the editor.", "string")
                  .SetCallback(OpenFileInEditor)
                  .Build());

    table.Add(FunctionBuilder("Read the compiler build output")
                  .SetDescription(
                      "Read and fetches the compiler build log output of the most recent build command executed by "
                      "the user and return it to the caller. Use this method to read the compiler output. This is "
                      "useful for helping explaining and resolving build issues. On success read, this function return "
                      "the complete build log output.")
                  .SetCallback(GetCompilerOutput)
                  .Build());

    wxTheApp->Bind(wxEVT_IDLE, ProcessFunctionsQueue);
}

/// Implementation details

FunctionResult WriteFileContent(const ollama::json& args)
{
    if (args.size() != 2) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));
    ASSIGN_FUNC_ARG_OR_RETURN(wxString file_content, ::ollama::GetFunctionArg<std::string>(args, "file_content"));

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        wxString fullpath = FileManager::GetFullPath(filepath);
        if (FileManager::FileExists(fullpath)) {
            msg << _("The file: ") << fullpath << _(" already exists.\nWould you like to overwrite it?");
            if (::wxMessageBox(msg, "CodeLite", wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
                msg.clear();
                msg << _("The file: ") << fullpath << _(" already exists");
                return Err(msg);
            }
        }

        if (!FileManager::WriteContent(filepath, file_content, true)) {
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
    return RunOnMain(std::move(cb));
}

FunctionResult ReadFileContent(const ollama::json& args)
{
    if (args.size() != 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        auto content = FileManager::ReadContent(filepath);
        if (!content.has_value()) {
            wxString msg;
            msg << "Error occurred while reading the file: '" << filepath << "' from disk.";
            return Err(msg);
        }
        return Ok(content.value());
    };
    return RunOnMain(std::move(cb));
}

FunctionResult OpenFileInEditor(const ollama::json& args)
{
    if (args.size() != 1) {
        return Err("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        IEditor* editor{nullptr};
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            clDEBUG() << "Open file (workspace):" << filepath << endl;
            editor = clWorkspaceManager::Get().GetWorkspace()->OpenFileInEditor(filepath, false);
        } else {
            clDEBUG() << "Open file:" << filepath << endl;
            editor = clGetManager()->OpenFile(filepath);
        }

        if (!editor) {
            msg << "Error occurred while loading file '" << filepath << "' into an editor.";
            return Err(msg);
        }
        msg << "File '" << filepath << "' has been successfully loaded into an editor.";
        return Ok(msg);
    };
    return RunOnMain(std::move(cb));
}

FunctionResult GetCompilerOutput(const ollama::json& args)
{
    auto cb = [=]() -> FunctionResult { return Ok(clGetManager()->GetBuildOutput()); };
    return RunOnMain(std::move(cb));
}

} // namespace ollama
