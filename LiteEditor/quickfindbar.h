//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickfindbar.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
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
#ifndef __quickfindbar__
#define __quickfindbar__

#include <wx/panel.h>

class wxTextCtrl;
class wxScintilla;


class QuickFindBar : public wxPanel
{
protected:
    wxTextCtrl  *m_findWhat;
    wxScintilla *m_sci;
    int          m_flags;
    
    void DoSearch(bool fwd, bool incr);
    
	void OnHide    (wxCommandEvent  &e);
	void OnNext    (wxCommandEvent  &e);
	void OnPrev    (wxCommandEvent  &e);
	void OnText    (wxCommandEvent  &e);
    void OnCheckBox(wxCommandEvent  &e);
	void OnKeyDown (wxKeyEvent      &e);
    void OnUpdateUI(wxUpdateUIEvent &e);
    
    DECLARE_EVENT_TABLE()
    
public:
	QuickFindBar(wxWindow *parent, wxWindowID id = wxID_ANY);
    
    bool Show(bool s = true);
    
    wxScintilla *GetEditor() { return m_sci; }
    void SetEditor(wxScintilla *sci) { m_sci = sci; }
};

#endif // __quickfindbar__
