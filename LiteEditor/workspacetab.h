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

class WorkspaceTab : public wxPanel 
{
	FileViewTree *m_fileView;
    wxString      m_caption;
    bool          m_isLinkedToEditor;
	
	void CreateGUIControls();
    void ConnectEvents();
    
	void OnLinkEditor           (wxCommandEvent  &e);
	void OnCollapseAll          (wxCommandEvent  &e);
	void OnCollapseAllUI        (wxUpdateUIEvent &e);
	void OnGoHome               (wxCommandEvent  &e);
	void OnGoHomeUI             (wxUpdateUIEvent &e);
	void OnProjectSettingsUI    (wxUpdateUIEvent &e);
	void OnProjectSettings      (wxCommandEvent  &e);
    void OnShowFile             (wxCommandEvent  &e);
    void OnShowFileUI           (wxUpdateUIEvent &e);
	void OnShowProjectListPopup (wxCommandEvent  &e);
	void OnMenuSelection        (wxCommandEvent  &e);
    
    void OnWorkspaceLoaded      (wxCommandEvent  &e);
    void OnWorkspaceClosed      (wxCommandEvent  &e);
    void OnProjectAdded         (wxCommandEvent  &e);
    void OnProjectRemoved       (wxCommandEvent  &e);
    void OnProjectFileAdded     (wxCommandEvent  &e);
    void OnProjectFileRemoved   (wxCommandEvent  &e);
    void OnActiveEditorChanged  (wxCommandEvent  &e);
    void OnEditorClosing        (wxCommandEvent  &e);
    void OnAllEditorsClosed     (wxCommandEvent  &e);
	

public:
	WorkspaceTab(wxWindow *parent, const wxString &caption);
	~WorkspaceTab();

	FileViewTree   *GetFileView()       { return m_fileView; }
    const wxString &GetCaption () const { return m_caption;  }
};
#endif // __workspacetab__
