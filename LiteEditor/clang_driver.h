#ifndef CLANGDRIVER_H
#define CLANGDRIVER_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "clangpch_cache.h"
#include "clang_pch_maker_thread.h"

class IEditor;
class ClangDriverCleaner;

class ClangDriver : public wxEvtHandler
{
public:
	enum WorkingContext {
		CTX_CodeCompletion,
		CTX_Calltip,
		CTX_CachePCH,
		CTX_WordCompletion
	};

protected:
	IProcess *                  m_process;
	wxString                    m_tmpfile;
	std::map<wxString,wxString> m_backticks;
	wxString                    m_output;
	int                         m_activationPos;
	ClangPchMakerThread         m_pchMakerThread;
	wxString                    m_filename;
	WorkingContext              m_context;
	wxString                    m_ccfilename;
	
protected:
	void          DoRunCommand(IEditor *editor);
	void          DoCleanup();
	void          DoRemoveAllIncludeStatements(wxString &buffer, wxArrayString &includesRemoved);
	wxString      DoGetPchHeaderFile(const wxString &filename);
	wxString      DoGetPchOutputFileName(const wxString &filename);
	wxString      DoPrepareCompilationArgs(const wxString &projectName, wxString &projectPath);
	wxString      DoExpandBacktick(const wxString &backtick, const wxString &projectName);
	bool          DoProcessBuffer(IEditor* editor, wxString &location, wxString &completefileName, wxString &filterWord);
	int           DoGetHeaderScanLastPos(IEditor *editor);
	wxString      DoGetClangBinary();

	// Internal
	void OnCodeCompletionCompleted();

public:
	ClangDriver();
	virtual ~ClangDriver();

	void CodeCompletion(IEditor *editor);
	void Abort();

	bool IsBusy() const {
		return m_process != NULL;
	}

	void SetContext(WorkingContext context) {
		this->m_context = context;
	}
	WorkingContext GetContext() const {
		return m_context;
	}
	void SetActivationPos(int activationPos) {
		this->m_activationPos = activationPos;
	}
	int GetActivationPos() const {
		return m_activationPos;
	}

	void ClearCache();
	bool IsCacheEmpty();
	
	DECLARE_EVENT_TABLE()
	void OnClangProcessOutput    (wxCommandEvent &e);
	void OnClangProcessTerminated(wxCommandEvent &e);
};

#endif // CLANGDRIVER_H
