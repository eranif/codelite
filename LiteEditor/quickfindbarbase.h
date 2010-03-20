///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickfindbarbase__
#define __quickfindbarbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include "cl_defs.h"

//#ifdef USE_AUI_TOOLBAR
//#undef USE_AUI_TOOLBAR
//#undef clToolBar
//#define clToolBar wxToolBar
//#undef clTB_DEFAULT_STYLE
//#define clTB_DEFAULT_STYLE 0
//#define USE_AUI_TOOLBAR 0
//#endif

#if USE_AUI_TOOLBAR
#include <wx/pen.h>
#include <wx/aui/auibar.h>
#endif
///////////////////////////////////////////////////////////////////////////

#define wxID_HIDE 1000
#define wxID_SHOW_REPLACE_CONTROLS 1001
#define wxID_FIND_NEXT 1002
#define wxID_FIND_PREVIOUS 1003
#define wxID_TOOL_REPLACE 1004
#define wxID_HIGHLIGHT_MATCHES 1005

///////////////////////////////////////////////////////////////////////////////
/// Class QuickFindBarBase
///////////////////////////////////////////////////////////////////////////////
class QuickFindBarBase : public wxPanel
{
	private:

	protected:
		clToolBar*    m_toolBar1;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticTextFind;
		wxTextCtrl*   m_findWhat;
		clToolBar*    m_toolBar2;
		wxStaticText* m_replaceStaticText;
		wxTextCtrl*   m_replaceWith;
		clToolBar*    m_toolBarReplace;
		wxStaticLine* m_staticline1;
		wxCheckBox*   m_checkBoxCase;
		wxCheckBox*   m_checkBoxWord;
		wxCheckBox*   m_checkBoxRegex;

		// Virtual event handlers, overide them in your derived class
		virtual void OnHide( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggleReplaceControls( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToggleReplaceControlsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnHighlightMatches( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHighlightMatchesUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReplaceUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnReplace( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxCase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxWord( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckBoxRegex( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxBoxSizer* optionsSizer;

		QuickFindBarBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL );
		~QuickFindBarBase();

};

#endif //__quickfindbarbase__
