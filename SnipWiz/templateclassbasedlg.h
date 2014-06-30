//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : templateclassbasedlg.h
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __templateclassbasedlg__
#define __templateclassbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TemplateClassBaseDlg
///////////////////////////////////////////////////////////////////////////////
class TemplateClassBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* ID_NOTEBOOK1;
		wxPanel* ID_PANEL;
		wxStaticText* m_static1;
		wxTextCtrl* m_textCtrlClassName;
		wxStaticText* m_static2;
		wxComboBox* m_comboxCurrentTemplate;
		wxStaticText* m_static3;
		wxTextCtrl* m_textCtrlHeaderFile;
		
		wxStaticText* m_static4;
		wxTextCtrl* m_textCtrlCppFile;
		
		wxStaticText* m_staticProjectTreeFolder;
		wxTextCtrl* m_textCtrlVD;
		wxButton* m_buttonBrowseVD;
		wxStaticText* m_static5;
		wxTextCtrl* m_textCtrlFilePath;
		wxButton* m_buttonBrowsePath;
		wxButton* m_buttonGenerate;
		wxButton* m_buttonCancel;
		wxPanel* ID_PANEL1;
		wxStaticText* m_static6;
		wxComboBox* m_comboxTemplates;
		wxButton* m_buttonAddTemplate;
		wxButton* m_buttonChangeTemplate;
		wxButton* m_buttonRemoveTemplate;
		wxButton* m_buttonClear;
		wxButton* m_buttonInsertClassMacro;
		wxNotebook* m_notebookFiles;
		wxPanel* m_panel3;
		wxTextCtrl* m_textCtrlHeader;
		wxPanel* m_panel4;
		wxTextCtrl* m_textCtrlImpl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClassNameEntered( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBrowseVD( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBrowseFilePath( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnGenerate( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnGenerateUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTemplateClassSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonAdd( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonAddUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonChangeUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonRemove( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonRemoveUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonClear( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonClearUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnInsertClassKeyword( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnInsertClassKeywordUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnHeaderFileContentChnaged( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnImplFileContentChnaged( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		TemplateClassBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Template Class Wizard "), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL ); 
		~TemplateClassBaseDlg();
	
};

#endif //__templateclassbasedlg__
