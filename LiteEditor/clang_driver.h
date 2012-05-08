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
	wxString            DoPrepareCompilationArgs(const wxString &projectName, wxString &projectPath);
	wxString            DoExpandBacktick(const wxString &backtick, const wxString &projectName);
	void                DoParseCompletionString(CXCompletionString str, int depth, wxString &entryName, wxString &signature, wxString &completeString, wxString &returnValue);
	void                DoProcessMacros(ClangThreadReply *reply);
	ClangThreadRequest* DoMakeClangThreadRequest(IEditor* editor, WorkingContext context);
	
public:
	ClangDriver();
	virtual ~ClangDriver();

	void QueueRequest  (IEditor *editor, WorkingContext context);
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
};

#endif // HAS_LIBCLANG

#endif // CLANGDRIVER_H
