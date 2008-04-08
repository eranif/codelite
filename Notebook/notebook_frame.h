#ifndef __NotebookFrame__
#define __NotebookFrame__

#include "wx/frame.h"
class Notebook;
class NotebookFrame : public wxFrame {
	Notebook *m_topbook;
private:
	void Initialize();
	void CreateMenuBar();
	
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
};
#endif // __NotebookFrame__
