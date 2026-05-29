#include "ai/ToolsUtils.hpp"

#include "ai/LLMManager.hpp"
#include "fileutils.h"
#include "wx/tokenzr.h"

#include <assistant/common/json.hpp> // <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/thread.h>

namespace llm
{
CanInvokeToolResult ConfirmPathTool(const std::string& tool_name,
                                    const wxString& prompt_text,
                                    const wxString& trusted_text,
                                    const std::string& filepath_utf8,
                                    std::function<CanInvokeToolResult()> validator,
                                    const wxString& purpose)
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
        if (!purpose.IsEmpty()) {
            message << _("**Purpose:** ") << purpose << "\n\n";
        }
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
        if (!purpose.IsEmpty()) {
            message << _("**Purpose:** ") << purpose << "\n\n";
        }
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

namespace
{
// List of non-interesting folders to filter out from grep results
const wxArrayString& GetNonInterestingFolders()
{
    static const wxArrayString folders = []() {
        wxArrayString arr;
        // Wildcard patterns for non-interesting folders
        arr.Add(wxT("*/.git/*"));         // Git repository data
        arr.Add(wxT("*/.github/*"));      // GitHub repository data
        arr.Add(wxT("*/.cache/*"));       // Cache directories
        arr.Add(wxT("*/.svn/*"));         // SVN repository data
        arr.Add(wxT("*/.hg/*"));          // Mercurial repository data
        arr.Add(wxT("*/node_modules/*")); // Node.js modules
        arr.Add(wxT("*/__pycache__/*"));  // Python cache
        arr.Add(wxT("*/.vscode/*"));      // VS Code settings
        arr.Add(wxT("*/.idea/*"));        // JetBrains IDE settings
        arr.Add(wxT("*/build/*"));        // Build output
        arr.Add(wxT("*/dist/*"));         // Distribution packages
        arr.Add(wxT("*/.build-*/*"));     // Build directories with prefixes
        arr.Add(wxT("*/.codelite/*"));    // CodeLite workspace files
        return arr;
    }();
    return folders;
}

bool IsPathInNonInterestingFolder(const wxString& filepath)
{
    const wxArrayString& non_interesting = GetNonInterestingFolders();
    wxString path = filepath;

    // Use FileUtils::WildMatch for pattern matching
    for (const wxString& mask : non_interesting) {
        if (FileUtils::WildMatch(mask, path)) {
            return true;
        }
    }
    return false;
}
} // anonymous namespace

GrepResult ParseGrepOutput(const wxArrayString& output_arr)
{
    GrepResult grep_result;
    for (const wxString& line : output_arr) {
        // Skip separator lines from context (they look like "--")
        if (line == wxT("--")) {
            continue;
        }

        auto result = FileUtils::ParseGrepLine(line);
        if (!result) {
            continue;
        }

        const FileUtils::GrepMatch& match = result.value();

        // Skip files in non-interesting folders
        if (IsPathInNonInterestingFolder(match.filename)) {
            continue;
        }

        auto& v = grep_result.matches[match.filename]; // Create new or get an existing
        LineAndPattern line_and_pattern{
            .line_number = match.line_number,
            .pattern = match.matched_text,
        };
        v.push_back(std::move(line_and_pattern));
        grep_result.count++;
    }
    return grep_result;
}

} // namespace llm
