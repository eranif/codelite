///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __abbreviationssettingsbase__
#define __abbreviationssettingsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
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
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlName;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlExpansion;
		wxButton* m_buttonNew;
		wxButton* m_buttonDelete;
		wxButton* m_buttonSave;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMarkDirty( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNew( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSaveUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		AbbreviationsSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Abbreviations Settings..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~AbbreviationsSettingsBase();
	
};

#endif //__abbreviationssettingsbase__
