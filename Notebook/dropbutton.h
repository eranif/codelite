#ifndef __dropbutton__
#define __dropbutton__

#include "wx/panel.h"
#include "wx/bitmap.h"

class wxTabContainer;

enum ButtonState {
	BTN_NONE = 0,
	BTN_PUSHED
};

class DropButton : public wxPanel {
	wxTabContainer *m_tabContainer;
	ButtonState m_state;
	wxBitmap m_arrowDownBmp;
	
public:
	DropButton(wxWindow *parent, wxTabContainer *tabContainer);
	~DropButton();
	
	DECLARE_EVENT_TABLE()
	virtual void OnLeftDown(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnMenuSelection(wxCommandEvent &e);
};

#endif // __dropbutton__
