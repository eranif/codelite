#ifndef WINDOWSTACK_H
#define WINDOWSTACK_H

#include "wx/panel.h"
#include "map"
#include "wx/sizer.h"

class WindowStack : public wxPanel {
	std::map<wxString, wxWindow*> m_windows;
	wxBoxSizer *m_mainSizer;
	wxWindow *m_selection;

public:
	WindowStack(wxWindow *parent, wxWindowID id = wxID_ANY);
	virtual ~WindowStack();
	
	void Add(wxWindow *win, const wxString &key);
	void Select(const wxString &key);
	void Clear();
	void Delete(const wxString &key);
	wxWindow *Find(const wxString &key);
};

#endif //WINDOWSTACK_H


