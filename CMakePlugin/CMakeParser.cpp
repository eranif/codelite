//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : CMakeParser.cpp
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

/* ************************************************************************ */
/* INCLUDES                                                                 */
/* ************************************************************************ */

// Declaration
#include "CMakeParser.h"

// C++
#include <cctype>
#include <cassert>
#include <iterator>

// wxWidgets
#include <wx/ffile.h>

/* ************************************************************************ */
/* STRUCTURES                                                               */
/* ************************************************************************ */

/**
 * @brief Parse token.
 */
struct Token
{
    /// Token start position.
    wxString::size_type start;

    /// Token length.
    wxString::size_type length;

    /// Token type.
    enum {
        TypeUnknown = 0,
        TypeIdentifier,
        TypeString,
        TypeLeftParen,
        TypeRightParen,
        TypeComment,
        TypeSpace,
        TypeVariable
    } type;

    /// String value.
    wxString value;
};

/* ************************************************************************ */

/**
 * @brief Iterator pair.
 */
struct IteratorPair
{

// Public Data Members
public:

    /// Start position.
    wxString::const_iterator start;

    /// Current position.
    wxString::const_iterator current;

    /// Input end position.
    wxString::const_iterator end;


// Public Ctors
public:


    /**
     * @brief Creates an iterator pair.
     *
     * @param beg Source begin.
     * @param end Source end.
     */
    IteratorPair(wxString::const_iterator beg, wxString::const_iterator end)
        : start(beg), current(beg), end(end)
    {}


// Public Accessors
public:


    /**
     * @brief Returns position of the current character from the start
     * of the source.
     *
     * @return Offset
     */
    wxString::size_type GetOffset() const {
        return std::distance(start, current);
    }


    /**
     * @brief Returns current character.
     *
     * @return
     */
    wxUniChar Get() const {
        return *current;
    }


    /**
     * @brief Increment position iterator.
     *
     * @return this
     */
    IteratorPair& Next() {
        ++current;
        return *this;
    }


    /**
     * @brief Check if end of the input is reached.
     *
     * @return
     */
    bool IsEof() const {
        return current == end;
    }


    /**
     * @brief Test if current character match given character.
     *
     * @param ch Tested character.
     *
     * @return
     */
    bool Is(wxUniChar ch) const {
        return Get() == ch;
    }


    /**
     * @brief Test if current character is in given range.
     *
     * @param ch1
     * @param ch2
     *
     * @return
     */
    bool IsRange(wxUniChar ch1, wxUniChar ch2) const {
        return Get() >= ch1 && Get() <= ch2;
    }


    /**
     * @brief Checks if current character is alpha (a-z).
     *
     * @return
     */
    bool IsAlpha() const {
        return IsRange('a', 'z') || IsRange('A', 'Z');
    }


    /**
     * @brief Checks if current character is alphanumeric (a-z|0-9).
     *
     * @return
     */
    bool IsAlphaNumberic() const {
        return IsAlpha() || IsRange('0', '9');
    }


    /**
     * @brief Checks if current character identifier (a-z|0-9|_).
     *
     * @return
     */
    bool IsIdentifier() const {
        return IsAlphaNumberic() || Is('_');
    }

};

/* ************************************************************************ */
/* FUNCTIONS                                                                */
/* ************************************************************************ */

/**
 * @brief Parses a token from input stream.
 *
 * @param context Parsing context.
 * @param token   Output token.
 */
static void GetToken(IteratorPair& context, Token& token)
{
    // EOS
    if (context.IsEof())
        return;

    token.type = Token::TypeUnknown;
    token.value.clear();
    token.start = context.GetOffset();

    if (context.Is('#')) {

        // COMMENT
        token.type = Token::TypeComment;

        // Everything until EOL is part of the comment
        while (!context.IsEof() && !context.Is('\n')) {
            token.value += context.Get();
            context.Next();
        }

        context.Next();

    } else if (context.Is('(')) {

        // LEFT PARENTHESIS
        token.type = Token::TypeLeftParen;
        token.value += context.Get();
        context.Next();

    } else if (context.Is(')')) {

        // RIGHT PARENTHESIS
        token.type = Token::TypeRightParen;
        token.value += context.Get();
        context.Next();

    } else if (context.Is(' ') || context.Is('\n') || context.Is('\t')) {

        // WHITESPACE
        token.type = Token::TypeSpace;
        token.value += context.Get();
        context.Next();

    } else if (context.IsAlpha()) {

        token.type = Token::TypeIdentifier;
        token.value += context.Get();
        context.Next();

        // Rest of the identifier
        while (!context.IsEof() && context.IsIdentifier()) {
            token.value += context.Get();
            context.Next();
        }

    } else if (context.Is('$')) {

        // VARIABLE
        token.value += context.Get();
        context.Next();

        if (context.Is('{')) {

            token.type = Token::TypeVariable;
            token.value += context.Get();
            context.Next();

            // Rest of the identifier
            while (!context.IsEof() && context.IsIdentifier()) {
                token.value += context.Get();
                context.Next();
            }

            // TODO Check }
            token.value += context.Get();
            context.Next();

        } else if (context.Is('(')) {

            token.type = Token::TypeVariable;
            token.value += context.Get();
            context.Next();

            // Rest of the identifier
            while (!context.IsEof() && context.IsIdentifier()) {
                token.value += context.Get();
                context.Next();
            }

            // TODO Check )
            token.value += context.Get();
            context.Next();
        }

    } else {

        token.value += context.Get();
        context.Next();

    }

    // Calculate token size
    token.length = context.GetOffset() - token.start;
}

