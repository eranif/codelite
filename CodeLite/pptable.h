//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : pptable.h
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

#ifndef PPTABLE_H
#define PPTABLE_H

#include <wx/arrstr.h>
#include <map>
#include <wx/ffile.h>
#include <vector>
#include <string>
#include <list>
#include <set>

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#define WXDLLIMPEXP_CL
#endif

#endif

struct WXDLLIMPEXP_CL CLReplacement {
    bool is_compound;
    bool is_ok;
    std::string full_pattern;
    std::string searchFor;
    std::string replaceWith;
    void construct(const std::string& pattern, const std::string& replacement);
};

typedef std::list<CLReplacement> CLReplacementList;

/**
 * @brief perform search and replace using CL pattern
 * an example:
 * pattern=wx_dynamic_cast(%0, %1)
 * replacement=dynamic_cast<%0>(%1)
 * in=wx_dynamic_cast(wxApp*, ptr)->OnInit();
 *
 * the expected result is:
 * dynamic_cast<wxApp*>(ptr)->OnInit()
 *
 * It also supports simple search and replace
 */
bool CLReplacePatternA(const std::string& in, const CLReplacement& repl, std::string& outStr);

/**
 *
 */
struct WXDLLIMPEXP_CL PPToken {
    enum { IsFunctionLike = 0x00000001, IsValid = 0x00000002, IsOverridable = 0x00000004 };

    int line;             // line where found
    wxString name;        // preprocessor name
    wxString replacement; // un processed replacement
    wxArrayString args;   // for function like macros, contains the argument's names
    size_t flags;         // PP token flags
    wxString fileName;

    PPToken()
        : line(0)
        , flags(IsOverridable)
    {
    }

    ~PPToken() {}

    void expandOnce(const wxArrayString& initList);
    void print(wxFFile& fp);
    static bool readInitList(const wxString& in, int from, wxString& initList, wxArrayString& initListArr);
    static bool
    readInitList(const std::string& in, size_t from, std::string& initList, std::vector<std::string>& initListArr);
};
#endif // PPTABLE_H
