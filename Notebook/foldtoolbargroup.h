#ifndef __foldtoolbargroup__
#define __foldtoolbargroup__

#include "wx/panel.h"
#include "wx/bitmap.h"

class FoldToolBar;

class FoldToolbarGroup : public wxPanel {
	wxString m_caption;
	bool m_hovered;
	
protected:
	void Initialize();
	
public:
	FoldToolbarGroup(FoldToolBar *parent, const wxString &caption);
	virtual ~FoldToolbarGroup();
	
	void Add(wxWindow* item, int proportion = 0, int border = 0);
	
	//Setters
	void SetHovered(const bool& hovered) ;
	
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSize(wxSizeEvent &e);
	virtual void OnEnterWindow(wxMouseEvent &e);

};
#endif // __foldtoolbargroup__
