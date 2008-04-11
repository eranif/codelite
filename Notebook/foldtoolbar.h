#ifndef __foldtoolbar__
#define __foldtoolbar__

#include "wx/panel.h"
#include "wx/sizer.h"

class FoldToolbarGroup;

enum FbButtonState {
	FB_BTN_NONE,
	FB_BTN_HOVERED,
	FB_BTN_PUSHED
};

class FoldToolBar : public wxPanel {
	long m_style;
	wxSizer *m_mainSizer;
	FbButtonState m_state;
	
protected:	
	void Initialize();
	
public:
	FoldToolBar(wxWindow *parent, long style);
	virtual ~FoldToolBar();
	
	void AddGroup(FoldToolbarGroup *group, int proportion = 0, int border = 0);
	void Realize();
	
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSize(wxSizeEvent &e);
	virtual void OnEnterWindow(wxMouseEvent &e);
	virtual void OnLeaveWindow(wxMouseEvent &e);
	virtual void OnLeftDown(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnMouseMove(wxMouseEvent &e);
};
#endif // __foldtoolbar__
