//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : dropbutton.h              
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
