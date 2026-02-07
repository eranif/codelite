//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2026 by Eran Ifrah
// file name            : clFilesFinder.h
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

#ifndef CLFILESFINDER_H
#define CLFILESFINDER_H

#include "clFileFinder.h"
#include "codelite_exports.h"

#include <functional>
#include <unordered_set>
#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

/**
 * @brief Represents a match found in a specific file
 */
struct WXDLLIMPEXP_CL clFilesFinderMatch {
    wxString m_filepath; ///< Full path to the file containing the match
    std::vector<clFileFinderMatch> m_matches;

    clFilesFinderMatch() = default;
    clFilesFinderMatch(const wxString& path, std::vector<clFileFinderMatch> matches)
        : m_filepath(path)
        , m_matches(std::move(matches))
    {
    }

    inline nlohmann::json ToJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["filepath"] = m_filepath.ToStdString(wxConvUTF8);
        for (const auto& m : m_matches) {
            j["matches"].push_back(m.ToJson());
        }
        return j;
    }
};

/**
 * @brief Search options for clFilesFinder
 */
struct WXDLLIMPEXP_CL clFilesFinderOptions {
    wxString find_what;             ///< The pattern to search for (literal string or regex pattern)
    bool is_case_sensitive{false};  ///< If true, perform case-sensitive matching
    bool is_whole_match{false};     ///< If true, only match whole words
    bool is_regex{false};           ///< If true, treat find_what as a regular expression pattern
    wxString file_spec{"*"};        ///< File pattern to match (e.g., "*.cpp;*.h")
    wxString exclude_file_spec;     ///< File patterns to exclude
    wxString exclude_folders_spec;  ///< Folder patterns to exclude (e.g., ".git;build;node_modules")
    bool exclude_hidden_dirs{true}; ///< If true, skip hidden directories
    bool follow_symlinks{false};    ///< If true, follow symbolic links
};

/**
 * @brief Progress information passed to the progress callback
 */
struct WXDLLIMPEXP_CL clFilesFinderProgress {
    size_t files_scanned{0};      ///< Number of files scanned so far
    size_t files_with_matches{0}; ///< Number of files that contain matches
    size_t total_matches{0};      ///< Total number of matches found
    wxString current_file;        ///< The file currently being searched

    clFilesFinderProgress() = default;
};

/// Callback type for reporting progress during search
/// Return false from the callback to cancel the search
using clFilesFinderProgressCallback = std::function<bool(const clFilesFinderProgress&)>;

/// Callback type for reporting matches as they are found
/// Return false from the callback to cancel the search
using clFilesFinderMatchCallback = std::function<bool(const wxString&, const clFilesFinderMatch&)>;

/**
 * @brief A multi-file searcher that can search across files in a directory tree
 *
 * This class provides functionality to search for string patterns or regular expressions
 * across multiple files.
 * It supports:
 * - Recursive directory scanning with file pattern filtering
 * - Exclusion of specific files and folders
 * - Progress reporting via callbacks
 * - Match-by-match reporting via callbacks
 * - Cancellation support
 * - Regular expression searching
 *
 * Example usage:
 * @code
 * clFilesFinder finder;
 *
 * // Simple search - get all results at once
 * clFilesFinderOptions options;
 * options.find_what = "wxString";
 * options.file_spec = "*.cpp;*.h";
 * options.is_case_sensitive = false;
 * options.is_whole_match = true;
 *
 * auto results = finder.Search("/path/to/project", options);
 *
 * // Regex search example
 * clFilesFinderOptions regex_options;
 * regex_options.find_what = "wx[A-Z][a-zA-Z]+";  // Match wxString, wxWindow, etc.
 * regex_options.file_spec = "*.cpp;*.h";
 * regex_options.is_regex = true;
 *
 * auto regex_results = finder.Search("/path/to/project", regex_options);
 *
 * // Search with progress callback
 * auto results2 = finder.Search("/path/to/project", options,
 *     [](const clFilesFinderProgress& progress) {
 *         wxLogMessage("Scanned %zu files, found %zu matches",
 *                      progress.files_scanned, progress.total_matches);
 *         return true; // continue searching
 *     });
 *
 * // Search with match callback (for streaming results)
 * finder.Search("/path/to/project", options,
 *     nullptr, // no progress callback
 *     [](const clFilesFinderMatch& match) {
 *         wxLogMessage("Found in %s at line %zu", match.filepath, match.line);
 *         return true; // continue searching
 *     });
 * @endcode
 */
class WXDLLIMPEXP_CL clFilesFinder
{
public:
    clFilesFinder() = default;
    ~clFilesFinder() = default;

    /**
     * @brief Search for a pattern across all files in a directory tree
     * @param root_folder The root folder to start the search from
     * @param options Search options (pattern, case sensitivity, file filters, regex mode, etc.)
     * @param progress_cb Optional callback for progress reporting (return false to cancel)
     * @param match_cb Optional callback called for each match found (return false to cancel)
     * @return Vector of all matches found (empty if match_cb is provided and handles results)
     */
    std::vector<clFilesFinderMatch> Search(const wxString& root_folder,
                                           const clFilesFinderOptions& options,
                                           clFilesFinderProgressCallback progress_cb = nullptr,
                                           clFilesFinderMatchCallback match_cb = nullptr) const;

    /**
     * @brief Search for a pattern across all files in a directory tree (wxFileName overload)
     */
    std::vector<clFilesFinderMatch> Search(const wxFileName& root_folder,
                                           const clFilesFinderOptions& options,
                                           clFilesFinderProgressCallback progress_cb = nullptr,
                                           clFilesFinderMatchCallback match_cb = nullptr) const;

    /**
     * @brief Search for a pattern in a specific list of files (wxArrayString overload)
     */
    std::vector<clFilesFinderMatch> SearchInFiles(const wxArrayString& files,
                                                  const clFilesFinderOptions& options,
                                                  clFilesFinderProgressCallback progress_cb = nullptr,
                                                  clFilesFinderMatchCallback match_cb = nullptr) const;

private:
    /**
     * @brief Internal search implementation
     */
    std::vector<clFilesFinderMatch> DoSearchInFiles(const wxArrayString& files,
                                                    const clFilesFinderOptions& options,
                                                    clFilesFinderProgressCallback progress_cb,
                                                    clFilesFinderMatchCallback match_cb) const;

    /**
     * @brief Collect files from the root folder based on options
     */
    wxArrayString CollectFiles(const wxString& root_folder, const clFilesFinderOptions& options) const;

private:
    clFileFinder m_fileFinder; ///< Single-file finder used for searching individual files
};

#endif // CLFILESFINDER_H
