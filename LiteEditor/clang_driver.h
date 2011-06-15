#ifndef CLANGDRIVER_H
#define CLANGDRIVER_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "clangpch_cache.h"

class IEditor;
class ClangDriverCleaner;

class ClangDriver : public wxEvtHandler {
	friend class ClangDriverCleaner;
public:
	enum CommandType {
		CT_CodeCompletion,
		CT_CreatePCH,
		CT_PreProcess
	};

protected:
	IProcess *                  m_process;
	wxString                    m_tmpfile;
	std::map<wxString,wxString> m_backticks;
	wxString                    m_output;
	int                         m_activationPos;
	IEditor*                    m_activationEditor;
	CommandType                 m_commandType;
	ClangPCHCache               m_cache;
	wxArrayString               m_removedIncludes;
	wxArrayString               m_pchHeaders;
	wxString                    m_compilationArgs;
	bool                        m_isCalltip;

protected:
	void          DoRunCommand(IEditor *editor, CommandType type);
	wxArrayString GetStandardIncludePathsArgs(const wxString &clangBinary);
	void          DoCleanup();
	void          DoFilterIncludeFilesFromPP();
	void          DoRemoveAllIncludeStatements(wxString &buffer, wxArrayString &includesRemoved);
	wxString      DoGetPchHeaderFile(const wxString &filename);
	wxString      DoGetPchOutputFileName(const wxString &filename);
	bool          ShouldInclude(const wxString &header);
	void          DoPrepareCompilationArgs(const wxString &projectName, const wxString &clangBinary);
	wxString      DoExpandBacktick(const wxString &backtick);
	
	// Internal
	void OnPCHCreationCompleted();
	void OnCodeCompletionCompleted();
	void OnPreProcessingCompleted();

public:
	ClangDriver();
	virtual ~ClangDriver();

	void CodeCompletion(IEditor *editor);
	void Abort();

	void SetActivationPos(int activationPos) {
		this->m_activationPos = activationPos;
	}
	int GetActivationPos() const {
		return m_activationPos;
	}

	const ClangPCHCache& GetCache() const {
		return m_cache;
	}

	ClangPCHCache &GetCache() {
		return m_cache;
	}

	void SetIsCalltip(bool isCalltip) {
		this->m_isCalltip = isCalltip;
	}
	bool GetIsCalltip() const {
		return m_isCalltip;
	}
	DECLARE_EVENT_TABLE()
	void OnClangProcessOutput    (wxCommandEvent &e);
	void OnClangProcessTerminated(wxCommandEvent &e);
	void OnFileSaved             (wxCommandEvent &e);
};

class ClangDriverCleaner {
	ClangDriver *m_driver;
public:
	ClangDriverCleaner(ClangDriver *driver) : m_driver(driver) {}
	~ClangDriverCleaner() {
		if(m_driver)
			m_driver->DoCleanup();
	}
	void Clear() {
		m_driver = NULL;
	}
};

#endif // CLANGDRIVER_H