/* ************************************************************************ */

/**
 * @brief Parses command.
 *
 * @param context Parse context.
 * @param command Output variable.
 * @param errors  Container for errors.
 */
static bool ParseCommand(IteratorPair& context, CMakeParser::Command& command,
                         wxVector<CMakeParser::Error>& errors)
{
    command.pos = 0;
    command.name.clear();
    command.arguments.clear();

    Token token;

    // Skip spaces and find identifier (command name)
    for (GetToken(context, token); !context.IsEof(); GetToken(context, token)) {
        // Identifier found
        if (token.type == Token::TypeIdentifier) {
            break;
        }
    }

    // Done
    if (context.IsEof())
        return false;

    // Must be an identifier
    assert(token.type == Token::TypeIdentifier);

    // Store command name
    command.name = token.value;
    command.pos = token.start;

    // Skip spaces and find open parenthessis
    for (GetToken(context, token); !context.IsEof(); GetToken(context, token)) {
        // Identifier found
        if (token.type == Token::TypeLeftParen) {
            break;
        } else if (token.type == Token::TypeSpace) {
            continue;
        } else {
            // Expected open parenthesis
            CMakeParser::Error error = {token.start, CMakeParser::ErrorUnexpectedToken};
            errors.push_back(error);

            // Don't stop parsing, just find the parenthesis
        }
    }

    // Unexpected EOF.
    if (context.IsEof())
        // TODO add error?
        return false;

    // Must be a '('
    assert(token.type == Token::TypeLeftParen);

    // Parse next token
    GetToken(context, token);

    // Command have arguments
    if (token.type != Token::TypeRightParen) {

        wxString arg;

        // Read tokens
        for (; !context.IsEof(); GetToken(context, token)) {
            // End of arguments
            if (token.type == Token::TypeRightParen) {
                break;
            }

            // Next argument
            if (token.type == Token::TypeSpace) {

                // Store argument
                if (!arg.IsEmpty())
                    command.arguments.push_back(arg);

                arg.Clear();
                continue;
            }

            // Add token value
            arg += token.value;
        }

        // Store last argument
        if (!arg.IsEmpty())
            command.arguments.push_back(arg);

    }

    // Command must ends with close paren
    return (token.type == Token::TypeRightParen);
}

/* ************************************************************************ */
/* CLASSES                                                                  */
/* ************************************************************************ */

CMakeParser::CMakeParser()
{
    // Nothing to do
}

/* ************************************************************************ */

void
CMakeParser::Clear()
{
    m_filename.Clear();
    m_commands.clear();
    m_errors.clear();
}

/* ************************************************************************ */

bool
CMakeParser::Parse(const wxString& content)
{
    // Clear everything
    Clear();

    Command command;
    IteratorPair context(content.begin(), content.end());

    // Parse input into tokens
    while (ParseCommand(context, command, m_errors)) {

        // If command is 'set', store variable info
        if (command.name == "set") {
            if (!command.arguments.IsEmpty()) {
                m_variables.insert(command.arguments[0]);
            } else {
                Error error = {command.pos, ErrorSetMissingArguments};
                m_errors.push_back(error);
            }
        }

        // Add command
        m_commands.push_back(command);
    }

    return true;
}

/* ************************************************************************ */

bool
CMakeParser::ParseFile(const wxFileName& filename)
{
    m_filename = filename;

    // Open file
    wxFFile file(m_filename.GetFullPath());

    // File cannot be opened
    if (!file.IsOpened())
        return false;

    // Read file content
    wxString content;
    file.ReadAll(&content);

    return Parse(content);
}

/* ************************************************************************ */

wxString
CMakeParser::GetError(ErrorCode code)
{
    // Error codes are linear and we can use an array.
    static wxString s_strings[ErrorCount] = {
        "Common error",
        "Unexpected token",
        "Missing arguments for SET command"
    };

    return s_strings[code];
}

/* ************************************************************************ */
