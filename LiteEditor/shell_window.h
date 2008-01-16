#ifndef SHELL_WINDOW_H
#define SHELL_WINDOW_H

#include "wx/wxscintilla.h"
#include "wx/event.h"
#include <wx/arrstr.h>

class wxScintilla;
class wxTextCtrl;
class wxSplitterWindow;

extern const wxEventType wxEVT_SHELLWIN_LINE_ENTERED;
extern const wxEventType wxEVT_SHELLWIN_CTRLC;

class ShellWindow : public wxPanel 
{
	wxEvtHandler *m_handler;
	wxScintilla *m_outWin;
	wxTextCtrl *m_inWin;
	wxArrayString m_history;
	int m_cur;
	bool m_keepFocus;

protected:
	void ConnectEvents();
	void Initialize();
	
public:
	ShellWindow(wxWindow *parent, wxEvtHandler *handler);
	virtual ~ShellWindow();

	//Setters
	void SetHandler(wxEvtHandler*& handler) {this->m_handler = handler;}
	
	//Getters
	const wxEvtHandler* GetHandler() const {return m_handler;}
	const wxTextCtrl* GetInWin() const {return m_inWin;}
	const wxScintilla* GetOutWin() const {return m_outWin;}
	
	void AppendLine(const wxString &text);
	void Clear();
	void OnKeyDown(wxKeyEvent &event);
	void OnEnterPressed(wxCommandEvent &e);
};
#endif // SHELL_WINDOW_H
