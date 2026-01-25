//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2026 by Eran Ifrah
// file name            : clFilesFinder.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "clFilesFinder.h"

#include "StringUtils.h"
#include "clFilesCollector.h"

#include <wx/log.h>

std::vector<clFilesFinderMatch> clFilesFinder::Search(const wxString& root_folder,
                                                      const clFilesFinderOptions& options,
                                                      clFilesFinderProgressCallback progress_cb,
                                                      clFilesFinderMatchCallback match_cb) const
{
    // Validate inputs
    if (options.find_what.IsEmpty() || root_folder.IsEmpty()) {
        return {};
    }

    // Collect files from the root folder
    auto files = CollectFiles(root_folder, options);

    // Search in the collected files
    return DoSearchInFiles(files, options, progress_cb, match_cb);
}

std::vector<clFilesFinderMatch> clFilesFinder::Search(const wxFileName& root_folder,
                                                      const clFilesFinderOptions& options,
                                                      clFilesFinderProgressCallback progress_cb,
                                                      clFilesFinderMatchCallback match_cb) const
{
    return Search(root_folder.GetPath(), options, progress_cb, match_cb);
}

std::vector<clFilesFinderMatch> clFilesFinder::SearchInFiles(const wxArrayString& files,
                                                             const clFilesFinderOptions& options,
                                                             clFilesFinderProgressCallback progress_cb,
                                                             clFilesFinderMatchCallback match_cb) const
{
    return DoSearchInFiles(files, options, progress_cb, match_cb);
}

std::vector<clFilesFinderMatch> clFilesFinder::DoSearchInFiles(const wxArrayString& files,
                                                               const clFilesFinderOptions& options,
                                                               clFilesFinderProgressCallback progress_cb,
                                                               clFilesFinderMatchCallback match_cb) const
{
    std::vector<clFilesFinderMatch> results;
    clFilesFinderProgress progress;
    bool cancelled = false;

    for (const auto& filepath : files) {
        if (cancelled) {
            break;
        }

        // Update progress
        progress.files_scanned++;
        progress.current_file = filepath;

        // Report progress if callback is provided
        if (progress_cb && !progress_cb(progress)) {
            // User cancelled
            cancelled = true;
            break;
        }

        // Search in this file
        auto file_matches =
            m_fileFinder.FindInFile(filepath, options.find_what, options.is_case_sensitive, options.is_whole_match);

        if (file_matches.empty()) {
            if (match_cb && !match_cb(filepath, {})) {
                // User cancelled
                cancelled = true;
                break;
            }
            continue;
        }

        progress.files_with_matches++;
        progress.total_matches += file_matches.size();
        clFilesFinderMatch entry{filepath, std::move(file_matches)};

        // If match callback is provided, call it
        if (match_cb && !match_cb(filepath, entry)) {
            // User cancelled
            cancelled = true;
            break;
        }
        results.push_back(std::move(entry));
    }

    // Final progress report
    if (progress_cb && !cancelled) {
        progress.current_file.Clear();
        progress_cb(progress);
    }

    return results;
}

wxArrayString clFilesFinder::CollectFiles(const wxString& root_folder, const clFilesFinderOptions& options) const
{
    wxArrayString files;

    // Use clFilesScanner to collect files
    clFilesScanner scanner;

    // Use the Scan method with file spec and exclusions
    scanner.Scan(root_folder, files, options.file_spec, options.exclude_file_spec, options.exclude_folders_spec);
    return files;
}
