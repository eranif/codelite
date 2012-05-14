#ifndef CLANGDRIVER_H
#define CLANGDRIVER_H

#if HAS_LIBCLANG

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "clangpch_cache.h"
#include "clang_pch_maker_thread.h"
#include <clang-c/Index.h>

class IEditor;
class ClangDriverCleaner;

struct ClangRequest
{
	wxString       filename;
	WorkingContext context;
};

class ClangDriver : public wxEvtHandler
{
protected:
	bool                        m_isBusy;
	std::map<wxString,wxString> m_backticks;
	ClangWorkerThread           m_pchMakerThread;
	WorkingContext              m_context;
	CXIndex                     m_index;
	IEditor*                    m_activeEditor;
	int                         m_position;
	
protected:
	void                DoCleanup();
	wxArrayString       DoPrepareCompilationArgs(const wxString &projectName, const wxString &sourceFile, wxString &projectPath, wxString &pchfile);
	wxArrayString       DoExpandBacktick(const wxString &backtick, const wxString &projectName);
	void                DoParseCompletionString(CXCompletionString str, int depth, wxString &entryName, wxString &signature, wxString &completeString, wxString &returnValue);
	void                DoGotoDefinition(ClangThreadReply *reply);
	ClangThreadRequest* DoMakeClangThreadRequest(IEditor* editor, WorkingContext context);
	
	// Event handlers
	void OnDeletMacroHandler(wxCommandEvent &e);
	
public:
	ClangDriver();
	virtual ~ClangDriver();

	void QueueRequest  (IEditor *editor, WorkingContext context);
	void GetMacros(IEditor* editor);
	void ReparseFile(const wxString &filename);
	void CodeCompletion(IEditor *editor);
	void Abort();

	bool IsBusy() const {
		return m_isBusy;
	}

	void SetContext(WorkingContext context) {
		this->m_context = context;
	}
	WorkingContext GetContext() const {
		return m_context;
	}
	
	void ClearCache();
	bool IsCacheEmpty();
	
	// Event Handlers
	void OnPrepareTUEnded(wxCommandEvent &e);
	void OnCacheCleared(wxCommandEvent &e);
	void OnTUCreateError(wxCommandEvent &e);
	void OnWorkspaceLoaded(wxCommandEvent &event);
};

#endif // HAS_LIBCLANG

#endif // CLANGDRIVER_H
