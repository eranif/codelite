///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __codeforammaterbasedlg__
#define __codeforammaterbasedlg__

#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CodeFormatterBaseDlg
///////////////////////////////////////////////////////////////////////////////
class CodeFormatterBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxRadioBox* m_radioBoxPredefinedStyle;
		wxRadioBox* m_radioBoxBrackets;
		wxCheckBox* m_checkBoxIndetClass;
		wxCheckBox* m_checkBoxIndentBrackets;
		wxCheckBox* m_checkBoxIndetSwitch;
		wxCheckBox* m_checkBoxIndentNamespaces;
		wxCheckBox* m_checkBoxIndetCase;
		wxCheckBox* m_checkBoxIndentLabels;
		wxCheckBox* m_checkBoxIndetBlocks;
		wxCheckBox* m_checkBoxIndentPreprocessors;
		wxCheckBox* m_checkBoxIndentMaxInst;
		wxCheckBox* m_checkBoxIndentMinCond;
		wxCheckBox* m_checkBoxFormatBreakBlocks;
		wxCheckBox* m_checkBoxFormatPadParenth;
		wxCheckBox* m_checkBoxFormatBreakBlocksAll;
		wxCheckBox* m_checkBoxFormatPadParentOut;
		wxCheckBox* m_checkBoxFormatBreakElseif;
		wxCheckBox* m_checkBoxFormatPadParentIn;
		wxCheckBox* m_checkBoxFormatPadOperators;
		wxCheckBox* m_checkBoxFormatUnPadParent;
		wxCheckBox* m_checkBoxFormatOneLineKeepStmnt;
		wxCheckBox* m_checkBoxFormatFillEmptyLines;
		wxCheckBox* m_checkBoxFormatOneLineKeepBlocks;
		wxTextCtrl* m_textCtrlPreview;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonClose;
		wxButton* m_buttonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRadioBoxPredefinedStyle( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRadioBoxBrackets( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckBox( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		CodeFormatterBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("AStyle Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 589,618 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	
};

#endif //__codeforammaterbasedlg__
