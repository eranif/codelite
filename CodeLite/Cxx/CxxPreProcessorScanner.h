//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxPreProcessorScanner.h
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

#ifndef CXXPREPROCESSORSCANNER_H
#define CXXPREPROCESSORSCANNER_H

#include "CxxLexerAPI.h"
#include "codelite_exports.h"
#include <list>
#include <unordered_set>
#include <wx/sharedptr.h>
#include <wx/string.h>

class CxxPreProcessor;
class WXDLLIMPEXP_CL CxxPreProcessorScanner
{
protected:
    Scanner_t m_scanner;
    wxFileName m_filename;
    size_t m_options;
    std::unordered_set<wxString>& m_visitedFiles;

public:
    typedef wxSharedPtr<CxxPreProcessorScanner> Ptr_t;

private:
    /**
     * @brief run the scanner until we reach the closing #endif
     * directive
     * Note that this function consumes the 'endif' directive
     */
    bool ConsumeBlock();
    /**
     * @brief run the scanner until it reaches the next pre processor branch
     * or until it finds the closing #endif directive
     * Note that this function also consumes the directive 'endif'
     * but will _not_ consume the elif/else directives
     */
    bool ConsumeCurrentBranch();
    /**
     * @brief read the next token that matches 'type'
     * If we reached the end of the 'PreProcessor' state and there is no match
     * throw an exception
     */
    void ReadUntilMatch(int type, CxxLexerToken& token);

    void GetRestOfPPLine(wxString& rest, bool collectNumberOnly = false);
    bool CheckIfDefined(const CxxPreProcessorToken::Map_t& table);
    bool CheckIf(const CxxPreProcessorToken::Map_t& table);
    bool IsTokenExists(const CxxPreProcessorToken::Map_t& table, const CxxLexerToken& token);

public:
    CxxPreProcessorScanner(const wxFileName& file, size_t options, std::unordered_set<wxString>& visitedFiles);

    /**
     * @brief return true if we got a valid scanner
     */
    bool IsNull() const
    {
        return m_scanner == NULL;
        ;
    }

    virtual ~CxxPreProcessorScanner();

    /**
     * @brief the main parsing function
     * @param ppTable
     */
    void Parse(CxxPreProcessor* pp);
};

#endif // CXXPREPROCESSORSCANNER_H
