//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CMakeParser.h
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

/* ************************************************************************ */
/*                                                                          */
/* CMakePlugin for Codelite                                                 */
/* Copyright (C) 2013 Jiří Fatka <ntsfka@gmail.com>                         */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program. If not, see <http://www.gnu.org/licenses/>.     */
/*                                                                          */
/* ************************************************************************ */

#ifndef CMAKE_PARSER_H_
#define CMAKE_PARSER_H_

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// C++
#include <set>

// wxWidgets
#include <wx/string.h>
#include <wx/vector.h>
#include <wx/filename.h>

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

/**
 * @brief CMake configuration files parser.
 */
class CMakeParser
{

    // Public Enums
public:
    /**
     * @brief Error codes.
     */
    enum ErrorCode {
        /// Common error.
        ErrorCommon = 0,

        /// Unexpected token.
        ErrorUnexpectedToken,

        /// Missing argument for SET command.
        ErrorSetMissingArguments,

        /// Number of error codes.
        ErrorCount
    };

    // Public Structures
public:
    /**
     * @brief Represents cmake command.
     */
    struct Command {
        /// Command start position.
        wxString::size_type pos;

        /// Command name.
        wxString name;

        /// Command call arguments.
        wxArrayString arguments;
    };

    /**
     * @brief Represents source error.
     */
    struct Error {
        /// Error position.
        wxString::size_type pos;

        /// Error code.
        ErrorCode code;
    };

    // Public Ctors
public:
    /**
     * @brief Constructor.
     */
    CMakeParser();

    // Public Accessors
public:
    /**
     * @brief Returns a path to the last parsed file.
     *
     * @return
     */
    const wxFileName& GetFilename() const { return m_filename; }

    /**
     * @brief Returns parsed commands.
     *
     * @return Parsed commands.
     */
    const wxVector<Command>& GetCommands() const { return m_commands; }

    /**
     * @brief Returns defined variables.
     *
     * @return Variables.
     */
    const std::set<wxString>& GetVariables() const { return m_variables; }

    // Public Operations
public:
    /**
     * @brief Clears internal variables.
     */
    void Clear();

    /**
     * @brief Parses given CMakeFileLists.txt.
     *
     * @param content File content.
     *
     * @return Result of the parsing.
     */
    bool Parse(const wxString& content);

    /**
     * @brief Parses given CMakeFileLists.txt.
     *
     * @param filename Path to the parsed CMakeFileLists.txt.
     *
     * @return Result of the parsing.
     */
    bool ParseFile(const wxFileName& filename);

    /**
     * @brief Translate error code into human readable string.
     *
     * @param code Error code.
     *
     * @return Error string.
     */
    static wxString GetError(ErrorCode code);

    // Private Data Members
private:
    /// Last parsed file.
    wxFileName m_filename;

    /// Parsed commands.
    wxVector<Command> m_commands;

    /// Defined variables.
    std::set<wxString> m_variables;

    /// Errors found in the last parsed source.
    wxVector<Error> m_errors;
};

/* ************************************************************************ */

#endif // CMAKE_PARSER_H_
