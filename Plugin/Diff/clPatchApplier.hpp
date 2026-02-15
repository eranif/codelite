#pragma once

#include "clResult.hpp"
#include "codelite_exports.h"
#include "wx/stc/stc.h"

#include <optional>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

/**
 * Represents a single hunk in a unified diff
 */
struct WXDLLIMPEXP_SDK PatchHunk {
    int originalStart;
    int originalCount;
    int newStart;
    int newCount;
    wxArrayString lines; // Lines with their prefix (+, -, or space)
};

/**
 * Represents a parsed unified patch
 */
struct WXDLLIMPEXP_SDK UnifiedPatch {
    wxString originalFile;
    wxString newFile;
    std::vector<PatchHunk> hunks;
};

/**
 * Result of a patch operation
 */
struct WXDLLIMPEXP_SDK PatchResult {
    bool success;
    wxString errorMessage;
    int hunksApplied;
    int hunksFailed;
    wxArrayString warnings;
};

/**
 * Options for patch application
 */
struct WXDLLIMPEXP_SDK PatchOptions {
    bool dryRun = false; // Don't actually modify the file
    bool backup = true;  // Create a backup file
    wxString working_directory;
};

struct WXDLLIMPEXP_SDK ITextArea {
    virtual int GetLineCount() = 0;
    virtual wxString GetLine(int line) = 0;
    virtual int PositionFromLine(int line) = 0;
    virtual void DeleteRange(int start, int lengthDelete) = 0;
    virtual void InsertText(int pos, const wxString& text) = 0;
};

struct WXDLLIMPEXP_SDK StcViewArea : public ITextArea {
    explicit StcViewArea(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
    }
    int GetLineCount() override { return m_ctrl->GetLineCount(); }
    wxString GetLine(int line) override { return m_ctrl->GetLine(line); }
    int PositionFromLine(int line) override { return m_ctrl->PositionFromLine(line); }
    void DeleteRange(int start, int lengthDelete) override { m_ctrl->DeleteRange(start, lengthDelete); }
    void InsertText(int pos, const wxString& text) override { m_ctrl->InsertText(pos, text); }
    wxStyledTextCtrl* m_ctrl{nullptr};
};

class WXDLLIMPEXP_SDK PatchApplier
{
public:
    /**
     * @brief Applies a patch to a file using the system 'patch' utility.
     *
     * This method writes the patch content to a temporary file, locates the system 'patch' command,
     * and executes it with the specified options. This function must be called from the main thread only.
     *
     * @param filePath The path to the file to be patched (currently unused in implementation).
     * @param patchContent The patch content in unified diff format to be applied.
     * @param options A PatchOptions structure containing configuration flags such as working directory,
     *                dry-run mode, and backup options.
     *
     * @return PatchResult A structure containing a success flag and an error message if the operation failed.
     *         Returns success=true if the patch was applied successfully, or success=false with an
     *         appropriate error message if the operation failed.
     *
     * @throws None. All errors are returned via the PatchResult structure.
     *
     * @note This function requires the 'patch' command-line utility to be available in the system PATH.
     * @note Must be called from the main thread; will fail if called from a worker thread.
     */
    static PatchResult ApplyPatchStrict(const wxString& filePath,
                                        const wxString& patchContent,
                                        const PatchOptions& options = PatchOptions{});
    /**
     * @brief Parses a unified diff patch string into a structured UnifiedPatch object.
     *
     * This function tokenizes the input patch content line by line and extracts file headers
     * (--- and +++), hunk headers (@@ ... @@), and hunk content lines (additions, deletions,
     * and context lines). Empty lines within hunks are treated as context lines.
     *
     * @param patchContent The unified diff patch content as a wxString.
     *
     * @return UnifiedPatch A structured object containing the original and new file paths,
     *         and a collection of hunks with their respective line ranges and content.
     *
     * @note Lines starting with '+', '-', ' ', or '\\' are captured as hunk content.
     *       Lines that do not match any expected pattern are ignored.
     */
    static UnifiedPatch ParseUnifiedPatch(const wxString& patchContent);

    /**
     * @brief Applies a unified diff patch to a file with loose matching, allowing hunks to be applied at flexible line
     * positions.
     *
     * This method parses the provided patch content and applies each hunk sequentially to the specified file.
     * The file is opened in the appropriate editor (remote via SFTP or local) and modified in-place.
     * This function must be called from the main thread only.
     *
     * @param filePath The path to the file to be patched (relative or absolute).
     * @param patchContent The unified diff patch content as a string.
     *
     * @return PatchResult A structure containing:
     *         - success: true if all hunks were applied successfully, false otherwise.
     *         - errorMessage: A description of the error if success is false, empty otherwise.
     *
     * @note This function can only be called from the main thread. Calling from another thread will result in failure.
     * @note The function performs in-place modification of the file through the editor interface.
     *
     * @see ParseUnifiedPatch
     * @see ApplyHunk
     * @see PatchResult
     */
    static PatchResult ApplyPatchLoose(const wxString& filePath, const wxString& patchContent);

    /**
     * @brief Applies a unified diff hunk to a ITextArea.
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
     * @param ctrl The ITextArea to apply the hunk to.
     * @param lines The array of hunk lines including their diff prefixes (' ', '-', '+').
     * @param start_line The line number (0-based) where the hunk should be applied.
     *
     * @return The line number where the next hunk can be applied, or wxNOT_FOUND if the hunk failed to apply.
     */
    static clStatusOr<int> ApplyHunk(ITextArea* ctrl, const wxArrayString& lines, int start_line);
};
