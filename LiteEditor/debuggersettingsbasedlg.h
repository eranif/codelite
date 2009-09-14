///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __debuggersettingsbasedlg__
#define __debuggersettingsbasedlg__

#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class DebuggerSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
		wxPanel* m_panel1;
		wxNotebook* m_book;
		wxPanel* m_panel2;
		wxListCtrl* m_listCtrl1;
		wxButton* m_buttonNewType;
		wxButton* m_buttonEdit;
		wxButton* m_buttonDelete;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemDeselected( wxListEvent& event ){ event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ){ event.Skip(); }
		virtual void OnNewShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDeleteShortcut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DebuggerSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Debugger Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 537,451 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DebuggerSettingsBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DebuggerPageBase
///////////////////////////////////////////////////////////////////////////////
class DebuggerPageBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrDbgPath;
		wxButton* m_buttonBrowse;
		wxCheckBox* m_checkBoxEnablePendingBreakpoints;
		wxCheckBox* m_checkBreakAtWinMain;
		wxCheckBox* m_checkResolveStarThis;
		wxCheckBox* m_catchThrow;
		wxCheckBox* m_checkBoxDebugAssert;
		wxCheckBox* m_checkBoxEnableLog;
		wxCheckBox* m_checkShowTerminal;
		wxCheckBox* m_checkUseRelativePaths;
		wxCheckBox* m_showTooltips;
		wxTextCtrl* m_textCtrlStartupCommands;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowse( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDebugAssert( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DebuggerPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~DebuggerPageBase();
	
};

#endif //__debuggersettingsbasedlg__
