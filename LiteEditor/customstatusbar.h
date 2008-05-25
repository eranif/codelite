//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : customstatusbar.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #ifndef __customstatusbar__
#define __customstatusbar__

#include <wx/statusbr.h>
#include <wx/gauge.h>

class ProgressCtrl;

class CustomStatusBar : public wxStatusBar {
	ProgressCtrl *m_gauge; 
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
