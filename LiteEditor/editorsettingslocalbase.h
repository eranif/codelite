///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingslocalbase__
#define __editorsettingslocalbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LocalEditorSettingsbase
///////////////////////////////////////////////////////////////////////////////
class LocalEditorSettingsbase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText;
		wxCheckBox* m_indentsUsesTabsEnable;
		wxCheckBox* m_indentsUsesTabs;
		wxCheckBox* m_tabWidthEnable;
		wxStaticText* m_staticTexttabWidth;
		
		wxSpinCtrl* m_tabWidth;
		wxCheckBox* m_indentWidthEnable;
		wxStaticText* m_staticTextindentWidth;
		
		wxSpinCtrl* m_indentWidth;
		wxCheckBox* m_displayBookmarkMarginEnable;
		wxCheckBox* m_displayBookmarkMargin;
		wxCheckBox* m_checkBoxDisplayFoldMarginEnable;
		wxCheckBox* m_checkBoxDisplayFoldMargin;
		wxCheckBox* m_checkBoxHideChangeMarkerMarginEnable;
		wxCheckBox* m_checkBoxHideChangeMarkerMargin;
		wxCheckBox* m_displayLineNumbersEnable;
		wxCheckBox* m_displayLineNumbers;
		wxCheckBox* m_showIndentationGuideLinesEnable;
		wxCheckBox* m_showIndentationGuideLines;
		wxCheckBox* m_highlightCaretLineEnable;
		wxCheckBox* m_highlightCaretLine;
		wxCheckBox* m_checkBoxTrimLineEnable;
		wxCheckBox* m_checkBoxTrimLine;
		wxCheckBox* m_checkBoxAppendLFEnable;
		wxCheckBox* m_checkBoxAppendLF;
		wxCheckBox* m_whitespaceStyleEnable;
		wxStaticText* m_staticTextwhitespaceStyle;
		wxChoice* m_whitespaceStyle;
		wxCheckBox* m_choiceEOLEnable;
		wxStaticText* m_EOLstatic;
		wxChoice* m_choiceEOL;
		wxCheckBox* m_fileEncodingEnable;
		wxStaticText* m_staticTextfileEncoding;
		wxChoice* m_fileEncoding;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void indentsUsesTabsUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void tabWidthUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void indentWidthUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void displayBookmarkMarginUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void checkBoxDisplayFoldMarginUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void checkBoxHideChangeMarkerMarginUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void displayLineNumbersUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void showIndentationGuideLinesUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void highlightCaretLineUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void checkBoxTrimLineUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void checkBoxAppendLFUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void whitespaceStyleUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void choiceEOLUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void fileEncodingUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		LocalEditorSettingsbase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Local Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~LocalEditorSettingsbase();
	
};

#endif //__editorsettingslocalbase__
