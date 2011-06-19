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
	
	wxArrayString GetIncludePaths() const;
};

#endif // CODECOMPLETIONPAGE_H
