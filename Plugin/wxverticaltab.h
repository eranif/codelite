#ifndef __wxverticaltab__
#define __wxverticaltab__

#include "wx/bitmap.h"
#include "wx/panel.h"

class wxWindow;
class wxVerticalTab : public wxPanel {
	
	wxString m_text;
	wxBitmap m_bmp;
	bool m_selected;
	int m_padding;
	int m_orientation;
	wxWindow *m_window;
	bool m_leftDown;
	
protected:
	int CalcTabHeight();
	int CalcTabWidth();
	
public:
	wxVerticalTab(wxWindow *win, wxWindowID id, const wxString &text, const wxBitmap &bmp = wxNullBitmap, bool selected = false, int orientation = wxLEFT);
	virtual ~wxVerticalTab();
	
	//Getters
	void SetPadding(const int& padding) {this->m_padding = padding;}
	void SetOrientation(const int& orientation) {this->m_orientation = orientation;}
	void SetWindow(wxWindow* window) {this->m_window = window;}
	void SetSelected(bool selected) {this->m_selected = selected;}
	//Getters
	const wxBitmap& GetBmp() const {return m_bmp;}
	const bool& GetSelected() const {return m_selected;}
	const wxString& GetText() const {return m_text;}
	const int& GetOrientation() const {return m_orientation;}
	const int& GetPadding() const {return m_padding;}
	wxWindow* GetWindow() const {return m_window;}
	
	DECLARE_EVENT_TABLE()
	
	//Event handlers
	virtual void OnPaint(wxPaintEvent &event);
	virtual void OnErase(wxEraseEvent &event);
	virtual void OnLeftDown(wxMouseEvent &e);
	virtual void OnMouseEnterWindow(wxMouseEvent &e);
	virtual void OnMouseMove(wxMouseEvent &e);
	virtual void OnLeftUp(wxMouseEvent &e);
};
#endif // __wxverticaltab__
