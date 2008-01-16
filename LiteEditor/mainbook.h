#ifndef MAINBOOK_H
#define MAINBOOK_H

#include "wx/event.h"
#include "wx/string.h"
#include "entry.h"

class wxChoice;

class MainBook : public wxEvtHandler 
{
	wxChoice *m_choiceFunc;
	wxChoice *m_choiceScope;
protected:
	void OnFuncListMouseDown(wxMouseEvent &e);
	void OnScopeListMouseDown(wxMouseEvent &e);
	void OnScope(wxCommandEvent &e);
	void OnFunction(wxCommandEvent &e);
	
public:
	MainBook(wxChoice* choiceFunc, wxChoice* choiceScope);
	~MainBook();
	
	void UpdateScope(TagEntryPtr tag);
	void Clear();
};

#endif //MAINBOOK_H
