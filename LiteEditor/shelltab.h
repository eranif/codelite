#ifndef SHELLTAB_H
#define SHELLTAB_H

#include "wx/wxscintilla.h"
#include "wx/panel.h"

class ShellWindow;

class ShellTab : public wxPanel
{
	wxString m_name;
	ShellWindow *m_window;

protected:
	void CreateGUIControl();
	void OnClearAll(wxCommandEvent &e);
	void OnWordWrap(wxCommandEvent &e);

public:
	ShellTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~ShellTab();

	const wxString &GetCaption() const {return m_name;}
	void Clear();
	void AppendText(const wxString &text);
	ShellWindow *GetShell() {return m_window;}
};

#endif //SHELLTAB_H



