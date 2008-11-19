//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : errorscontainer.h              
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

 #ifndef __errorscontainer__
#define __errorscontainer__

#include <wx/checkbox.h>

class ErrorsTab;

class ErrorsContainer : public wxPanel {
public:
	static const wxString ERRORS_WIN;
	
protected:
	wxCheckBox*  m_buttonErrors;
	wxCheckBox*  m_buttonWarnings;
	ErrorsTab*       m_errorsTab;
	
protected:
	virtual void Initialize();
	
	virtual void OnErrorButton  ( wxCommandEvent& event );
	virtual void OnWarningButton( wxCommandEvent& event );

public:
	ErrorsContainer( wxWindow *win, wxWindowID id );
	virtual ~ErrorsContainer();
	
	virtual void AppendText( const wxString &text );
	virtual void Clear();
	
	ErrorsTab *GetErrorsTab() { return m_errorsTab; }
};


#endif // __errorscontainer__
