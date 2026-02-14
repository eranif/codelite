#pragma once
#include "codelite_exports.h"

#include <optional>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

/**
 * Represents a single hunk in a unified diff
 */
struct WXDLLIMPEXP_CL PatchHunk {
    int originalStart;
    int originalCount;
    int newStart;
    int newCount;
    wxArrayString lines; // Lines with their prefix (+, -, or space)
};

/**
 * Represents a parsed unified patch
 */
struct WXDLLIMPEXP_CL UnifiedPatch {
    wxString originalFile;
    wxString newFile;
    std::vector<PatchHunk> hunks;
};

/**
 * Result of a patch operation
 */
struct WXDLLIMPEXP_CL PatchResult {
    bool success;
    wxString errorMessage;
    int hunksApplied;
    int hunksFailed;
    wxArrayString warnings;
};

/**
 * Options for patch application
 */
struct WXDLLIMPEXP_CL PatchOptions {
    bool reverse = false; // Apply patch in reverse
    int fuzzFactor = 0;   // Number of lines of context that can be ignored
    bool dryRun = false;  // Don't actually modify the file
    bool backup = true;   // Create a backup file
    wxString backupSuffix = ".orig";
};

class WXDLLIMPEXP_CL PatchApplier
{
public:
    /**
     * Applies a unified patch to a file
     *
     * @param filePath Path to the file to be patched
     * @param patchContent The unified diff/patch content as a string
     * @param options Optional settings for patch application
     * @return PatchResult indicating success/failure and details
     */
    static PatchResult
    ApplyPatch(const wxString& filePath, const wxString& patchContent, const PatchOptions& options = PatchOptions{});

    /**
     * Applies a unified patch from a patch file to a target file
     */
    static PatchResult ApplyPatchFile(const wxString& filePath,
                                      const wxString& patchFilePath,
                                      const PatchOptions& options = PatchOptions{});

    /**
     * Parses a unified patch string into structured format
     */
    static UnifiedPatch ParseUnifiedPatch(const wxString& patchContent);

private:
    /**
     * Reads a file into a vector of lines
     */
    static wxArrayString ReadFileLines(const wxString& filePath);

    /**
     * Writes lines to a file
     */
    static void WriteFileLines(const wxString& filePath, const wxArrayString& lines);

    /**
     * Creates a backup of the file
     */
    static void CreateBackup(const wxString& filePath, const wxString& suffix);

    /**
     * Reverses a hunk for reverse patch application
     */
    static void ReverseHunk(PatchHunk& hunk);

    /**
     * Attempts to apply a single hunk to the file content
     * Returns the modified content if successful, nullopt otherwise
     */
    static std::optional<wxArrayString>
    ApplyHunk(const wxArrayString& lines, const PatchHunk& hunk, int offset, int fuzzFactor);

    /**
     * Tries to apply a hunk at a specific position
     */
    static std::optional<wxArrayString> TryApplyHunkAt(const wxArrayString& lines, const PatchHunk& hunk, int position);
};
