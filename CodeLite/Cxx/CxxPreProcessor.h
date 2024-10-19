//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxPreProcessor.h
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

#ifndef CXXPREPROCESSOR_H
#define CXXPREPROCESSOR_H

#include "CxxLexerAPI.h"
#include "CxxPreProcessorScanner.h"
#include "codelite_exports.h"
#include <set>
#include <wx/filename.h>

class WXDLLIMPEXP_CL CxxPreProcessor
{
    CxxPreProcessorToken::Map_t m_tokens;
    wxArrayString m_includePaths;
    std::set<wxString> m_noSuchFiles;
    std::unordered_map<wxString, wxString> m_fileMapping;
    size_t m_options;

public:
    CxxPreProcessor();
    virtual ~CxxPreProcessor();

    void SetOptions(size_t options) { this->m_options = options; }
    size_t GetOptions() const { return m_options; }

    CxxPreProcessorToken::Map_t& GetTokens() { return m_tokens; }

    const CxxPreProcessorToken::Map_t& GetTokens() const { return m_tokens; }
    void SetTokens(const CxxPreProcessorToken::Map_t& tokens) { m_tokens = tokens; }

    /**
     * @brief resolve an include statement and return the full path for it
     * @param currentFile the current file being processed. ( we use it's path to resolve relative include
     * statements)
     * @param includeStatement the include statement found by the scanner
     * @param outFile [output]
     */
    bool ExpandInclude(const wxFileName& currentFile, const wxString& includeStatement, wxFileName& outFile);
    /**
     * @brief the main entry function
     * @param filename
     */
    void Parse(const wxFileName& filename, size_t options);

    /**
     * @brief return the definitions collected as an array
     * @return
     */
    wxArrayString GetDefinitions() const;
};

#endif // CXXPREPROCESSOR_H
