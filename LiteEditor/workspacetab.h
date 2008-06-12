//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : workspacetab.h              
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
 #ifndef __workspacetab__
#define __workspacetab__

#include <wx/panel.h>

class FileViewTree;
class wxComboBox;

class WorkspaceTab : public wxPanel {
	FileViewTree *m_fileView;
	
	void OnCollapseAll(wxCommandEvent &event);
	void OnCollapseAllUI(wxUpdateUIEvent &event);
	void DoCollpaseAll();
	
protected:	
	void CreateGUIControls();
	
public:
	WorkspaceTab(wxWindow *parent);
	~WorkspaceTab();
	
	void BuildFileTree();
	void CollpaseAll();
	
	FileViewTree *GetFileView(){return m_fileView;}
};
#endif // __workspacetab__
