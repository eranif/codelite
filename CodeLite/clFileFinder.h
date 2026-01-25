//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2026 by Eran Ifrah
// file name            : clFileFinder.h
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

#ifndef CLFILEFINDER_H
#define CLFILEFINDER_H

#include "assistant/common/json.hpp"
#include "codelite_exports.h"

#include <vector>
#include <wx/filename.h>
#include <wx/string.h>

/**
 * @brief Represents a single match found in a file
 */
struct WXDLLIMPEXP_CL clFileFinderMatch {
    size_t line{0};        ///< 1-based line number where the match was found
    size_t col{0};         ///< 1-based column number where the match starts
    wxString matched_line; ///< The entire line containing the match

    clFileFinderMatch() = default;
    clFileFinderMatch(size_t l, size_t c, const wxString& text)
        : line(l)
        , col(c)
        , matched_line(text)
    {
    }

    inline nlohmann::json ToJson() const
    {
        return {{"line", line}, {"column", col}, {"text", matched_line.ToStdString(wxConvUTF8)}};
    }
};

/**
 * @brief A Unicode-aware file content searcher
 *
 * This class provides functionality to search for string patterns within files.
 * It supports:
 * - Case-sensitive and case-insensitive searches
 * - Whole word matching with proper Unicode word boundary detection
 * - Multiple file encodings (UTF-8, UTF-16 LE/BE with BOM detection)
 * - Unicode characters in search patterns
 *
 * Example usage:
 * @code
 * clFileFinder finder;
 * auto results = finder.FindInFile("/path/to/file.cpp", "searchTerm", false, true);
 * for (const auto& match : results) {
 *     wxLogMessage("Found at line %zu, col %zu: %s",
 *                  match.line, match.col, match.matched_line);
 * }
 * @endcode
 */
class WXDLLIMPEXP_CL clFileFinder
{
public:
    clFileFinder() = default;
    ~clFileFinder() = default;

    /**
     * @brief Search for a pattern in a file
     * @param filepath The path to the file to search
     * @param find_what The pattern to search for
     * @param is_case_sensitive If true, perform case-sensitive matching
     * @param is_whole_match If true, only match whole words (Unicode-aware word boundaries)
     * @return A vector of matches found in the file
     */
    std::vector<clFileFinderMatch> FindInFile(const wxFileName& filepath,
                                              const wxString& find_what,
                                              bool is_case_sensitive = false,
                                              bool is_whole_match = false) const;

    /**
     * @brief Search for a pattern in a file (convenience overload)
     * @param filepath The path to the file to search as a string
     * @param find_what The pattern to search for
     * @param is_case_sensitive If true, perform case-sensitive matching
     * @param is_whole_match If true, only match whole words (Unicode-aware word boundaries)
     * @return A vector of matches found in the file
     */
    std::vector<clFileFinderMatch> FindInFile(const wxString& filepath,
                                              const wxString& find_what,
                                              bool is_case_sensitive = false,
                                              bool is_whole_match = false) const;

    /**
     * @brief Search for a pattern in the given content string
     * @param content The text content to search in
     * @param find_what The pattern to search for
     * @param is_case_sensitive If true, perform case-sensitive matching
     * @param is_whole_match If true, only match whole words (Unicode-aware word boundaries)
     * @return A vector of matches found in the content
     */
    std::vector<clFileFinderMatch> FindInContent(const wxString& content,
                                                 const wxString& find_what,
                                                 bool is_case_sensitive = false,
                                                 bool is_whole_match = false) const;

private:
    /**
     * @brief Split content into lines, handling different line endings (\r\n, \n, \r)
     * @param content The content to split
     * @return Vector of lines
     */
    std::vector<wxString> SplitIntoLines(const wxString& content) const;

    /**
     * @brief Check if a character is a Unicode word character (letter, digit, or underscore)
     * @param ch The character to check
     * @return true if the character is a word character
     */
    bool IsWordCharacter(wxUniChar ch) const;

    /**
     * @brief Check if there's a word boundary at the given position in the string
     * @param str The string to check
     * @param pos The position to check
     * @return true if there's a word boundary at the position
     */
    bool IsWordBoundary(const wxString& str, size_t pos) const;

    /**
     * @brief Perform case-insensitive comparison of a substring
     * @param text The text to search in
     * @param pos Starting position in text
     * @param pattern The pattern to match
     * @return true if the pattern matches at the given position
     */
    bool CaseInsensitiveMatch(const wxString& text, size_t pos, const wxString& pattern) const;

    /**
     * @brief Find pattern in text starting from a given position
     * @param text The text to search in
     * @param pattern The pattern to find
     * @param start_pos Starting position for the search
     * @param case_sensitive Whether to perform case-sensitive search
     * @return Position of the match, or wxString::npos if not found
     */
    size_t FindPattern(const wxString& text, const wxString& pattern, size_t start_pos, bool case_sensitive) const;
};

#endif // CLFILEFINDER_H
