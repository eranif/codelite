//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : findinfilesdlg.h              
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
#ifndef FIND_IN_FILES_DLG_H
#define FIND_IN_FILES_DLG_H

#include <wx/dialog.h>
#include "findreplacedlg.h"
#include "search_thread.h"

class DirPicker;

class FindInFilesDialog : public wxDialog
{
	FindReplaceData m_data;

	// Options
	wxComboBox *m_findString;
	wxCheckBox *m_matchCase;
    wxCheckBox *m_printScope;
	wxCheckBox *m_matchWholeWord;
	wxCheckBox *m_regualrExpression;
	wxCheckBox *m_fontEncoding;
	DirPicker  *m_dirPicker;
	wxComboBox *m_fileTypes;
	wxChoice   *m_searchResultsTab;
	
	// Buttons
	wxButton *m_find;
	wxButton *m_stop;
	wxButton *m_cancel;
	wxButton *m_replaceAll;
    
	void CreateGUIControls(size_t numpages);
	void ConnectEvents();
	void DoSearch();
	void DoSearchReplace();
    
	SearchData DoGetSearchData();
    
	DECLARE_EVENT_TABLE()
	void OnClick(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnCharEvent(wxKeyEvent &event);
	
public:
	FindInFilesDialog(wxWindow* parent, wxWindowID id, const FindReplaceData& data, size_t numpages = 5);
	virtual ~FindInFilesDialog();

    void SetSearchData(const SearchData &data);
    void SetRootDir   (const wxString &rootDir);
    
	FindReplaceData& GetData() { return m_data; }

	virtual bool Show();
};

#endif // FIND_IN_FILES_DLG_H
