#include "clang_utils.h"
#include <wx/string.h>

void ClangUtils::printDiagnosticsToLog(CXTranslationUnit& TU)
{
	//// Report diagnostics to the log file
	const unsigned diagCount = clang_getNumDiagnostics(TU);
	for(unsigned i=0; i<diagCount; i++) {
		CXDiagnostic diag     = clang_getDiagnostic(TU, i);
		CXString diagStr      = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
        wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);
        
		if(!wxDiagString.Contains(wxT("'dllimport' attribute"))) {
			fprintf(stdout, "Diagnostic: %s\n", clang_getCString(diagStr));
        
		}
		clang_disposeString(diagStr);
		clang_disposeDiagnostic(diag);
	}
}
