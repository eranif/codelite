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

/// Populate the function table with the built-in functions provided by CodeLite
/// to the model.
void PopulateBuiltInFunctions(FunctionTable& table);

} // namespace llm
