#ifndef __customstatusbar__
#define __customstatusbar__

#include <wx/statusbr.h>
#include <wx/gauge.h>

class CustomStatusBar : public wxStatusBar {
	wxGauge *m_gauge; 
public:
	CustomStatusBar(wxWindow *parent, wxWindowID id = wxID_ANY);
	virtual ~CustomStatusBar();
	
	/**
	 * @brief clear the gauge and set its max range
	 * @param range new gauge range
	 */
	void ResetGauge(int range);
	
	/**
	 * @brief 
	 * @param vlaue
	 * @param message
	 */
	void Update(int vlaue, const wxString &message);
	
	// event handlers
	DECLARE_EVENT_TABLE()
    void OnSize(wxSizeEvent& event); 
};
#endif // __customstatusbar__
