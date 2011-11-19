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

class ClangDriver : public wxEvtHandler
{
protected:
	bool                        m_isBusy;
	std::map<wxString,wxString> m_backticks;
	ClangWorkerThread           m_pchMakerThread;
	WorkingContext              m_context;
	CXIndex                     m_index;
	IEditor*                    m_activeEditor;
	
protected:
	void     DoCleanup();
	wxString DoPrepareCompilationArgs(const wxString &projectName, wxString &projectPath);
	wxString DoExpandBacktick(const wxString &backtick, const wxString &projectName);
	void     DoParseCompletionString(CXCompletionString str, wxString &entryName, wxString &signature, wxString &completeString, wxString &returnValue);
	
public:
	ClangDriver();
	virtual ~ClangDriver();

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
	
	DECLARE_EVENT_TABLE()
	void OnPrepareTUEnded(wxCommandEvent &e);
};
#endif // HAS_LIBCLANG

#endif // CLANGDRIVER_H
