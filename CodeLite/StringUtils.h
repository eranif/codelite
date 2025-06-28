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

typedef std::basic_string_view<wxChar> wxStringView;

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

    /// If string contains space, wrap it with double quotes
    static wxString WrapWithDoubleQuotes(const wxString& str);

    /// If a string is wrapped with double quotes -> strip them
    static wxString StripDoubleQuotes(const wxString& str);

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
