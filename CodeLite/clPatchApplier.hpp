#pragma once

#include "codelite_exports.h"

#include <optional>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

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
    bool dryRun = false; // Don't actually modify the file
    bool backup = true;  // Create a backup file
    wxString working_directory;
};

class WXDLLIMPEXP_CL PatchApplier
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
    static PatchResult
    ApplyPatch(const wxString& filePath, const wxString& patchContent, const PatchOptions& options = PatchOptions{});
};
