//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clang_utils.cpp
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

#if HAS_LIBCLANG

#include "clang_utils.h"
#include "file_logger.h"


void ClangUtils::GetCursorLocation(CXCursor cursor, wxString& filename, unsigned& line, unsigned& col)
{
	CXSourceLocation loc = clang_getCursorLocation(cursor);

	CXFile file;
	unsigned off;
	
	line = 1, col = 1;
	clang_getSpellingLocation(loc, &file, &line, &col, &off);

	CXString strFileName = clang_getFileName(file);
	filename = wxString(clang_getCString(strFileName), wxConvUTF8);
	clang_disposeString(strFileName);
}

bool ClangUtils::GetCursorAt(CXTranslationUnit &unit, const wxString &filename, int line, int col, CXCursor &cur)
{
	CXFile file1;
	CXCursor cursor;
	CXSourceLocation loc;
	file1  = clang_getFile    (unit, filename.mb_str(wxConvUTF8).data());
	loc    = clang_getLocation(unit, file1, line, col);
	cursor = clang_getCursor  (unit, loc);
	
	if(clang_isInvalid(cursor.kind))
		return false;

	if(cursor.kind == CXCursor_MemberRef     ||
	   cursor.kind == CXCursor_MemberRefExpr ||
	   cursor.kind == CXCursor_TypeRef       ||
	   cursor.kind == CXCursor_DeclRefExpr   ||
	   cursor.kind == CXCursor_TemplateRef   ||
	   cursor.kind == CXCursor_NamespaceRef  
	)
	{
		cursor = clang_getCursorReferenced(cursor);
	}
	cur = cursor;
	return true;
}

void ClangUtils::printDiagnosticsToLog(CXTranslationUnit& TU)
{
	//// Report diagnostics to the log file
	const unsigned diagCount = clang_getNumDiagnostics(TU);
	for(unsigned i=0; i<diagCount; i++) {
		CXDiagnostic diag     = clang_getDiagnostic(TU, i);
		CXString diagStr      = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
		wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);
		if(!wxDiagString.Contains(wxT("'dllimport' attribute"))) {
			CL_DEBUG(wxT("Diagnostic: %s"), wxDiagString.c_str());

		}
		clang_disposeString(diagStr);
		clang_disposeDiagnostic(diag);
	}
}

void ClangUtils::printCompletionDiagnostics(CXCodeCompleteResults *res)
{
	//// Report diagnostics to the log file
	const unsigned diagCount = clang_codeCompleteGetNumDiagnostics(res);
	for(unsigned i=0; i<diagCount; i++) {
		CXDiagnostic diag = clang_codeCompleteGetDiagnostic(res, i);
		CXString diagStr = clang_getDiagnosticSpelling(diag);
		wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);

		CL_DEBUG(wxT("Completion diagnostic [%d]: %s"), clang_getDiagnosticSeverity(diag), wxDiagString.c_str());
		clang_disposeString(diagStr);
		clang_disposeDiagnostic(diag);
	}
}

void ClangUtils::FreeArgv(char** argv, int argc)
{
    for(int i=0; i<argc; i++) {
        free(argv[i]);
    }
    delete [] argv;
}

char** ClangUtils::MakeArgv(const wxArrayString& arr, int& argc)
{
    argc = arr.GetCount();
    char** argv = new char*[argc];

    if(arr.IsEmpty())
        return argv;

    for(size_t i=0; i<arr.GetCount(); i++) {
        argv[i] = strdup(arr.Item(i).mb_str(wxConvUTF8).data());
    }
    return argv;
}

void ClangUtils::MakePCHIfNeeded(const wxArrayString& tokens, const CXIndex& index)
{
    // Copy the tokens
    wxArrayString mytokens;
    wxString      pchfile;
    mytokens.insert(mytokens.end(), tokens.begin(), tokens.end());
    
    int where = mytokens.Index(wxT("-include-pch"));
    if(where != wxNOT_FOUND) {
        
        int pchfileIndex = where + 1;
        if((int)mytokens.GetCount() > pchfileIndex) {
            
            pchfile = mytokens.Item(pchfileIndex);
            mytokens.RemoveAt(where, 2);
            
        } else {
            // Invalid tokens
            // Found -include-pch but not the actual pch file...
            return;
        }
        
        // At this point, pchfile contains the file that we should create for faster code-completion
        // Now we simply test to see if this file exists
        wxFileName filename(pchfile);
        wxFileName filename_h(pchfile);
        
        filename.SetExt(wxT("clang-pch"));
        
        if(filename.FileExists()) {
            return;
        }
        
        // We needed to create it...
        
    }
}

CXTranslationUnit ClangUtils::LoadTU(CXIndex index, const ClangCacheEntry& entry)
{
	if(entry.IsOk() == false)
		return NULL;
	
	return clang_createTranslationUnit(index, entry.fileTU.mb_str(wxConvUTF8).data());
}


#endif // #if HAS_LIBCLANG



