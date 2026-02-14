#include "clPatchApplier.hpp"

#include "codelite_exports.h"

#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "fileutils.h"

#include <optional>
#include <stdexcept>

PatchResult
PatchApplier::ApplyPatch(const wxString& filePath, const wxString& patchContent, const PatchOptions& options)
{
    PatchResult result{true, "", 0, 0, {}};

    try {
        // Parse the patch
        UnifiedPatch patch = ParseUnifiedPatch(patchContent);

        if (patch.hunks.empty()) {
            result.success = false;
            result.errorMessage = "No valid hunks found in patch";
            return result;
        }

        // Read the original file
        wxArrayString fileLines = ReadFileLines(filePath);

        // Apply each hunk
        wxArrayString resultLines = fileLines;
        int offset = 0; // Track line number changes from previous hunks

        for (size_t i = 0; i < patch.hunks.size(); ++i) {
            PatchHunk& hunk = patch.hunks[i];

            // Adjust for reverse application
            if (options.reverse) {
                ReverseHunk(hunk);
            }

            // Try to apply the hunk
            auto applyResult = ApplyHunk(resultLines, hunk, offset, options.fuzzFactor);

            if (applyResult.has_value()) {
                resultLines = std::move(applyResult.value());
                offset += (hunk.newCount - hunk.originalCount);
                result.hunksApplied++;
            } else {
                result.hunksFailed++;
                result.warnings.push_back("Hunk #" + wxString::Format("%d", (int)(i + 1)) + 
                                          " FAILED at line " + wxString::Format("%d", hunk.originalStart));
            }
        }

        if (result.hunksFailed > 0 && result.hunksApplied == 0) {
            result.success = false;
            result.errorMessage = "All hunks failed to apply";
            return result;
        }

        // Write the result (unless dry run)
        if (!options.dryRun) {
            // Create backup if requested
            if (options.backup) {
                CreateBackup(filePath, options.backupSuffix);
            }

            WriteFileLines(filePath, resultLines);
        }

    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = wxString::FromUTF8(e.what());
    }

    return result;
}

/**
 * Applies a unified patch from a patch file to a target file
 */
PatchResult
PatchApplier::ApplyPatchFile(const wxString& filePath, const wxString& patchFilePath, const PatchOptions& options)
{
    wxTextFile patchFile;
    if (!patchFile.Open(patchFilePath)) {
        return PatchResult{false, "Cannot open patch file: " + patchFilePath, 0, 0, {}};
    }

    wxString patchContent;
    for (wxString line = patchFile.GetFirstLine(); !patchFile.Eof(); line = patchFile.GetNextLine()) {
        patchContent << line << "\n";
    }
    // Don't forget the last line
    patchContent << patchFile.GetLastLine();
    patchFile.Close();
    
    return ApplyPatch(filePath, patchContent, options);
}

