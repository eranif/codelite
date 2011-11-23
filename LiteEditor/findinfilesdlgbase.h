///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FINDINFILESDLGBASE_H__
#define __FINDINFILESDLGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include "dirpicker.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FindInFilesDialogBase
///////////////////////////////////////////////////////////////////////////////
class FindInFilesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxComboBox* m_findString;
		DirPicker  *m_dirPicker;
		wxButton* m_btnAddPath;
		wxListBox* m_listPaths;
		wxButton* m_btnRemovelPath;
		wxButton* m_btnClearPaths;
		wxComboBox* m_fileTypes;
		wxStaticText* m_staticText5;
		wxChoice* m_choiceEncoding;
		wxStaticText* m_staticText7;
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxCheckBox* m_matchCase;
		wxCheckBox* m_matchWholeWord;
		wxCheckBox* m_regualrExpression;
		wxCheckBox* m_checkBoxSaveFilesBeforeSearching;
		wxCheckBox* m_checkBoxSeparateTab;
		wxPanel* m_panel2;
		wxCheckBox* m_printScope;
		wxCheckBox* m_checkBoxSkipMatchesFoundInComments;
		wxCheckBox* m_checkBoxSkipMatchesFoundInStrings;
		wxCheckBox* m_checkBoxHighlighStringComments;
		wxButton* m_find;
		wxButton* m_replaceAll;
		wxButton* m_stop;
		wxButton* m_cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnAddPath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemovePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemovePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearPaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearPathsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseDiffColourForCommentsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnFindWhatUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindInFilesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Find in files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP ); 
		~FindInFilesDialogBase();
	
};

#endif //__FINDINFILESDLGBASE_H__
