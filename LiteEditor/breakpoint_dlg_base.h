///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __breakpoint_dlg_base__
#define __breakpoint_dlg_base__

class BreakpointsListctrl;

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/radiobox.h>
#include <wx/choicebk.h>
#include <wx/spinctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BreakpointTab
///////////////////////////////////////////////////////////////////////////////
class BreakpointTab : public wxPanel 
{
	private:
	
	protected:
		BreakpointsListctrl* m_listCtrlBreakpoints;
		wxButton* m_buttonAdd;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDelete;
		wxButton* m_buttonDeleteAll;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeselected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemRightClick( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnAdd( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEdit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteAll( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		BreakpointTab( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BreakpointTab();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class BreakpointPropertiesDlg
///////////////////////////////////////////////////////////////////////////////
class BreakpointPropertiesDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText9;
		wxChoicebook* m_choicebook;
		wxPanel* m_panel1;
		wxStaticText* m_staticText51;
		wxCheckBox* m_checkLineno;
		wxTextCtrl* m_textLineno;
		wxCheckBox* m_checkBreakFunction;
		wxTextCtrl* m_textFunctionname;
		
		wxCheckBox* m_checkRegex;
		wxCheckBox* m_checkBreakMemory;
		wxTextCtrl* m_textBreakMemory;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textFilename;
		wxButton* m_buttonBrowse;
		wxStaticLine* m_staticline5;
		wxPanel* m_panel2;
		wxRadioBox* m_radioWatchtype;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textWatchExpression;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCond;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCommands;
		wxCheckBox* m_checkDisable;
		wxCheckBox* m_checkTemp;
		wxStaticText* m_staticText7;
		wxSpinCtrl* m_spinIgnore;
		wxStaticLine* m_staticline4;
		wxButton* m_button6;
		wxButton* m_button7;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPageChanged( wxChoicebookEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakLineno( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakLinenoUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakFunction( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakFunctionUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakMemory( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckBreakMemoryUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnBrowse( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		BreakpointPropertiesDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Breakpoint and Watchpoint Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~BreakpointPropertiesDlg();
	
};

#endif //__breakpoint_dlg_base__
