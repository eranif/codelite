///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editoroptionsgeneralguidespanelbase__
#define __editoroptionsgeneralguidespanelbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorOptionsGeneralGuidesPanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorOptionsGeneralGuidesPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_displayLineNumbers;
		wxCheckBox* m_showIndentationGuideLines;
		wxCheckBox* m_checkBoxMatchBraces;
		wxCheckBox* m_checkBoxAutoCompleteBraces;
		wxCheckBox* m_highlightCaretLine;
		wxCheckBox* m_checkBoxAdjustScrollbarSize;
		wxStaticText* m_staticText1;
		wxColourPickerCtrl* m_caretLineColourPicker;
		wxStaticText* m_staticText2;
		wxChoice* m_whitespaceStyle;
		wxStaticText* m_staticText4;
		wxChoice* m_choiceEOL;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHighlightCaretLine( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		EditorOptionsGeneralGuidesPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorOptionsGeneralGuidesPanelBase();
	
};

#endif //__editoroptionsgeneralguidespanelbase__
