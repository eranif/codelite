#ifndef __progressctrl__
#define __progressctrl__

#include <wx/panel.h>

class ProgressCtrl : public wxPanel {
	
	wxString m_msg;
	size_t m_maxRange;
	size_t m_currValue;
	
public:
	ProgressCtrl(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style =0);
	virtual ~ProgressCtrl();
	
	void Update(size_t value, const wxString &msg);
	void Clear();
	
	//Setters
	void SetMaxRange(const size_t& maxRange) {this->m_maxRange = maxRange;}
	void SetMsg(const wxString& msg) {this->m_msg = msg;}
	//Getters
	const size_t& GetMaxRange() const {return m_maxRange;}
	const wxString& GetMsg() const {return m_msg;}
	
	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent &e);
	void OnEraseBg(wxEraseEvent &e);
};
#endif // __progressctrl__
