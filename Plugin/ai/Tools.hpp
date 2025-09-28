#pragma once

// clang-format off
#include <wx/app.h>
#include "ai/LLMManager.hpp"
#include "ollama/function_base.hpp"
// clang-format on

namespace llm
{
using ollama::FunctionResult;
using ollama::FunctionTable;

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

/// Write file to the disk.
FunctionResult WriteFileContent(const ollama::json& args);

/// Read filepath and return its content.
FunctionResult ReadFileContent(const ollama::json& args);

/// Open list of files to open.
FunctionResult OpenFileInEditor(const ollama::json& args);

/// Return the current compiler output.
FunctionResult GetCompilerOutput(const ollama::json& args);

/// Return the current editor's text content.
FunctionResult GetCurrentEditorText(const ollama::json& args);

/// Populate the function table with the built-in functions provided by CodeLite
/// to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

} // namespace llm
