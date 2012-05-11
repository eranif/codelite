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

#endif // #if HAS_LIBCLANG

