#ifndef __foldtoolbar__
#define __foldtoolbar__

#include "wx/panel.h"
#include "wx/sizer.h"

class FoldToolbarGroup;

class FoldToolBar : public wxPanel {
	long m_style;
	wxSizer *m_mainSizer;
	
protected:	
	void Initialize();
	
public:
	FoldToolBar(wxWindow *parent, long style);
	virtual ~FoldToolBar();
	
	void AddGroup(FoldToolbarGroup *group, int proportion = 0);
	void Realize();
	
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSize(wxSizeEvent &e);
	virtual void OnDeselectAll(wxMouseEvent &e);
};
#endif // __foldtoolbar__
