#ifndef __NotebookFrame__
#define __NotebookFrame__

#include "wx/aui/framemanager.h"
#include "wx/frame.h"

class Notebook;
class wxPanel;
class FoldToolBar;

class NotebookFrame : public wxFrame {
	Notebook *m_topbook;
	wxAuiManager m_mgr;
	FoldToolBar *barPanel;
	
private:
	void Initialize();
	void CreateMenuBar();
	void CreateFoldToolbar();
	
public:
	NotebookFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

	~NotebookFrame();
	DECLARE_EVENT_TABLE()
	
	void OnClose(wxCloseEvent &e);
	void OnQuit(wxCommandEvent &e);
	
	
	void OnDeletePage(wxCommandEvent &e);
	void OnNewPage(wxCommandEvent &e);
	void OnPanelDClick(wxMouseEvent &e); 
};
#endif // __NotebookFrame__
