#pragma once

// clang-format off
#include <wx/app.h>
#include "ollama/function_base.hpp"
// clang-format on

namespace ollama
{

/// Available API that CodeLite exposes to the model.
FunctionResult WriteFileContent(const ollama::json& args);
FunctionResult ReadFileContent(const ollama::json& args);
FunctionResult OpenFileInEditor(const ollama::json& args);

/// Populate the function table with the build-in functions provided by CodeLite to the model.
void PopulateBuildInFunctions(FunctionTable& table);

} // namespace ollama
