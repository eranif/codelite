#ifndef CLANGCODECOMPLETION_H
#define CLANGCODECOMPLETION_H

#if HAS_LIBCLANG

#include <wx/event.h>
#include "asyncprocess.h"
#include <map>
#include "entry.h"
#include "clang_driver.h"
#include "clang_output_parser_api.h"

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
	ClangDriver                 m_clang;
	bool                        m_allEditorsAreClosing;
	
	friend class ClangDriver;

public:
	static ClangCodeCompletion* Instance();
	static void Release();

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
	
private:
	ClangCodeCompletion();
	~ClangCodeCompletion();

};

#endif // HAS_LIBCLANG
#endif // CLANGCODECOMPLETION_H
