#if HAS_LIBCLANG

#include <wx/app.h>
#include "clang_pch_maker_thread.h"
#include <wx/thread.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "file_logger.h"
#include "globals.h"
#include "procutils.h"
#include "fileextmanager.h"
#include <clang-c/Index.h>
#include <wx/xrc/xmlres.h>

#define cstr(x) x.mb_str(wxConvUTF8).data()

const wxEventType wxEVT_CLANG_PCH_CACHE_STARTED = XRCID("clang_pch_cache_started");
const wxEventType wxEVT_CLANG_PCH_CACHE_ENDED   = XRCID("clang_pch_cache_ended");

static wxString PCHFileName(const wxString &filename)
{
	wxString pchfile = wxStandardPaths::Get().GetUserDataDir();
	pchfile << wxFileName::GetPathSeparator() << wxT("clang_cache") << wxFileName::GetPathSeparator() << wxFileName(filename).GetFullName() << wxT(".pch");
	return pchfile;
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

	wxCharBuffer cb = task->GetDirtyBuffer().mb_str(wxConvUTF8).data();
	CXUnsavedFile unsavedFile = { cstr(task->GetFileName()), cb.data(), cb.length() };
	CXTranslationUnit TU = findEntry(task->GetFileName());
	if(!TU) {
		int argc(0);
		char **argv = MakeCommandLine(task->GetCompilationArgs(), argc, !isSource);

		for(int i=0; i<argc; i++) {
			CL_DEBUG(wxT("Command Line Argument: %s"), wxString(argv[i], wxConvUTF8).c_str());
		}

		CL_DEBUG(wxT("Calling clang_parseTranslationUnit..."));
		// First time, need to create it
		TU = clang_parseTranslationUnit(task->GetIndex(),
		                                cstr(task->GetFileName()),
		                                argv,
		                                argc,
		                                NULL,
		                                0,
		                                clang_defaultEditingTranslationUnitOptions());

		CL_DEBUG(wxT("Calling clang_parseTranslationUnit... done"));
		for(int i=0; i<argc; i++) {
			free(argv[i]);
		}
		delete [] argv;
	}

	DoCacheResult(TU, task->GetFileName());

	wxCommandEvent eEnd(wxEVT_CLANG_PCH_CACHE_ENDED);
	ClangThreadReply *reply = new ClangThreadReply;
	reply->context    = task->GetContext();
	reply->filterWord = task->GetFilterWord();
	reply->results    = NULL;

	if(task->GetContext() != CTX_CachePCH) {
#ifndef __WXMSW__
		CL_DEBUG(wxT("Calling clang_reparseTranslationUnit..."));
		clang_reparseTranslationUnit(TU, 1, &unsavedFile, clang_defaultReparseOptions(TU));
		CL_DEBUG(wxT("Calling clang_reparseTranslationUnit..."));
#endif
		CL_DEBUG(wxT("Calling clang_codeCompleteAt..."));
		// Do the code-completion
		reply->results = clang_codeCompleteAt(TU,
		                                      cstr(task->GetFileName()),
		                                      task->GetLine(),
		                                      task->GetColumn(),
		                                      &unsavedFile,
		                                      1,
		                                      clang_defaultCodeCompleteOptions());

		CL_DEBUG(wxT("Calling clang_codeCompleteAt... done"));

		// Report diagnostics to the log file
		const unsigned diagCount = clang_getNumDiagnostics(TU);
		for(unsigned i=0; i<diagCount; i++) {
			CXDiagnostic diag = clang_getDiagnostic(TU, i);
			CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diag);
			if(severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal || severity == CXDiagnostic_Warning) {
				CXString diagStr  = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
				wxString wxDiagString = wxString(clang_getCString(diagStr), wxConvUTF8);
				if(!wxDiagString.Contains(wxT("'dllimport' attribute"))) {
					CL_DEBUG(wxT("Diagnostic: %s"), wxDiagString.c_str());

				}
				clang_disposeString(diagStr);
				clang_disposeDiagnostic(diag);
			}
		}
	}
	eEnd.SetClientData(reply);
	wxTheApp->AddPendingEvent(eEnd);
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
}

bool ClangWorkerThread::IsCacheEmpty()
{
	wxCriticalSectionLocker locker(m_cs);
	return m_cache.IsEmpty();
}

char** ClangWorkerThread::MakeCommandLine(const wxString& command, int& argc, bool isHeader)
{
	wxArrayString tokens = wxStringTokenize(command, wxT(" \t"), wxTOKEN_STRTOK);
	if(isHeader) {
		tokens.Add(wxT("-x"));
		tokens.Add(wxT("c++-header"));
	}
	argc = tokens.GetCount();
	char** argv = new char*[argc];

	if(tokens.IsEmpty())
		return argv;

	for(size_t i=0; i<tokens.GetCount(); i++) {
		argv[i] = strdup(tokens.Item(i).mb_str(wxConvUTF8).data());
	}
	return argv;
}

#endif // HAS_LIBCLANG
