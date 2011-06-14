#ifndef CODECOMPLETIONMANAGER_H
#define CODECOMPLETIONMANAGER_H

#include <wx/string.h>
#include <wx/filename.h>
#include "cl_editor.h"

class CodeCompletionManager
{
protected:
	size_t m_options;
	
protected:
	/// ctags implementions
	bool DoCtagsWordCompletion(LEditor* editor, const wxString& expr, const wxString& word);
	bool DoCtagsCalltip       (LEditor* editor, int line, const wxString &expr, const wxString &text, const wxString &word);
	bool DoCtagsCodeComplete  (LEditor* editor, int line, const wxString &expr, const wxString &text);

	/// clang implementations
	void DoClangWordCompletion(LEditor* editor);
	void DoClangCalltip       (LEditor* editor);
	void DoClangCodeComplete  (LEditor* editor);
	
	void DoUpdateOptions();
	
public:
	CodeCompletionManager();
	virtual ~CodeCompletionManager();

	void SetOptions(size_t options) {
		this->m_options = options;
	}
	size_t GetOptions() const {
		return m_options;
	}
	static CodeCompletionManager& Get();
	void WordCompletion(LEditor* editor, const wxString& expr, const wxString& word);
	void Calltip       (LEditor* editor, int line, const wxString &expr, const wxString &text, const wxString &word);
	void CodeComplete  (LEditor* editor, int line, const wxString &expr, const wxString &text);
};

#endif // CODECOMPLETIONMANAGER_H
