#ifndef CODECOMPLETIONPAGE_H
#define CODECOMPLETIONPAGE_H

#include "workspacesettingsbase.h" // Base class: CodeCompletionBasePage

class CodeCompletionPage : public CodeCompletionBasePage
{
public:
	enum {
		TypeWorkspace,
		TypeProject
	};
	
protected:
	int m_type;
	
public:
	CodeCompletionPage(wxWindow *parent, int type);
	virtual ~CodeCompletionPage();
	
	void Save();
	
	wxArrayString GetIncludePaths() const;
	wxString      GetMacros() const;
	wxString      GetCmpOptions() const;
	wxString      GetIncludePathsAsString() const;
};

#endif // CODECOMPLETIONPAGE_H
