//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : StdToWX.h
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

#ifndef STDTOWX_H
#define STDTOWX_H

#include "codelite_exports.h"

#include <string>
#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL StdToWX
{
public:
    /**
     * @brief remove from str string from offset 'from' with len
     */
    static void Remove(std::string& str, size_t from, size_t len);

    /**
     * @brief return true if str starts with what
     */
    static bool StartsWith(const std::string& str, const std::string& what);

    /**
     * @brief return true if str ends with what
     */
    static bool EndsWith(const std::string& str, const std::string& what);

    /**
     * @brief
     */
    static void Trim(std::string& str, bool fromRight = true);

    /**
     * @brief remove last count bytes
     */
    static void RemoveLast(std::string& str, size_t count);

    /**
     * @brief convert wxArrayString into std::vector
     */
    static void ToVector(const wxArrayString& arr, std::vector<wxString>* vec);

    /**
     * @brief convert std::vector into wxArrayString
     */
    static void ToArrayString(const std::vector<wxString>& vec, wxArrayString* arr);
};

#endif // STDTOWX_H
