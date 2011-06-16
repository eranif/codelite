#ifndef CLANGCODECOMPLETION_H
#define CLANGCODECOMPLETION_H

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "entry.h"
#include "clang_driver.h"

class IEditor;
class IManager;

/**
 * @class ClangCodeCompletion
 * @author eran
 * @date 07/18/10
 * @file clang_code_completion.h
 * @brief codelite's interface to clang's code completion
 */
class ClangCodeCompletion : public wxEvtHandler
{

	static ClangCodeCompletion* ms_instance;
	IProcess*                   m_process;
	int                         m_activationPos;
	IEditor*                    m_activationEditor;
	ClangDriver                 m_clang;
	bool                        m_allEditorsAreClosing;
	
	friend class ClangDriver;

public:
	static ClangCodeCompletion* Instance();
	static void Release();

	/**
	 * @brief perform code completion
	 * @param editor
	 */
	void CodeComplete(IEditor *editor);
	void Calltip(IEditor *editor);
	
	void CancelCodeComplete();
	const ClangPCHCache &GetCache() const {
		return m_clang.GetCache();
	}
	ClangPCHCache &GetCache() {
		return m_clang.GetCache();
	}
protected:
	void DoParseOutput(const wxString &output);
	void DoCleanUp();
	TagEntryPtr ClangEntryToTagEntry(const wxString &line);
	
	// Event handling
	void OnFileLoaded(wxCommandEvent &e);
	void OnAllEditorsClosing(wxCommandEvent &e);
	void OnAllEditorsClosed(wxCommandEvent &e);
	
private:
	ClangCodeCompletion();
	~ClangCodeCompletion();

};

#endif // CLANGCODECOMPLETION_H
