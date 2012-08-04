//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : fileexplorer.h              
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
 #ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "wx/panel.h"
#include "wx/choice.h"
#include "volumelocatorthread.h"

class FileExplorerTree;

class FileExplorer : public wxPanel
{
private:
	FileExplorerTree *m_fileTree;
	wxString m_caption;
	bool m_isLinkedToEditor;

	void CreateGUIControls();
 	
    void OnLinkEditor(wxCommandEvent &e);
    void OnCollapseAll(wxCommandEvent &e);
	void OnGoHome(wxCommandEvent &e);
    void OnShowFile(wxCommandEvent &e);
    void OnShowFileUI(wxUpdateUIEvent &e);
    void OnWorkspaceLoaded(wxCommandEvent &e);
    void OnActiveEditorChanged(wxCommandEvent &e);
	void OnRootChanged(wxCommandEvent &e);

#ifdef __WXMSW__
#if wxUSE_FSVOLUME
	wxChoice *m_volumes;

	void OnVolumeChanged(wxCommandEvent &e);
	void OnVolumes(wxCommandEvent &e);
#endif // wxUSE_FSVOLUME
#endif // __WXMSW__

public:
	FileExplorer(wxWindow *parent, const wxString &caption);
	virtual ~FileExplorer();
    
	const wxString &GetCaption() const{return m_caption;}
	FileExplorerTree *GetFileTree() {return m_fileTree;}
};

#endif //FILEEXPLORER_H

