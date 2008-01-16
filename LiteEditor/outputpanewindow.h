#ifndef OUTPUTPANEWINDOW_H
#define OUTPUTPANEWINDOW_H

#include "wx/wxscintilla.h"
#include "wx/panel.h"

#define WINTYPE_SCI		0
#define WINTYPE_SHELL	1

class OutputPaneWinodw : public wxPanel
{
	wxString m_name;
	wxWindow *m_window;
	int m_type;

protected:
	void CreateGUIControl();
	void OnClearAll(wxCommandEvent &e);
	void OnWordWrap(wxCommandEvent &e);

public:
	OutputPaneWinodw(wxWindow *parent, wxWindowID id, const wxString &name, int type = WINTYPE_SCI);
	virtual ~OutputPaneWinodw();

	const wxString &GetCaption() const {return m_name;}
	wxWindow *GetWindow() {return m_window;}
	void Clear();
	void AppendText(const wxString &text);
};

#endif //OUTPUTPANEWINDOW_H



