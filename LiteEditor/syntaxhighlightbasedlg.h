///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SYNTAXHIGHLIGHTBASEDLG_H__
#define __SYNTAXHIGHLIGHTBASEDLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/fontpicker.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/aui/auibook.h>

///////////////////////////////////////////////////////////////////////////

#define ID_buttonTextSelApplyToAll 10000

///////////////////////////////////////////////////////////////////////////////
/// Class SyntaxHighlightBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SyntaxHighlightBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText91;
		wxColourPickerCtrl* m_colourPickerOutputPanesFgColour;
		wxStaticText* m_staticText911;
		wxColourPickerCtrl* m_colourPickerOutputPanesBgColour;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
		wxButton* m_buttonDefaults;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOutputViewColourChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRestoreDefaults( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SyntaxHighlightBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Syntax Highlight:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,373 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~SyntaxHighlightBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class LexerPageBase
///////////////////////////////////////////////////////////////////////////////
class LexerPageBase : public wxPanel 
{
	private:
	
	protected:
		wxAuiNotebook* m_auinotebook1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText6;
		wxFontPickerCtrl* m_globalFontPicker;
		wxStaticText* m_staticText7;
		wxColourPickerCtrl* m_globalBgColourPicker;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_fileSpec;
		wxPanel* m_panel1;
		wxListBox* m_properties;
		wxStaticText* m_staticText2;
		wxFontPickerCtrl* m_fontPicker;
		wxStaticText* m_staticText3;
		wxColourPickerCtrl* m_colourPicker;
		wxStaticText* m_staticText4;
		wxColourPickerCtrl* m_bgColourPicker;
		wxCheckBox* m_eolFilled;
		wxCheckBox* m_styleWithinPreProcessor;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText1;
		wxButton* m_button5;
		wxButton* m_button6;
		wxButton* m_button7;
		wxButton* m_button8;
		wxButton* m_button9;
		wxPanel* m_panel3;
		wxStaticText* m_staticText9;
		wxColourPickerCtrl* m_colourPickerSelTextBgColour;
		wxButton* m_buttonTextSelApplyToAll;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFontChanged( wxFontPickerEvent& event ) { event.Skip(); }
		virtual void OnColourChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEolFilled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStyleWithinPreprocessor( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStyleWithingPreProcessorUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnEditKeyWordsButton0( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditKeyWordsButton1( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditKeyWordsButton2( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditKeyWordsButton3( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEditKeyWordsButton4( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelTextChanged( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnTextSelApplyToAll( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		LexerPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,272 ), long style = wxTAB_TRAVERSAL ); 
		~LexerPageBase();
	
};

#endif //__SYNTAXHIGHLIGHTBASEDLG_H__
