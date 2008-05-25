//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findresultstab.h              
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
 #ifndef __findresultstab__
#define __findresultstab__

#include "outputtabwindow.h"
 
class FindResultsTab : public OutputTabWindow 
{

private:
	FindResultsTab(const FindResultsTab& rhs);
	FindResultsTab& operator=(const FindResultsTab& rhs);

public:
	FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name);
	virtual ~FindResultsTab();

	virtual void OnMouseDClick(wxScintillaEvent &event);
	virtual void OnHotspotClicked(wxScintillaEvent &event);
};
#endif // __findresultstab__
