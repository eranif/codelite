#include "clPatchApplier.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "FileManager.hpp"
#include "Platform/Platform.hpp"
#include "StringUtils.h"
#include "clResult.hpp"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"
#include "globals.h"
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

namespace
{
/**
 * @brief Applies a unified diff hunk to a wxStyledTextCtrl.
 *
 * This function applies a hunk of changes to the editor control starting at the specified line.
 * The hunk lines should be in unified diff format with prefixes:
 * - ' ' (space) for context lines (unchanged)
 * - '-' for lines to be removed
 * - '+' for lines to be added
 *
 * The function validates that context and deletion lines match the current content before
 * applying any changes. If validation fails, no changes are made.
 *
 * @param ctrl The wxStyledTextCtrl to apply the hunk to.
 * @param lines The array of hunk lines including their diff prefixes (' ', '-', '+').
 * @param start_line The line number (0-based) where the hunk should be applied.
 *
 * @return The line number where the next hunk can be applied, or wxNOT_FOUND if the hunk failed to apply.
 */
clStatusOr<int> ApplyHunk(wxStyledTextCtrl* ctrl, const wxArrayString& lines, int start_line)
{
    if (!ctrl || lines.IsEmpty()) {
        return StatusInvalidArgument("Empty hunk");
    }

    int totalLines = ctrl->GetLineCount();
    if (start_line < 0 || start_line > totalLines) {
        return StatusInvalidArgument("Not enough lines");
    }

    wxString hunk_start_line = lines[0];
    if (!hunk_start_line.StartsWith(" ")) {
        return StatusInvalidArgument("Hunk must start with a context line");
    }
    hunk_start_line.Remove(0, 1).Trim(); // Remove the prefix and trim trailing spaces

    // Validate the hunk + prepare list of lines to search in the editor (removed or context lines).
    wxArrayString lines_to_find;
    for (auto l : lines) {
        if (l.empty()) {
            return StatusInvalidArgument("Invalid hunk");
        }

        wxChar prefx = l[0];
        switch (prefx) {
        case ' ':
        case '-':
            lines_to_find.push_back(l.Mid(1).Trim());
            break;
        case '+':
            break;
        default:
            return StatusInvalidArgument("Hunk lines must start with ' ', '-' or '+'");
        }
    }

    // First pass: locate the hunk.
    int hunk_found_line{wxNOT_FOUND};
    for (size_t current_editor_line_number = start_line; current_editor_line_number < totalLines;
         ++current_editor_line_number) {
        wxString editor_line = ctrl->GetLine(current_editor_line_number);
        editor_line.Trim();
        if (editor_line != hunk_start_line) {
            continue;
        }

        bool hunk_found{true};
        for (size_t hunk_line_number = 0; hunk_line_number < lines_to_find.size(); ++hunk_line_number) {
            wxString hunk_line = lines_to_find[hunk_line_number];
            // These lines must exist and match in the control
            if (current_editor_line_number + hunk_line_number >= totalLines) {
                return StatusInvalidArgument("Not enough lines");
            }

            editor_line = ctrl->GetLine(current_editor_line_number + hunk_line_number);
            editor_line.Trim();

            if (editor_line != hunk_line) {
                hunk_found = false;
                break;
            }
        }

        if (hunk_found) {
            hunk_found_line = current_editor_line_number;
            break;
        }
    }

    if (hunk_found_line == wxNOT_FOUND) {
        return StatusNotFound("Could not find hunk in editor");
    }

    int currentLine = hunk_found_line;
    for (size_t i = 0; i < lines.GetCount(); ++i) {
        wxString line = lines[i];
        if (line.IsEmpty()) {
            continue;
        }

        wxChar prefix = line[0];
        wxString content = line.Mid(1);

        if (prefix == ' ') {
            // Context line - just move to next line
            currentLine++;
        } else if (prefix == '-') {
            // Delete line
            int lineStart = ctrl->PositionFromLine(currentLine);
            int lineEnd = ctrl->PositionFromLine(currentLine + 1);
            ctrl->DeleteRange(lineStart, lineEnd - lineStart);
            // Don't increment currentLine since we deleted the line
        } else if (prefix == '+') {
            // Add line
            int pos = ctrl->PositionFromLine(currentLine);
            // Ensure content has a line ending
            if (!content.EndsWith("\n") && !content.EndsWith("\r\n")) {
                content += "\n";
            }
            ctrl->InsertText(pos, content);
            currentLine++;
        }
    }

    // Return the line where the next hunk can be applied
    return currentLine;
}
} // namespace

PatchResult PatchApplier::ApplyPatchLoose(const wxString& filePath, const wxString& patchContent)
{
    clDEBUG() << "ApplyPatchLoose is called for file:" << filePath << "and patch:\n" << patchContent << endl;
    if (!wxThread::IsMain()) {
        return PatchResult{.success = false, .errorMessage = "ApplyPatchLoose can only be called from the main thread"};
    }

    auto patch = ParseUnifiedPatch(patchContent);
    if (patch.hunks.empty()) {
        return PatchResult{.success = false, .errorMessage = "Failed to parse patch hunks"};
    }

    auto fullpath = FileManager::GetFullPath(filePath);
    IEditor* editor{nullptr};
#if USE_SFTP
    if (clWorkspaceManager::Get().IsWorkspaceOpened() && clWorkspaceManager::Get().GetWorkspace()->IsRemote()) {
        editor = clSFTPManager::Get().OpenFile(fullpath, clWorkspaceManager::Get().GetWorkspace()->GetSshAccount());

    } else {
        editor = clGetManager()->OpenFile(fullpath);
    }
#else
    editor = clGetManager()->OpenFile(fullpath);
#endif

    if (!editor) {
        return PatchResult{.success = false, .errorMessage = "Failed to open source file."};
    }

    wxStyledTextCtrl* stc = editor->GetCtrl();
    // Apply hunks
    int start_line{0};
    stc->BeginUndoAction();
    for (const auto& hunk : patch.hunks) {
        auto res = ApplyHunk(stc, hunk.lines, start_line);
        if (!res.ok()) {
            // Revert the changes
            stc->EndUndoAction();
            stc->Undo();
            return PatchResult{.success = false, .errorMessage = res.error_message()};
        }
        start_line = res.value();
    }
    stc->EndUndoAction();
    return PatchResult{.success = true};
}
