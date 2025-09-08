#include "Tools.hpp"

#include "clWorkspaceManager.h"
#include "globals.h"
#include "ollama/function.hpp"

#include <future>
#include <wx/msgqueue.h>
#include <wx/string.h>

namespace ollama
{
namespace
{
FunctionResult Success(std::optional<wxString> text)
{
    FunctionResult result{.isError = false};
    if (text.has_value()) {
        result.text = text.value().ToStdString();
    }
    return result;
}

FunctionResult Error(std::optional<wxString> text)
{
    FunctionResult result{.isError = true};
    if (text.has_value()) {
        result.text = text.value().ToStdString();
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
    table.Add(FunctionBuilder("WriteFileContent")
                  .SetDescription(
                      "Write the content 'file_content' to the file system at the given path identified by 'filepath'")
                  .AddRequiredParam("file_content", "The content of the file to be written to the disk.", "string")
                  .AddRequiredParam("filepath", "The path file path.", "string")
                  .SetCallback(WriteFileContent)
                  .Build());

    table.Add(
        FunctionBuilder("ReadFileContent")
            .SetDescription(
                "Reads the entire content of the file 'filepath' from the disk. On success, this function returns the "
                "entire file's content.")
            .AddRequiredParam("filepath", "The path of the file to read.", "string")
            .SetCallback(ReadFileContent)
            .Build());

    table.Add(FunctionBuilder("OpenFileInEditor")
                  .SetDescription("Try to open file 'filepath' and load it into the editor for editing or viewing.")
                  .AddRequiredParam("filepath", "The path of the file to open inside the editor.", "string")
                  .SetCallback(OpenFileInEditor)
                  .Build());

    wxTheApp->Bind(wxEVT_IDLE, ProcessFunctionsQueue);
}

/// Implementation details

FunctionResult WriteFileContent(const ollama::json& args)
{
    if (args.size() != 2) {
        return Error("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));
    ASSIGN_FUNC_ARG_OR_RETURN(wxString file_content, ::ollama::GetFunctionArg<std::string>(args, "file_content"));

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            if (clWorkspaceManager::Get().GetWorkspace()->WriteFileContent(filepath, file_content)) {
                msg << "Error while writing file: '" << filepath << "' to disk.";
                return Error(msg);
            }
        } else {
            if (!FileUtils::WriteFileContent(filepath, file_content)) {
                msg << "Error while writing file: '" << filepath << "' to disk, check CodeLite logs.";
                return Error(msg);
            }
        }
        msg << "file: '" << filepath << "' successfully written to disk!.";
        return Success(msg);
    };
    return RunOnMain(std::move(cb));
}

FunctionResult ReadFileContent(const ollama::json& args)
{
    if (args.size() != 1) {
        return Error("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        wxString file_content;
        wxString msg;

        msg << "Error occurred while reading the file: '" << filepath << "' from disk.";
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            auto content = clWorkspaceManager::Get().GetWorkspace()->ReadFileContent(filepath);
            if (!content.has_value()) {
                return Error(msg);
            }
            file_content.swap(content.value());
        } else {
            if (!FileUtils::ReadFileContent(filepath, file_content)) {
                return Error(msg);
            }
        }
        return Success(file_content);
    };
    return RunOnMain(std::move(cb));
}

FunctionResult OpenFileInEditor(const ollama::json& args)
{
    if (args.size() != 1) {
        return Error("Invalid number of arguments");
    }

    ASSIGN_FUNC_ARG_OR_RETURN(wxString filepath, ::ollama::GetFunctionArg<std::string>(args, "filepath"));

    auto cb = [=]() -> FunctionResult {
        wxString msg;
        IEditor* editor{nullptr};
        if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
            clSYSTEM() << "Open file (workspace):" << filepath << endl;
            editor = clWorkspaceManager::Get().GetWorkspace()->OpenFileInEditor(filepath, false);
        } else {
            clSYSTEM() << "Open file:" << filepath << endl;
            editor = clGetManager()->OpenFile(filepath);
        }

        if (!editor) {
            msg << "Error occurred while loading file '" << filepath << "' into an editor.";
            return Error(msg);
        }
        msg << "File '" << filepath << "' has been successfully loaded into an editor.";
        return Success(msg);
    };
    return RunOnMain(std::move(cb));
}
} // namespace ollama
