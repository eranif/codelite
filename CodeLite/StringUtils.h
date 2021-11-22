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
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL StringUtils
{
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
     * @brief add backslash to markdown styling characters
     */
    static void DisableMarkdownStyling(wxString& buffer);

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
};

#endif // STRINGUTILS_H
