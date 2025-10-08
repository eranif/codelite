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

/// Write file to the disk.
FunctionResult WriteFileContent(const assistant::json& args);

/// Read filepath and return its content.
FunctionResult ReadFileContent(const assistant::json& args);

/// Open list of files to open.
FunctionResult OpenFileInEditor(const assistant::json& args);

/// Return the current compiler output.
FunctionResult GetCompilerOutput(const assistant::json& args);

/// Return the current editor's text content.
FunctionResult GetCurrentEditorText(const assistant::json& args);

/// Populate the function table with the built-in functions provided by CodeLite
/// to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

} // namespace llm
