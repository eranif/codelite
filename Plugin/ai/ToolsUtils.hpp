#pragma once

#include "assistant/common.hpp"
#include "codelite_exports.h"

#include <functional>
#include <string>
#include <wx/arrstr.h>
#include <wx/string.h>

namespace llm
{
/// Implementation details
#define VERIFY_WORKER_THREAD()                                                                   \
    if (wxIsMainThread()) {                                                                      \
        wxString message;                                                                        \
        message << _("Tool ") << __PRETTY_FUNCTION__ << _(" can not be run on the main thread"); \
        return Err(message);                                                                     \
    }

#define CONFIRM_ARG(VarType, VarName, InvalidValue, Expr, ErrorMsg) \
    VarType VarName = Expr.value_or(InvalidValue);                  \
    if (VarName == InvalidValue) {                                  \
        return CanInvokeToolResult{                                 \
            .can_invoke = false,                                    \
            .reason = ErrorMsg,                                     \
        };                                                          \
    }

using assistant::CanInvokeToolResult;

/**
 * Checks if a tool is permitted to access a specific file path and handles user trust prompts.
 *
 * This function validates the path against a trust list, potentially triggering a
 * user-facing prompt to grant permission for the specific file, its parent directory,
 * or the tool globally. It must be called from a non-GUI thread.
 *
 * @param tool_name The unique identifier of the tool requesting access.
 * @param prompt_text The text to display when asking the user for trust permission.
 * @param trusted_text The text to display when the path is already trusted.
 * @param filepath_utf8 The UTF-8 encoded string representing the path to the file.
 * @param validator An optional callback function to perform additional validation before checking trust.
 *
 * @return CanInvokeToolResult A result structure indicating whether the tool can be invoked and the reason if not.
 */
WXDLLIMPEXP_SDK CanInvokeToolResult ConfirmPathTool(const std::string& tool_name,
                                                    const wxString& prompt_text,
                                                    const wxString& trusted_text,
                                                    const std::string& filepath_utf8,
                                                    std::function<CanInvokeToolResult()> validator = nullptr);

/**
 * Truncates a stdout string to a specified maximum number of lines.
 *
 * The function converts a UTF-8 encoded string into a wxArrayString by tokenizing
 * it by newline characters and ensures the resulting array does not exceed
 * the provided limit or a hard-coded internal maximum of 100 lines.
 *
 * @param out The raw UTF-8 encoded output string to be processed.
 * @param max_lines The desired maximum number of lines to retain in the resulting array.
 * @return A wxArrayString containing the first few lines of the input string, capped at max_lines or 100.
 */
WXDLLIMPEXP_SDK wxArrayString TruncateStdout(const std::string& out, size_t max_lines = 20);
} // namespace llm