UnifiedPatch PatchApplier::ParseUnifiedPatch(const wxString& patchContent)
{
    UnifiedPatch patch;
    
    // Split content into lines
    wxArrayString contentLines = wxStringTokenize(patchContent, "\n", wxTOKEN_RET_EMPTY_ALL);

    // wxRegEx for hunk header: @@ -l,s +l,s @@
    wxRegEx hunkHeaderRegex(wxT("^@@[[:space:]]+-([0-9]+)(,([0-9]+))?[[:space:]]+\\+([0-9]+)(,([0-9]+))?[[:space:]]+@@"));
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

wxArrayString PatchApplier::ReadFileLines(const wxString& filePath)
{
    wxArrayString lines;
    wxTextFile file(filePath);

    if (!file.Exists()) {
        // File doesn't exist - might be a new file creation
        return lines;
    }

    if (!file.Open(filePath)) {
        // Could not open file
        return lines;
    }

    if (file.GetLineCount() > 0) {
        for (wxString line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()) {
            lines.push_back(line);
        }
        // Get the last line if not empty
        if (!file.GetLastLine().empty() || file.GetLineCount() == 1) {
            lines.push_back(file.GetLastLine());
        }
    }
    file.Close();
    return lines;
}

/**
 * Writes lines to a file
 */
void PatchApplier::WriteFileLines(const wxString& filePath, const wxArrayString& lines)
{
    wxTextFile file(filePath);
    
    if (file.Exists()) {
        if (!file.Open()) {
            throw std::runtime_error("Cannot open file for writing: " + filePath.ToStdString());
        }
        file.Clear();
    } else {
        if (!file.Create(filePath)) {
            throw std::runtime_error("Cannot create file: " + filePath.ToStdString());
        }
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        file.AddLine(lines[i]);
    }

    file.Write();
    file.Close();
}

/**
 * Creates a backup of the file
 */
void PatchApplier::CreateBackup(const wxString& filePath, const wxString& suffix)
{
    wxString backupPath = filePath + suffix;
    
    if (!wxFile::Exists(filePath)) {
        return;
    }
    
    if (!wxCopyFile(filePath, backupPath)) {
        throw std::runtime_error("Cannot create backup file");
    }
}

/**
 * Reverses a hunk for reverse patch application
 */
void PatchApplier::ReverseHunk(PatchHunk& hunk)
{
    std::swap(hunk.originalStart, hunk.newStart);
    std::swap(hunk.originalCount, hunk.newCount);

    for (size_t i = 0; i < hunk.lines.size(); ++i) {
        wxString& line = hunk.lines[i];
        if (!line.empty()) {
            wxChar firstChar = line[0];
            if (firstChar == '+') {
                line[0] = '-';
            } else if (firstChar == '-') {
                line[0] = '+';
            }
        }
    }
}

/**
 * Attempts to apply a single hunk to the file content
 * Returns the modified content if successful, nullopt otherwise
 */
std::optional<wxArrayString>
PatchApplier::ApplyHunk(const wxArrayString& lines, const PatchHunk& hunk, int offset, int fuzzFactor)
{
    // Try to find the best match for this hunk
    int startLine = hunk.originalStart - 1 + offset; // Convert to 0-based

    // Try exact position first, then search with increasing offset
    for (int fuzz = 0; fuzz <= fuzzFactor; ++fuzz) {
        for (int searchOffset = 0; searchOffset <= fuzz; ++searchOffset) {
            for (int direction : {0, -1, 1}) {
                int tryPos = startLine + (direction * searchOffset);
                if (tryPos < 0)
                    continue;

                auto result = TryApplyHunkAt(lines, hunk, tryPos);
                if (result.has_value()) {
                    return result;
                }
            }
        }
    }

    return std::nullopt;
}

/**
 * Tries to apply a hunk at a specific position
 */
std::optional<wxArrayString>
PatchApplier::TryApplyHunkAt(const wxArrayString& lines, const PatchHunk& hunk, int position)
{
    // Extract context and removed lines from the hunk
    wxArrayString expectedLines;
    for (size_t i = 0; i < hunk.lines.size(); ++i) {
        const wxString& line = hunk.lines[i];
        if (line.empty())
            continue;
        wxChar firstChar = line[0];
        if (firstChar == ' ' || firstChar == '-') {
            expectedLines.push_back(line.Mid(1));
        }
        // Skip "\ No newline at end of file" markers
        if (firstChar == '\\')
            continue;
    }

    // Verify the context matches
    if (position + static_cast<int>(expectedLines.size()) > static_cast<int>(lines.size())) {
        return std::nullopt;
    }

    for (size_t i = 0; i < expectedLines.size(); ++i) {
        if (position + static_cast<int>(i) >= static_cast<int>(lines.size())) {
            return std::nullopt;
        }
        if (lines[position + i] != expectedLines[i]) {
            return std::nullopt;
        }
    }

    // Apply the hunk
    wxArrayString result;

    // Copy lines before the hunk
    for (int i = 0; i < position; ++i) {
        result.push_back(lines[i]);
    }

    // Apply the hunk changes
    for (size_t i = 0; i < hunk.lines.size(); ++i) {
        const wxString& line = hunk.lines[i];
        if (line.empty())
            continue;
        wxChar firstChar = line[0];
        if (firstChar == ' ' || firstChar == '+') {
            // Context or addition - include in output
            result.push_back(line.Mid(1));
        }
        // Removed lines (starting with '-') are not included
        // Skip "\ No newline at end of file" markers
    }

    // Copy lines after the hunk
    for (size_t i = position + expectedLines.size(); i < lines.size(); ++i) {
        result.push_back(lines[i]);
    }

    return result;
}
