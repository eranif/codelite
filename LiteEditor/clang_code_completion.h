#ifndef CLANGCODECOMPLETION_H
#define CLANGCODECOMPLETION_H

#if HAS_LIBCLANG

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "entry.h"
#include "clang_driver.h"
#include "clang_output_parser_api.h"
#include <set>

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
public:
	typedef std::set<wxString> Set_t;

protected:
	static ClangCodeCompletion* ms_instance;
	ClangDriver                 m_clang;
	bool                        m_allEditorsAreClosing;
	bool                        m_parseBuildOutput;
	wxString                    m_projectCompiled;
	wxString                    m_configurationCompiled;
	wxString                    m_processOutput;
	
	friend class ClangDriver;
public:
	static ClangCodeCompletion* Instance();
	static void Release();

	bool IsParseBuildOutput() const {
		return m_parseBuildOutput;
	}
	/**
	 * @brief perform codecompletion in the editor
	 */
	void CodeComplete(IEditor *editor);
	/**
	 * @brief perform word-completin in the editor
	 */
	void WordComplete(IEditor *editor);
	/**
	 * @brief provide list of macros and pass them to the editor (this is useful for disabling block of text which is not visible due to #if #endif conditions)
	 */
	void ListMacros(IEditor *editor);
	/**
	 * @brief go to the definition under the caret
	 */
	void GotoDeclaration(IEditor *editor);
	/**
	 * @brief go to the definition under the caret
	 */
	void GotoImplementation(IEditor *editor);
	
	/**
	 * @brief display calltip for a function
	 */
	void Calltip(IEditor *editor);
	void CancelCodeComplete();
	void ClearCache();
	bool IsCacheEmpty();

protected:
	void DoCleanUp();
	
	// Event handling
	void OnFileLoaded(wxCommandEvent &e);
	void OnFileSaved(wxCommandEvent &e);
	void OnAllEditorsClosing(wxCommandEvent &e);
	void OnAllEditorsClosed(wxCommandEvent &e);

	void OnBuildStarted(wxCommandEvent &e);
	void OnBuildEnded(wxCommandEvent &e);
	void OnBuildOutput(wxCommandEvent &e);
	void OnClangPathResolved(wxCommandEvent &e);
private:
	ClangCodeCompletion();
	~ClangCodeCompletion();

};

#endif // HAS_LIBCLANG
#endif // CLANGCODECOMPLETION_H
