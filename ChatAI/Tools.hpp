#pragma once

// clang-format off
#include <wx/app.h>
#include "ai/LLMManager.hpp"
#include "ollama/function_base.hpp"
// clang-format on

namespace ollama
{

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

/// Register plugin function into the internal plugin function tables. This does not make the function available
/// it will become available once `PopulatePluginFunctions(..)` is called.
void RegisterPluginFunction(llm::Function func);

/// Populate the function table with the built-in functions provided by CodeLite to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

/// Populate the function table with the plugins functions provided by CodeLite's plugins.
/// This function is guarded by a mutex.
void PopulatePluginFunctions(FunctionTable& table);

} // namespace ollama
