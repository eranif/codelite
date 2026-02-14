#include "clPatchApplier.hpp"

#include "AsyncProcess/asyncprocess.h"
#include "Platform/Platform.hpp"
#include "StringUtils.h"
#include "clTempFile.hpp"
#include "cl_standard_paths.h"
#include "file_logger.h"

#include <optional>

PatchResult
PatchApplier::ApplyPatch(const wxString& filePath, const wxString& patchContent, const PatchOptions& options)
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
