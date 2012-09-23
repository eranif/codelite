///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ABBREVIATIONSSETTINGSBASE_H__
#define __ABBREVIATIONSSETTINGSBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AbbreviationsSettingsBase
///////////////////////////////////////////////////////////////////////////////
class AbbreviationsSettingsBase : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listBoxAbbreviations;
		wxButton* m_buttonNew;
		wxButton* m_buttonDelete;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlName;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlExpansion;
		wxCheckBox* m_checkBoxImmediateInsert;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonSave;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnMarkDirty( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AbbreviationsSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Abbreviations Settings..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 679,484 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~AbbreviationsSettingsBase();
	
};

#endif //__ABBREVIATIONSSETTINGSBASE_H__
