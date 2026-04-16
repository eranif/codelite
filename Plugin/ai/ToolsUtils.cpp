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

std::vector<FileMatchInfo> ParseGrepOutput(const wxArrayString& output_arr)
{
    std::unordered_map<wxString, FileMatchInfo> file_matches;

    for (const wxString& line : output_arr) {
        // Skip separator lines from context (they look like "--")
        if (line == wxT("--")) {
            continue;
        }

        // Find the first colon which separates filepath from line number
        int first_colon = line.Find(':');
        if (first_colon == wxNOT_FOUND) {
            continue;
        }

        wxString filepath = line.Mid(0, first_colon);
        wxString rest = line.Mid(first_colon + 1);

        // Extract line number (digits before next colon)
        long line_num = 0;
        rest.BeforeFirst(':').ToLong(&line_num);

        if (line_num > 0) {
            file_matches[filepath].filepath = filepath;
            file_matches[filepath].line_numbers.push_back(static_cast<int>(line_num));
        }
    }

    // Convert map to vector
    std::vector<FileMatchInfo> result;
    for (auto& pair : file_matches) {
        // Sort and deduplicate line numbers
        auto& lines = pair.second.line_numbers;
        std::sort(lines.begin(), lines.end());
        lines.erase(std::unique(lines.begin(), lines.end()), lines.end());
        result.push_back(pair.second);
    }

    return result;
}

std::string FormatGrepMatchesAsJson(const wxArrayString& output_arr,
                                    const std::vector<FileMatchInfo>& matches,
                                    size_t summary_threshold)
{
    using json = nlohmann::json;

    // Count total matches
    size_t total_matches = 0;
    for (const auto& fm : matches) {
        total_matches += fm.line_numbers.size();
    }

    json result_json = json::array();

    if (total_matches > summary_threshold) {
        // Summary mode: return compact summary with file path, match count, and line numbers
        for (const auto& fm : matches) {
            json file_summary = json::object();
            file_summary["f"] = fm.filepath.ToStdString(wxConvUTF8);
            file_summary["c"] = fm.line_numbers.size();
            file_summary["l"] = fm.line_numbers;
            result_json.push_back(file_summary);
        }

        // Add a note about summary mode
        json summary_note = json::object();
        summary_note["_note"] = "Summary mode: " + std::to_string(total_matches) + " matches across " +
                                std::to_string(matches.size()) + " files. Use ReadFileContent to view specific lines.";
        result_json.push_back(summary_note);
    } else {
        // Detailed mode: return full match information
        for (const wxString& line : output_arr) {
            // Skip separator lines
            if (line == wxT("--")) {
                continue;
            }

            int first_colon = line.Find(':');
            if (first_colon == wxNOT_FOUND) {
                continue;
            }

            wxString filepath = line.Mid(0, first_colon);
            wxString rest = line.Mid(first_colon + 1);

            json match = json::object();
            match["f"] = filepath.ToStdString(wxConvUTF8);

            // Extract line number and text
            int second_colon = rest.Find(':');
            if (second_colon != wxNOT_FOUND) {
                wxString line_num_str = rest.Mid(0, second_colon);
                wxString text = rest.Mid(second_colon + 1);
                match["l"] = std::stoi(line_num_str.ToStdString(wxConvUTF8));
                match["t"] = text.ToStdString(wxConvUTF8);
            }

            result_json.push_back(match);
        }
    }

    return result_json.dump();
}
} // namespace llm
