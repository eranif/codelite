#ifndef __wxtabcontainer__
#define __wxtabcontainer__

#include "wx/panel.h"

class wxVerticalTab;

class wxTabContainer : public wxPanel {
	
	int m_orientation;
	wxSizer *m_tabsSizer;
	wxVerticalTab *m_draggedTab;
	
protected:
	void Initialize();
	
public:
	wxTabContainer(wxWindow *win, wxWindowID id = wxID_ANY, int orientation = wxLEFT);
	virtual ~wxTabContainer();
	
	void AddTab(wxVerticalTab *tab);
	wxVerticalTab *GetSelection();
	
	void SetSelection(wxVerticalTab *tab, bool notify = false);
	wxVerticalTab *IndexToTab(size_t page);
	size_t TabToIndex(wxVerticalTab *tab);
	size_t GetTabsCount();
	
	/**
	 * \brief set tab as being dragged
	 * \param tab
	 */
	void SetDraggedTab(wxVerticalTab *tab);
	
	/**
	 * \brief swap tab with the dragged one
	 * \param tab swap dragged tab this one
	 */
	void SwapTabs(wxVerticalTab *tab);
	
	//Setters
	void SetOrientation(const int& orientation) ;
	const int& GetOrientation() const {return m_orientation;}
	
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent &e);
	virtual void OnEraseBg(wxEraseEvent &e);
	virtual void OnSizeEvent(wxSizeEvent &e);
	virtual void OnLeaveWindow(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	
	
};
#endif // __wxtabcontainer__
