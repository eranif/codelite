#include "clPatchApplier.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "Platform/Platform.hpp"
#include "StringUtils.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "wx/regex.h"
#include "wx/tokenzr.h"

#include <optional>

UnifiedPatch PatchApplier::ParseUnifiedPatch(const wxString& patchContent)
{
    UnifiedPatch patch;

    // Split content into lines
    wxArrayString contentLines = wxStringTokenize(patchContent, "\n", wxTOKEN_RET_EMPTY_ALL);

    // wxRegEx for hunk header: @@ -l,s +l,s @@
    wxRegEx hunkHeaderRegex(
        wxT("^@@[[:space:]]+-([0-9]+)(,([0-9]+))?[[:space:]]+\\+([0-9]+)(,([0-9]+))?[[:space:]]+@@"));
    wxRegEx oldFileRegex(wxT("^---[[:space:]]+([^\t]+)"));
    wxRegEx newFileRegex(wxT("^\\+\\+\\+[[:space:]]+([^\t]+)"));

    PatchHunk* currentHunk = nullptr;

    for (size_t lineIdx = 0; lineIdx < contentLines.size(); ++lineIdx) {
        wxString line = contentLines[lineIdx];

        // Check for file headers
        if (oldFileRegex.Matches(line)) {
            patch.originalFile = oldFileRegex.GetMatch(line, 1);
            continue;
        }

        if (newFileRegex.Matches(line)) {
            patch.newFile = newFileRegex.GetMatch(line, 1);
            continue;
        }

        // Check for hunk header
        if (hunkHeaderRegex.Matches(line)) {
            patch.hunks.emplace_back();
            currentHunk = &patch.hunks.back();

            long val;
            hunkHeaderRegex.GetMatch(line, 1).ToLong(&val);
            currentHunk->originalStart = static_cast<int>(val);

            wxString origCountStr = hunkHeaderRegex.GetMatch(line, 3);
            if (!origCountStr.empty()) {
                origCountStr.ToLong(&val);
                currentHunk->originalCount = static_cast<int>(val);
            } else {
                currentHunk->originalCount = 1;
            }

            hunkHeaderRegex.GetMatch(line, 4).ToLong(&val);
            currentHunk->newStart = static_cast<int>(val);

            wxString newCountStr = hunkHeaderRegex.GetMatch(line, 6);
            if (!newCountStr.empty()) {
                newCountStr.ToLong(&val);
                currentHunk->newCount = static_cast<int>(val);
            } else {
                currentHunk->newCount = 1;
            }
            continue;
        }

        // Add line to current hunk
        if (currentHunk != nullptr) {
            if (line.empty()) {
                // Empty lines in patches are treated as context lines
                currentHunk->lines.push_back(" ");
            } else if (line[0] == '+' || line[0] == '-' || line[0] == ' ' || line[0] == '\\') {
                currentHunk->lines.push_back(line);
            }
        }
    }

    return patch;
}

PatchResult
PatchApplier::ApplyPatchStrict(const wxString& filePath, const wxString& patchContent, const PatchOptions& options)
{
    if (!wxThread::IsMain()) {
        return PatchResult{.success = false, .errorMessage = "ApplyPatch can only be called from the main thread"};
    }

    clTempFile input_file{clStandardPaths::Get().GetTempDir(), "patch"};
    if (!input_file.Write(patchContent)) {
        return PatchResult{.success = false, .errorMessage = "Failed to write patch into a temporary file"};
    }

    auto patch = ThePlatform->Which("patch");
    if (!patch.has_value()) {
        return PatchResult{.success = false, .errorMessage = "Could not locate the 'patch' command line utility"};
    }

    wxString command_line = StringUtils::WrapWithDoubleQuotes(patch.value());

    command_line << " --ignore-whitespace -p1";
    if (!options.working_directory.empty()) {
        command_line << " -d " << options.working_directory;
    }

    if (options.dryRun) {
        command_line << " --dry-run";
    }

    if (options.backup) {
        command_line << " --backup";
    }

    command_line << " --input=" << input_file.GetFullPath(true);

    clDEBUG() << "Applying patch:" << command_line << endl;
    wxArrayString output, error;
    auto error_code = ::wxExecute(command_line, output, error);

    if (error_code == 0) {
        return PatchResult{.success = true};
    } else {
        wxString output_str;
        output_str = StringUtils::Join(error);
        output_str << "\n" << StringUtils::Join(output);
        return PatchResult{.success = false, .errorMessage = output_str};
    }
}
