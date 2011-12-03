#if HAS_LIBCLANG

#include <wx/app.h>
#include "clang_pch_maker_thread.h"
#include <wx/thread.h>
#include <wx/stdpaths.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "y.tab.h"
#include "cpp_scanner.h"
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "fileextmanager.h"
#include <wx/xrc/xmlres.h>

#define cstr(x) x.mb_str(wxConvUTF8).data()

const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED = XRCID("clang_pch_cache_started");
const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED   = XRCID("clang_pch_cache_ended");

extern const wxEventType wxEVT_UPDATE_STATUS_BAR;

////////////////////////////////////////////////////////////////////////////
// Internal class used for traversing the macro found in a translation UNIT

struct MacroClientData {
	std::set<wxString> macros;
	std::set<wxString> interestingMacros;
	wxString           filename;

	wxString intersect() const {
		std::set<wxString> resultSet;
		std::set<wxString>::const_iterator iter = this->interestingMacros.begin();
		for(; iter != this->interestingMacros.end(); iter++) {
			if(this->macros.find(*iter) != this->macros.end()) {
				// this macro exists in both lists
				resultSet.insert(*iter);
			}
		}

		wxString macrosAsStr;
		std::set<wxString>::const_iterator it = resultSet.begin();
		for(; it != resultSet.end(); it++) {
			macrosAsStr << (*it) << wxT(" ");
		}
		return macrosAsStr;
	}
};

enum CXChildVisitResult MacrosCallback(CXCursor cursor,
                                       CXCursor parent,
                                       CXClientData clientData)
{
	// Get all macros here...
	if(cursor.kind == CXCursor_MacroDefinition) {

		// Dont collect macro defined in this file
		CXSourceLocation loc = clang_getCursorLocation(cursor);
		CXFile file;
		unsigned line, col, off;

		clang_getSpellingLocation(loc, &file, &line, &col, &off);
		CXString strFileName = clang_getFileName(file);
		wxFileName fn(wxString(clang_getCString(strFileName), wxConvUTF8));
		clang_disposeString(strFileName);
		MacroClientData *cd = (MacroClientData*)clientData;

		if(cd->filename != fn.GetFullPath()) {
			CXString displayName = clang_getCursorDisplayName(cursor);
			cd->macros.insert(wxString(clang_getCString(displayName), wxConvUTF8));
			clang_disposeString(displayName);
		}

	}
	return CXChildVisit_Continue;
}

static void printDiagnosticsToLog(CXTranslationUnit TU)
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

static void printCompletionDiagnostics(CXCodeCompleteResults *res)
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

ClangWorkerThread::ClangWorkerThread()
{
}

ClangWorkerThread::~ClangWorkerThread()
{
}

