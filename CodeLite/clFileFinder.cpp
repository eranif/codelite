//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2026 by Eran Ifrah
// file name            : clFileFinder.cpp
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

#include "clFileFinder.h"

#include "fileutils.h"

std::vector<clFileFinderMatch> clFileFinder::FindInFile(const wxFileName& filepath,
                                                        const wxString& find_what,
                                                        bool is_case_sensitive,
                                                        bool is_whole_match,
                                                        bool is_regex) const
{
    std::vector<clFileFinderMatch> results;

    // Validate inputs
    if (find_what.IsEmpty() || !filepath.FileExists()) {
        return results;
    }

    // Read file content with proper encoding handling
    wxString content;
    if (!FileUtils::ReadFileContent(filepath, content, wxConvUTF8) || content.empty()) {
        return results;
    }

    return FindInContent(content, find_what, is_case_sensitive, is_whole_match, is_regex);
}

std::vector<clFileFinderMatch> clFileFinder::FindInFile(const wxString& filepath,
                                                        const wxString& find_what,
                                                        bool is_case_sensitive,
                                                        bool is_whole_match,
                                                        bool is_regex) const
{
    return FindInFile(wxFileName(filepath), find_what, is_case_sensitive, is_whole_match, is_regex);
}

std::vector<clFileFinderMatch> clFileFinder::FindInContent(const wxString& content,
                                                           const wxString& find_what,
                                                           bool is_case_sensitive,
                                                           bool is_whole_match,
                                                           bool is_regex) const
{
    std::vector<clFileFinderMatch> results;

    if (find_what.IsEmpty() || content.IsEmpty()) {
        return results;
    }

    // Split content into lines
    std::vector<wxString> lines = SplitIntoLines(content);

    if (is_regex) {
        // Compile the regex pattern
        int flags = wxRE_ADVANCED;
        if (!is_case_sensitive) {
            flags |= wxRE_ICASE;
        }

        wxRegEx regex;
        if (!regex.Compile(find_what, flags)) {
            // Invalid regex pattern, return empty results
            return results;
        }

        // Search each line using regex
        size_t line_number = 0;
        for (const wxString& line : lines) {
            ++line_number;
            FindRegexMatchesInLine(line, regex, line_number, is_whole_match, results);
        }
    } else {
        // Non-regex search (original implementation)
        size_t line_number = 0;
        for (const wxString& line : lines) {
            ++line_number;

            size_t pos = 0;
            while ((pos = FindPattern(line, find_what, pos, is_case_sensitive)) != wxString::npos) {
                bool include_match = true;

                if (is_whole_match) {
                    // Check word boundaries using Unicode-aware function
                    bool boundary_before = IsWordBoundary(line, pos);
                    bool boundary_after = IsWordBoundary(line, pos + find_what.length());
                    include_match = boundary_before && boundary_after;
                }

                if (include_match) {
                    results.emplace_back(line_number, pos + 1, line); // 1-based column
                }

                // Move to next position (move by 1 to allow finding adjacent matches)
                ++pos;
            }
        }
    }

    return results;
}

void clFileFinder::FindRegexMatchesInLine(const wxString& line,
                                          wxRegEx& regex,
                                          size_t line_number,
                                          bool is_whole_match,
                                          std::vector<clFileFinderMatch>& results) const
{
    wxString remaining = line;
    size_t offset = 0;

    while (!remaining.IsEmpty() && regex.Matches(remaining)) {
        size_t start = 0;
        size_t len = 0;

        if (!regex.GetMatch(&start, &len, 0)) {
            break;
        }

        // Calculate the actual position in the original line
        size_t actual_pos = offset + start;

        bool include_match = true;
        if (is_whole_match) {
            // Check word boundaries using Unicode-aware function
            bool boundary_before = IsWordBoundary(line, actual_pos);
            bool boundary_after = IsWordBoundary(line, actual_pos + len);
            include_match = boundary_before && boundary_after;
        }

        if (include_match) {
            results.emplace_back(line_number, actual_pos + 1, line); // 1-based column
        }

        // Move past the current match to find additional matches
        // Move by at least 1 character to avoid infinite loop on zero-length matches
        size_t advance = (len > 0) ? (start + len) : (start + 1);
        if (advance >= remaining.length()) {
            break;
        }

        offset += advance;
        remaining = remaining.Mid(advance);
    }
}

std::vector<wxString> clFileFinder::SplitIntoLines(const wxString& content) const
{
    std::vector<wxString> lines;
    wxString current_line;
    bool prev_was_cr = false;

    for (const wxUniChar& ch : content) {
        if (ch == '\r') {
            lines.push_back(current_line);
            current_line.Clear();
            prev_was_cr = true;
        } else if (ch == '\n') {
            if (!prev_was_cr) {
                lines.push_back(current_line);
                current_line.Clear();
            }
            prev_was_cr = false;
        } else {
            current_line += ch;
            prev_was_cr = false;
        }
    }

    // Don't forget the last line if file doesn't end with newline
    if (!current_line.IsEmpty() || !prev_was_cr) {
        lines.push_back(current_line);
    }

    return lines;
}

bool clFileFinder::IsWordCharacter(wxUniChar ch) const { return ch == '_' || ::wxIsalnum(ch); }

bool clFileFinder::IsWordBoundary(const wxString& str, size_t pos) const
{
    // Beginning of string is a word boundary if next char is a word character
    if (pos == 0) {
        return str.IsEmpty() || IsWordCharacter(str[0]);
    }

    // End of string is a word boundary if previous char is a word character
    if (pos >= str.length()) {
        return !str.IsEmpty() && IsWordCharacter(str[str.length() - 1]);
    }

    // Word boundary exists between a word char and a non-word char
    bool prev_is_word = IsWordCharacter(str[pos - 1]);
    bool curr_is_word = IsWordCharacter(str[pos]);

    return prev_is_word != curr_is_word;
}

bool clFileFinder::CaseInsensitiveMatch(const wxString& text, size_t pos, const wxString& pattern) const
{
    if (pos + pattern.length() > text.length()) {
        return false;
    }

    for (size_t i = 0; i < pattern.length(); ++i) {
        wxUniChar text_char = text[pos + i];
        wxUniChar pattern_char = pattern[i];

        // Use case folding for comparison
        if (text_char.IsAscii() && pattern_char.IsAscii()) {
            // Fast path for ASCII characters
            if (wxTolower(static_cast<wxUniChar::value_type>(text_char)) !=
                wxTolower(static_cast<wxUniChar::value_type>(pattern_char))) {
                return false;
            }
        } else {
            // Unicode case-insensitive comparison using case folding
            wxString text_folded = wxString(text_char).Lower();
            wxString pattern_folded = wxString(pattern_char).Lower();
            if (text_folded != pattern_folded) {
                return false;
            }
        }
    }

    return true;
}

size_t
clFileFinder::FindPattern(const wxString& text, const wxString& pattern, size_t start_pos, bool case_sensitive) const
{
    if (pattern.IsEmpty() || start_pos >= text.length()) {
        return wxString::npos;
    }

    if (case_sensitive) {
        return text.find(pattern, start_pos);
    }

    // Case-insensitive search
    size_t text_len = text.length();
    size_t pattern_len = pattern.length();

    for (size_t i = start_pos; i + pattern_len <= text_len; ++i) {
        if (CaseInsensitiveMatch(text, i, pattern)) {
            return i;
        }
    }

    return wxString::npos;
}
