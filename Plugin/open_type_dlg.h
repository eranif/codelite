//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : open_type_dlg.h              
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
 #ifndef __open_type_dlg__
#define __open_type_dlg__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "ctags_manager.h"
#include "map"

///////////////////////////////////////////////////////////////////////////////
/// Class OpenTypeDlg
///////////////////////////////////////////////////////////////////////////////
class wxImageList;
class wxTimer;
class OpenTypeVListCtrl;

class OpenTypeDlg : public wxDialog 
{
protected:
	wxStaticText* m_staticText;
	wxTextCtrl* m_textTypeName;
	OpenTypeVListCtrl* m_listTypes;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_button2;
	wxString m_filter ;
	wxImageList *m_il;
	TagEntryPtr m_tag;
	std::map<wxString, TagEntryPtr> m_itemsData;
	TagsManager *m_tagsManager;
	wxTimer *m_timer;
	long m_selectedItem;
	wxArrayString m_kind;
	
protected:
	void Ctor();
	void Init();
	void OnCharHook(wxKeyEvent &event);
	void OnItemActivated(wxListEvent &event);
	void PopulateList();
	void OnOK(wxCommandEvent &event);
	void TryOpenAndEndModal();
	void OnAllowPartialMatch(wxCommandEvent &e);
	void OnText(wxCommandEvent &e);
	
public:
	OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, int id = wxID_ANY, wxString title = wxT("Open Type"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 682, 353 ), int style = wxDEFAULT_DIALOG_STYLE);
	OpenTypeDlg( wxWindow* parent, TagsManager *tagsMgr, const wxArrayString &kind, int id = wxID_ANY, wxString title = wxT("Open Type"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 682, 353 ), int style = wxDEFAULT_DIALOG_STYLE);
	virtual ~OpenTypeDlg();
	TagEntryPtr GetSelectedTag() const {return m_tag;}
	DECLARE_EVENT_TABLE()
};

#endif //__open_type_dlg__
