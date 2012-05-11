#ifndef CLANGUTILS_H
#define CLANGUTILS_H

#if HAS_LIBCLANG

#include "precompiled_header.h"
#include <clang-c/Index.h>
#include <wx/string.h>

class ClangUtils 
{
public:
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
};

#endif // HAS_LIBCLANG

#endif // CLANGUTILS_H
