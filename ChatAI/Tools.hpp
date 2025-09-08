#pragma once

// clang-format off
#include <wx/app.h>
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

/// Create or load a local file system workspace.
FunctionResult CreateOrOpenLocalWorkspace(const ollama::json& args);

/// Create or load a local file system workspace.
FunctionResult AddFilesToWorkspace(const ollama::json& args);

/// Populate the function table with the build-in functions provided by CodeLite to the model.
void PopulateBuildInFunctions(FunctionTable& table);

} // namespace ollama
