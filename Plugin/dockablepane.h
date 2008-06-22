#ifndef __dockablepane__
#define __dockablepane__

#include <wx/panel.h>
#include <wx/toolbar.h>
class Notebook;

extern const wxEventType wxEVT_CMD_DELETE_DOCKPANE;
class DockablePane : public wxPanel {
	wxToolBar *m_tb;
	wxWindow *m_child;
	Notebook *m_book;
	wxString m_text;
	wxBitmap m_bmp;
	
	void DockPaneBackToNotebook(wxCommandEvent &e);
public:
	DockablePane(wxWindow *parent, Notebook *book, wxWindow *child, const wxString &title, const wxBitmap &bmp = wxNullBitmap, wxSize size = wxDefaultSize);
	virtual ~DockablePane();
	wxString GetName() {return m_text;}
};
#endif // __dockablepane__
