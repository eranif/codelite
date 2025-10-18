//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : StringUtils.h
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
#pragma once

#include "AsyncProcess/asyncprocess.h"
#include "codelite_exports.h"

#include <sstream>
#include <string>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/string.h>

using wxStringView = std::basic_string_view<wxChar>;

inline bool operator==(wxStringView sv, const wxString& s)
{
    return s.length() == sv.length() && ::wxStrncmp(s, sv.data(), sv.length()) == 0;
}

inline bool operator==(wxStringView sv, const char* cstr)
{
    return sv.length() == ::wxStrlen(cstr) && ::wxStrncmp(cstr, sv.data(), sv.length()) == 0;
}

class WXDLLIMPEXP_CL StringUtils
{
public:
    enum BuildCommandFlags {
        // build a command using only space as the delimiter
        ONE_LINER = (1 << 0),

        // include a comment block before the command
        WITH_COMMENT_PREFIX = (1 << 1),
    };

public:
    /**
     * @brief convert string into std::string
     */
    static std::string ToStdString(const wxString& str);

    /**
     * @brief convert string to integer using range validation and default value
     */
    static int wxStringToInt(const wxString& str, int defval, int min = -1, int max = -1);

    /**
     * @brief convert integer to string
     */
    static wxString wxIntToString(int val);

    static unsigned int UTF8Length(const wchar_t* uptr, unsigned int tlen);

    /**
     * @brief remove terminal colours from buffer
     */
    static void StripTerminalColouring(const std::string& buffer, std::string& modbuffer);
    /**
     * @brief wxString version.
     * @param buffer
     * @param modbuffer
     */
    static void StripTerminalColouring(const wxString& buffer, wxString& modbuffer);

    /**
     * @brief strip ANSI OSC ("Operating System Command sequences")sequence from `buffer`
     */
    static wxString StripTerminalOSC(const wxString& buffer);
    static wxString StripTerminalOSC(wxStringView buffer);

    /**
     * @brief add backslash to markdown styling characters
     */
    static void DisableMarkdownStyling(wxString& buffer);

    /**
     * @brief decode URI using percent encoding
     */
    static wxString DecodeURI(const wxString& uri);

    /**
     * @brief encode URI using percent encoding
     */
    static wxString EncodeURI(const wxString& uri);

    /**
     * @brief an efficient way to tokenize string into words (separated by SPACE and/or TAB)
     * @code
     * wxString str = "My String That Requires Tokenize";
     * wxString word; // The output
     * size_t offset = 0;
     * while (StringUtils::NextWord(str, offset, word)) {
     *      // Do something with "word" here
     * }
     * @codeend
     * @param str the string to tokenize
     * @param offset used internally, allocate one on the stack and initialise it to 0
     * @param word [output]
     * @return true if a word was found
     */
    static bool NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower = false);

    /**
     * @brief Joins elements of a container into a single string with a glue separator.
     *
     * @tparam Container The type of the container containing elements to join
     * @param c The container of elements to join
     * @param glue The string to use as separator between elements (default is newline)
     * @return wxString The joined string with all elements separated by the glue
     */
    template <typename Container>
    static wxString Join(const Container& c, const wxString& glue = "\n")
    {
        wxString output;
        for (const auto& ele : c) {
            output << ele << glue;
        }

        if (!output.empty()) {
            output.RemoveLast(glue.length());
        }
        return output;
    }

    /**
     * @brief join strings with `\n` or `\r\n` (depends on eol)
     * eol can be wxSTC_EOL_CRLF, wxSTC_EOL_LF etc
     */
    static wxString clJoinLinesWithEOL(const wxArrayString& lines, int eol);

    /**
     * @brief joins array element into a string using 'glue' as the array elements
     * separator
     */
    static wxString wxImplode(const wxArrayString& arr, const wxString& glue = "\n");

    /**
     * @brief split lines (using CR|LF as the separator), taking into consideration line continuation
     * @param trim trim the lines with set to true
     */
    static wxArrayString SplitString(const wxString& inString, bool trim = true);

    /**
     * @brief build argv out of str
     */
    static char** BuildArgv(const wxString& str, int& argc);

    /**
     * @brief build argv out of str
     */
    static wxArrayString BuildArgv(const wxString& str);

    /**
     * @brief free argv created by StringUtils::BuildArgv method
     */
    static void FreeArgv(char** argv, int argc);

    /**
     * @brief given environment string in form of: `a=b;c=d` construct a clEnvList_t
     */
    static clEnvList_t BuildEnvFromString(const wxString& envstr);

    /**
     * @brief using the current environment variables, resolve the list
     */
    static clEnvList_t ResolveEnvList(const clEnvList_t& env_list);
    static clEnvList_t ResolveEnvList(const wxString& envstr);

    /**
     * @brief accept command as input string and break it into array of commands
     * the input string can span on multiple lines or use space as the delimiter
     * (or both)
     * Empty lines and lines starting with "#" are ignored
     */
    static wxArrayString BuildCommandArrayFromString(const wxString& command);

    /**
     * @brief build command string from array of commands
     * @param command the command array
     * @param flags see `StringUtils::BuildCommandFlags`
     */
    static wxString BuildCommandStringFromArray(const wxArrayString& command_arr,
                                                size_t flags = BuildCommandFlags::ONE_LINER);

    /**
     * @brief wrap string with quotes if needed
     */
    static wxString& WrapWithQuotes(wxString& str);

    /// If string contains space, wrap it with double quotes
    [[nodiscard]] static wxString WrapWithDoubleQuotes(const wxString& str);

    /// If a string is wrapped with double quotes -> strip them
    [[nodiscard]] static wxString StripDoubleQuotes(const wxString& str);

    /// Append `str` to `arr`. If the array size exceed the truncation size, shrink it to fit
    static wxArrayString AppendAndMakeUnique(const wxArrayString& arr, const wxString& str, size_t truncate_size = 15);

    /// Given `Container` create an array with unique entries
    template <typename Container>
    static wxArrayString MakeUniqueArray(const Container& container)
    {
        wxArrayString arr;
        std::unordered_set<wxString> unique_set;
        arr.reserve(container.size());
        for (const auto& str : container) {
            if (unique_set.insert(str).second) {
                arr.Add(str);
            }
        }
        return arr;
    }
};
inline wxString BoolToString(bool b) { return b ? wxT("yes") : wxT("no"); }