void ClangWorkerThread::ProcessRequest(ThreadRequest* request)
{
	ClangThreadRequest *task = dynamic_cast<ClangThreadRequest*>( request );
	if( !task ) {
		wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
		eEnd.SetClientData(NULL);
		wxTheApp->AddPendingEvent(eEnd);
		return;
	}

	// Send start event
	wxCommandEvent e(wxEVT_CLANG_PCH_CACHE_STARTED);
	wxTheApp->AddPendingEvent(e);


	FileExtManager::FileType type = FileExtManager::GetType(task->GetFileName());
	bool isSource = (type == FileExtManager::TypeSourceC || type == FileExtManager::TypeSourceCpp);

	std::string c_dirtyBuffer = cstr(task->GetDirtyBuffer());
	std::string c_filename    = cstr(task->GetFileName());

	CXUnsavedFile unsavedFile = { c_filename.c_str(), c_dirtyBuffer.c_str(), c_dirtyBuffer.length() };
	CXTranslationUnit TU = findEntry(task->GetFileName());

	bool reparseRequired = true;
	if(!TU) {

		wxFileName fn(task->GetFileName());
		DoSetStatusMsg(wxString::Format(wxT("clang: parsing file %s..."), fn.GetFullName().c_str()));
		
		int argc(0);
		char **argv = MakeCommandLine(task->GetCompilationArgs(), argc, !isSource);

		for(int i=0; i<argc; i++) {
			CL_DEBUG(wxT("Command Line Argument: %s"), wxString(argv[i], wxConvUTF8).c_str());
		}

		CL_DEBUG(wxT("Calling clang_parseTranslationUnit..."));
		// First time, need to create it
		TU = clang_parseTranslationUnit(task->GetIndex(),
		                                c_filename.c_str(),
		                                argv,
		                                argc,
		                                NULL, 0, CXTranslationUnit_CXXPrecompiledPreamble
		                                | CXTranslationUnit_CacheCompletionResults
		                                | CXTranslationUnit_CXXChainedPCH
		                                | CXTranslationUnit_PrecompiledPreamble
		                                | CXTranslationUnit_Incomplete
		                                | CXTranslationUnit_DetailedPreprocessingRecord);

		CL_DEBUG(wxT("Calling clang_parseTranslationUnit... done"));
		for(int i=0; i<argc; i++) {
			free(argv[i]);
		}
		delete [] argv;

		if(TU) {
			reparseRequired = false;
			CL_DEBUG(wxT("Calling clang_reparseTranslationUnit..."));
			clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU));
			CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... done"));

		} else {

			DoSetStatusMsg(wxT("Ready"));

			CL_DEBUG(wxT("Failed to parse Translation UNIT..."));
			wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
			eEnd.SetClientData(NULL);
			wxTheApp->AddPendingEvent(eEnd);
			return;
		}
	}

	
	// Construct a cache-returner class
	// which makes sure that the TU is cached
	// when we leave the current scope
	CacheReturner cr(this, task->GetFileName(), TU);

	if(reparseRequired && task->GetContext() == ::CTX_ReparseTU) {
		DoSetStatusMsg(wxString::Format(wxT("clang: re-parsing file %s..."), task->GetFileName().c_str()));

		// We need to reparse the TU
		CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... [CTX_ReparseTU]"));
		clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU));
		CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... done [CTX_ReparseTU]"));
	}

	DoSetStatusMsg(wxT("Ready"));
	
	// Prepare the 'End' event
	wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
	ClangThreadReply *reply = new ClangThreadReply;
	reply->context    = task->GetContext();
	reply->filterWord = task->GetFilterWord();
	reply->filename   = task->GetFileName().c_str();
	reply->results    = NULL;

	if( task->GetContext() == CTX_CodeCompletion ||
	    task->GetContext() == CTX_WordCompletion ||
	    task->GetContext() == CTX_Calltip) {
#if 0
		//CL_DEBUG(wxT("Calling clang_reparseTranslationUnit..."));
		//clang_reparseTranslationUnit(TU, 0, NULL, clang_defaultReparseOptions(TU));
		//CL_DEBUG(wxT("Calling clang_reparseTranslationUnit... done"));
#endif
		CL_DEBUG(wxT("Calling clang_codeCompleteAt..."));
		CL_DEBUG(wxT("Location: %s:%u:%u"), task->GetFileName().c_str(), task->GetLine(), task->GetColumn());
		// Do the code-completion
		reply->results = clang_codeCompleteAt(TU,
		                                      cstr(task->GetFileName()),
		                                      task->GetLine(),
		                                      task->GetColumn(),
		                                      &unsavedFile,
		                                      1,
		                                      clang_defaultCodeCompleteOptions());

		CL_DEBUG(wxT("Calling clang_codeCompleteAt... done"));
		wxString displayTip;
		bool hasErrors(false);
		if(reply->results) {
			unsigned maxErrorToDisplay = 10;
			std::set<wxString> errorMessages;
			unsigned errorCount = clang_codeCompleteGetNumDiagnostics(reply->results);
			// Collect all errors / fatal errors and report them back to user
			for(unsigned i=0; i<errorCount; i++) {
				CXDiagnostic         diag     = clang_codeCompleteGetDiagnostic(reply->results, i);
				CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diag);
				if(!hasErrors) {
					hasErrors = (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal);
				}
				
				if(severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal || severity == CXDiagnostic_Note) {
					CXString diagStr      = clang_getDiagnosticSpelling(diag);
					wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);
					
					// Collect up to 10 error messages
					// and dont collect the same error twice
					if(errorMessages.find(wxDiagString) == errorMessages.end() && errorMessages.size() <= maxErrorToDisplay) {
						errorMessages.insert(wxDiagString);
						displayTip << wxDiagString.c_str() << wxT("\n");
					}
					
					clang_disposeString(diagStr);
				}
				clang_disposeDiagnostic(diag);
			}

			CL_DEBUG(wxT("Found %u matches"), reply->results->NumResults);
			printCompletionDiagnostics(reply->results);
		}
		
		if(!displayTip.IsEmpty() && hasErrors) {
			
			// Send back the error messages
			reply->errorMessage << wxT("Code Completion Error\n@@LINE@@\n") << displayTip;
			
			// Free the results
			clang_disposeCodeCompleteResults(reply->results);
			reply->results = NULL;
			reply->errorMessage.RemoveLast();
		}

	} else if(task->GetContext() == CTX_Macros) {

		MacroClientData clientData;
		clientData.filename = reply->filename;

		CL_DEBUG(wxT("Traversing TU..."));
		CXCursorVisitor visitor = MacrosCallback;
		clang_visitChildren(clang_getTranslationUnitCursor(TU), visitor, (CXClientData)&clientData);

		clientData.interestingMacros = DoGetUsedMacros(reply->filename);


		CL_DEBUG(wxT("Traversing TU...done"));
		CL_DEBUG(wxT("Collected %u Macros"), (unsigned int)clientData.macros.size());
		CL_DEBUG(wxT("Collected %u Interesting Macros"), (unsigned int)clientData.interestingMacros.size());

		wxString macros = clientData.intersect();
		CL_DEBUG(wxT("The following macros will be passed to scintilla: %s"), macros.c_str());
		reply->macrosAsString = macros.c_str(); // Make sure we create a new copy and not using ref-count
	}

	eEnd.SetClientData(reply);
	wxTheApp->AddPendingEvent(eEnd);
}

