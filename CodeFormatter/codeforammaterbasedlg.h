///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CODEFORAMMATERBASEDLG_H__
#define __CODEFORAMMATERBASEDLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/wxscintilla.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/splitter.h>
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
		wxSplitterWindow* m_splitterSettingsPreview;
		wxPanel* m_panelSettings;
		wxNotebook* m_notebookSettings;
		wxPanel* m_panelPredefinedStyles;
		wxStaticText* m_staticText2;
		wxRadioBox* m_radioBoxPredefinedStyle;
		wxStaticText* m_staticTextPredefineHelp;
		wxPanel* m_panelBrackets;
		wxRadioBox* m_radioBoxBrackets;
		wxPanel* m_panelIndentation;
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
		wxPanel* m_panelFormatting;
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
		wxPanel* m_panelCustomSettings;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlUserFlags;
		wxPanel* m_panelPreview;
		wxScintilla* m_textCtrlPreview;
		wxButton* m_buttonOK;
		wxButton* m_buttonClose;
		wxButton* m_buttonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRadioBoxPredefinedStyle( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRadioBoxBrackets( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBox( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CodeFormatterBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Formatter Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~CodeFormatterBaseDlg();
		
		void m_splitterSettingsPreviewOnIdle( wxIdleEvent& )
		{
			m_splitterSettingsPreview->SetSashPosition( 450 );
			m_splitterSettingsPreview->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CodeFormatterBaseDlg::m_splitterSettingsPreviewOnIdle ), NULL, this );
		}
	
};

#endif //__CODEFORAMMATERBASEDLG_H__
