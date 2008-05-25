//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findresultscontainer.h              
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
 #ifndef __findresultscontainer__
#define __findresultscontainer__

#include "wx/panel.h"
class FindResultsTab;
class Notebook;

class FindResultsContainer : public wxPanel {
	Notebook *m_book;
	
protected:
	void Initialize();
	
public:
	FindResultsContainer(wxWindow *win, wxWindowID id);
	~FindResultsContainer();
	
	FindResultsTab *GetActiveTab();
	void SetActiveTab(int selection);
	
	Notebook *GetNotebook(){return m_book;}
};
#endif // __findresultscontainer__
