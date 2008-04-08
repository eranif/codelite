#ifndef __wxverticaltab__
#define __CustomTab__

#include "wx/dc.h"
#include "wx/bitmap.h"
#include "wx/panel.h"

extern const wxEventType wxEVT_CMD_DELETE_TAB;

enum XState {
	XNone,
	XPushed
};

class wxWindow;
class CustomTab : public wxPanel {
	
	wxString m_text;
	wxBitmap m_bmp;
	bool m_selected;
	int m_padding;
	int m_heightPadding;
	int m_orientation;
	wxWindow *m_window;
	bool m_leftDown;
	bool m_hovered;
	wxRect m_xButtonRect;
	wxBitmap m_xButtonNormalBmp;
	wxBitmap m_xButtonPressedBmp;
	long m_style;
	XState m_x_state;
	
protected:
	int CalcTabHeight();
	int CalcTabWidth();
	
	void DoDrawVerticalTab(wxDC &dc);
	void DoDrawHorizontalTab(wxDC &dc);
	const wxBitmap &GetXBmp();
	
public:
	CustomTab(wxWindow *win, wxWindowID id, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false, int orientation = wxLEFT, long style=0);
	virtual ~CustomTab();
	
	//Getters
	void SetPadding(const int& padding) {this->m_padding = padding;}
	void SetOrientation(const int& orientation) {this->m_orientation = orientation;}
	void SetWindow(wxWindow* window) {this->m_window = window;}
	void SetSelected(bool selected) ;
	//Getters
	const wxBitmap& GetBmp() const {return m_bmp;}
	const bool& GetSelected() const {return m_selected;}
	const wxString& GetText() const {return m_text;}
	const int& GetOrientation() const {return m_orientation;}
	const int& GetPadding() const {return m_padding;}
	wxWindow* GetWindow() const {return m_window;}
	
	void SetHeightPadding(const int& heightPadding) {this->m_heightPadding = heightPadding;}
	const int& GetHeightPadding() const {return m_heightPadding;}
	
	DECLARE_EVENT_TABLE()
	
	//Event handlers
	virtual void OnPaint(wxPaintEvent &event);
	virtual void OnErase(wxEraseEvent &event);
	virtual void OnLeftDown(wxMouseEvent &e);
	virtual void OnMouseEnterWindow(wxMouseEvent &e);
	virtual void OnMouseLeaveWindow(wxMouseEvent &e);
	virtual void OnMouseMove(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
	virtual void OnMouseMiddleButton(wxMouseEvent &e);
};
#endif // __CustomTab__
