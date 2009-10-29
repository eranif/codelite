#ifndef __ActivetipdemoFrame__
#define __ActivetipdemoFrame__

#include "wx/frame.h"
#include "ActiveTip/active_tip.h"

class ActivetipdemoFrame : public wxFrame {

private:
	void Initialize();
	void CreateMenuBar();

	ATFrame CreateNewFrame();
public:
	ActivetipdemoFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

	~ActivetipdemoFrame();
	DECLARE_EVENT_TABLE()

	void OnClose(wxCloseEvent &e);
	void OnQuit(wxCommandEvent &e);
	void OnActiveTipExpanding(wxCommandEvent &e);

};
#endif // __ActivetipdemoFrame__
