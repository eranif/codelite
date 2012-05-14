#ifndef CLANGMACROHANDLER_H
#define CLANGMACROHANDLER_H

#include "precompiled_header.h"
#include "asyncprocess.h"
#include <wx/event.h>
#include "ieditor.h"
#include <set>

extern const wxEventType wxEVT_CMD_CLANG_MACRO_HADNLER_DELETE;

class ClangMacroHandler : public wxEvtHandler
{
	IProcess *         m_process;
	wxString           m_output;
	IEditor*           m_editor;
	std::set<wxString> m_interestingMacros;
	
public:
	ClangMacroHandler();
	virtual ~ClangMacroHandler();

	void SetOutput(const wxString& output) {
		this->m_output = output;
	}
	void SetProcessAndEditor(IProcess* process, IEditor *editor);

	const wxString& GetOutput() const {
		return m_output;
	}
	IProcess* GetProcess() {
		return m_process;
	}

	IEditor* GetEditor() {
		return m_editor;
	}
	void Cancel();
	DECLARE_EVENT_TABLE()

	void OnClangProcessTerminated(wxCommandEvent &e);
	void OnClangProcessOutput(wxCommandEvent &e);
	void OnEditorClosing(wxCommandEvent &e);
	void OnAllEditorsClosing(wxCommandEvent &e);
};

#endif // CLANGMACROHANDLER_H
