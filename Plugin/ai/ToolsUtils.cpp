#include "ai/ToolsUtils.hpp"

#include "ai/LLMManager.hpp"
#include "fileutils.h"
#include "wx/tokenzr.h"

#include <string>
#include <vector>
#include <wx/string.h>
#include <wx/thread.h>

namespace llm
{
CanInvokeToolResult ConfirmPathTool(const std::string& tool_name,
                                    const wxString& prompt_text,
                                    const wxString& trusted_text,
                                    const std::string& filepath_utf8,
                                    std::function<CanInvokeToolResult()> validator)
{
    if (wxIsMainThread()) {
        return CanInvokeToolResult{.can_invoke = false, .reason = "Internal Error."};
    }

    if (validator) {
        auto res = validator();
        if (!res.can_invoke)
            return res;
    }

    wxString fullpath = FileUtils::NormalizePath(wxString::FromUTF8(filepath_utf8));
    bool is_trusted = llm::Manager::GetInstance().CheckIfPathIsAllowedForTool(tool_name, fullpath);

    if (!is_trusted) {
        wxString message;
        message << prompt_text << "\n```\n" << fullpath << "\n```\n";
        return llm::Manager::GetInstance().PromptUserYesNoTrustQuestion(message, [tool_name, fullpath]() {
            auto& config = llm::Manager::GetInstance().GetConfig();
            wxString dirpath = FileUtils::GetPath(fullpath);

            std::vector<std::pair<wxString, wxString>> options{
                {wxString::Format(_("Specific path: %s"), fullpath), fullpath},
                {wxString::Format(_("Entire directory: %s/*"), dirpath), dirpath + "/*"},
                {_("Entire tool"), "*"},
            };

            auto result = llm::Manager::GetInstance().ShowTrustLevelDialog(tool_name, options);
            if (result.has_value()) {
                config.AddTrustedTool(tool_name, result->first, result->second);
                if (result->second)
                    config.Save(false);
            }
        });
    } else {
        wxString message;
        message << trusted_text << "\n```\n" << fullpath << "\n```\n";
        llm::Manager::GetInstance().PrintMessage(message, IconType::kInfo);
        return CanInvokeToolResult{.can_invoke = true};
    }
}

wxArrayString TruncateStdout(const std::string& out, size_t max_lines)
{
    static constexpr size_t kMaxLines = 100;
    max_lines = wxMin(kMaxLines, max_lines);

    wxArrayString result = ::wxStringTokenize(wxString::FromUTF8(out), "\n", wxTOKEN_STRTOK);
    if (result.size() < max_lines) {
        return result;
    }

    result.resize(max_lines);
    result.Shrink();
    return result;
}
} // namespace llm
