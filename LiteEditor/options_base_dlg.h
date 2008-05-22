///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __options_base_dlg__
#define __options_base_dlg__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include "lexer_configuration.h"
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OptionsDlg
///////////////////////////////////////////////////////////////////////////////
class wxNotebook;
class CommentPage;

class OptionsDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE()

	// Private event handlers
	void OnButtonOK( wxCommandEvent& event );
	void OnButtonCancel( wxCommandEvent& event );
	void OnButtonApply( wxCommandEvent& event );
	void OnThemeChanged( wxCommandEvent &event);
	
	void LoadLexers(const wxString &theme);
	
protected:
	wxNotebook* m_book;
	wxNotebook *m_lexersBook;
	wxPanel* m_general;
	wxCheckBox* m_checkBoxDisplayFoldMargin;
	wxCheckBox* m_checkBoxMarkFoldedLine;
	wxStaticText* m_staticText1;
	wxChoice* m_foldStyleChoice;
	wxCheckBox* m_displayBookmarkMargin;
	wxStaticText* m_staticText6;
	wxChoice* m_bookmarkShape;
	wxStaticText* m_staticText4;
	wxColourPickerCtrl* m_bgColourPicker;
	wxStaticText* m_staticText5;
	wxColourPickerCtrl* m_fgColourPicker;
	wxCheckBox* m_highlighyCaretLine;
	wxCheckBox* m_displayLineNumbers;
	wxCheckBox* m_showIndentationGuideLines;
	wxCheckBox* m_indentsUsesTabs;
	wxPanel* m_syntaxHighlightPage;
	wxStaticLine* m_staticline1;
	wxButton* m_okButton;
	wxButton* m_cancelButton;
	wxButton* m_applyButton;
	wxChoice* m_iconSize;
	wxCheckBox* m_checkBoxShowSplash;
	CommentPage *m_commentPage;
	wxChoice* m_themes;
	wxString m_startingTheme;
	wxSpinCtrl* m_spinCtrlTabWidth;
	wxColourPickerCtrl *m_caretLineColourPicker;
	wxColourPickerCtrl *m_wordHighlightColour;
	
private:
	wxPanel *CreateSyntaxHighlightPage();
	wxPanel *CreateGeneralPage();
	wxPanel *CreateLexerPage(wxWindow *parent, LexerConfPtr lexer);
	wxPanel *CreateBookmarksPage();
	wxPanel *CreateFoldingPage();
	wxPanel *CreateCxxCommentPage();
	void SaveChanges();

public:
	OptionsDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Editor's Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__options_base_dlg__