std::set<wxString> ClangWorkerThread::DoGetUsedMacros(const wxString &filename)
{
	std::set<wxString> pps;
	static wxRegEx reMacro(wxT("#[ \t]*((if)|(elif)|(ifdef)|(ifndef))[ \t]*"));
	wxString fileContent;
	if(!ReadFileWithConversion(filename, fileContent)) {
		return pps;
	}

	CppScannerPtr scanner(new CppScanner());
	wxArrayString lines = wxStringTokenize(fileContent, wxT("\r\n"));
	for(size_t i=0; i<lines.GetCount(); i++) {
		wxString line = lines.Item(i).Trim(false);
		if(line.StartsWith(wxT("#")) && reMacro.IsValid() && reMacro.Matches(line)) {
			// Macro line
			wxString match = reMacro.GetMatch(line, 0);
			wxString ppLine = line.Mid(match.Len());

			scanner->Reset();
			scanner->SetText(cstr(ppLine));
			int type(0);
			while( (type = scanner->yylex()) != 0 ) {
				if(type == IDENTIFIER) {
					wxString intMacro = wxString(scanner->YYText(), wxConvUTF8);
					CL_DEBUG1(wxT("Found interesting macro: %s"), intMacro.c_str());
					pps.insert(intMacro);
				}
			}
		}
	}

	return pps;
}

CXTranslationUnit ClangWorkerThread::findEntry(const wxString& filename)
{
	wxCriticalSectionLocker locker(m_cs);
	CXTranslationUnit TU = m_cache.GetPCH(filename);
	return TU;
}

void ClangWorkerThread::DoCacheResult(CXTranslationUnit TU, const wxString &filename)
{
	wxCriticalSectionLocker locker(m_cs);
	m_cache.AddPCH( filename, TU);
	CL_DEBUG(wxT("caching Translation Unit file: %s"), filename.c_str());
	CL_DEBUG(wxT(" ==========> [ ClangPchMakerThread ] PCH creation ended successfully <=============="));
}

void ClangWorkerThread::ClearCache()
{
	wxCriticalSectionLocker locker(m_cs);
	m_cache.Clear();
	this->DoSetStatusMsg(wxT("clang: cache cleared"));
}

bool ClangWorkerThread::IsCacheEmpty()
{
	wxCriticalSectionLocker locker(m_cs);
	return m_cache.IsEmpty();
}

char** ClangWorkerThread::MakeCommandLine(const wxString& command, int& argc, bool isHeader)
{
	wxArrayString tokens = wxStringTokenize(command, wxT(" \t\n\r"), wxTOKEN_STRTOK);
	if(isHeader) {
		tokens.Add(wxT("-x"));
		tokens.Add(wxT("c++-header"));
	}
	tokens.Add(wxT("-w"));
	tokens.Add(wxT("-ferror-limit=1000"));
	argc = tokens.GetCount();
	char** argv = new char*[argc];

	if(tokens.IsEmpty())
		return argv;

	for(size_t i=0; i<tokens.GetCount(); i++) {
		argv[i] = strdup(tokens.Item(i).mb_str(wxConvUTF8).data());
	}
	return argv;
}

void ClangWorkerThread::DoSetStatusMsg(const wxString& msg)
{
	wxCommandEvent e(wxEVT_UPDATE_STATUS_BAR);
	e.SetString(msg.c_str());
	e.SetInt(0);
	e.SetId(10);
	wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(e);
}

#endif // HAS_LIBCLANG
