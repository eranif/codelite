///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __attachdbgprocbasedlg__
#define __attachdbgprocbasedlg__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AttachDbgProcBaseDlg
///////////////////////////////////////////////////////////////////////////////
class AttachDbgProcBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxChoice* m_choiceDebugger;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrlFilter;
		wxStaticText* m_staticText4;
		wxListCtrl* m_listCtrlProcesses;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_button2;
		wxButton* m_buttonRefresh;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSortColumn( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemDeselected( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnBtnAttachUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRefresh( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AttachDbgProcBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Attach debugger to process:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 445,471 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~AttachDbgProcBaseDlg();
	
};

#endif //__attachdbgprocbasedlg__
