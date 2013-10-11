//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_utils.h
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

#ifndef CLANGUTILS_H
#define CLANGUTILS_H

#if HAS_LIBCLANG

#include "precompiled_header.h"
#include <clang-c/Index.h>
#include <wx/string.h>
#include "clangpch_cache.h"

class ClangUtils 
{
public:
	static CXTranslationUnit LoadTU(CXIndex index, const ClangCacheEntry &entry);
	/**
	 * @brief return the location of the current cursor
	 * @param cursor   [input]
	 * @param filename [output]
	 * @param line     [output]
	 * @param col      [output]
	 */
	static void GetCursorLocation(CXCursor cursor, wxString &filename, unsigned &line, unsigned &col);
	
	/**
	 * @brief get the cursor at a given location (filename:line:col)
	 */
	static bool GetCursorAt(CXTranslationUnit &unit, const wxString &filename, int line, int col, CXCursor &cur);
	
	/**
	 * @brief print diagostics to codelite's log file
	 * @param TU
	 */
	static void printDiagnosticsToLog(CXTranslationUnit &TU);
	
	/**
	 * @brief print code completion's diagnostics to codelite's log file
	 */
	static void printCompletionDiagnostics(CXCodeCompleteResults *res);
    
    static char** MakeArgv(const wxArrayString &arr, int &argc);
    static void FreeArgv(char **argv, int argc);
    
    /**
     * @brief make PCH if needed based on the command line arguments provided in the 'tokens' array
     * This function will create the PCH if one of the tokens is '-include-pch'
     */
    static void MakePCHIfNeeded(const wxArrayString &tokens, const CXIndex &index);
};

#endif // HAS_LIBCLANG

#endif // CLANGUTILS_H
