///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __implementparentvirtualfunctionsbase__
#define __implementparentvirtualfunctionsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ImplementParentVirtualFunctionsBase
///////////////////////////////////////////////////////////////////////////////
class ImplementParentVirtualFunctionsBase : public wxDialog 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlProtos;
		wxPanel* m_panel2;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlImpl;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddDoxy( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxTextCtrl* m_textCtrlImplFile;
		wxCheckBox* m_checkBoxFormat;
		wxCheckBox* m_checkBoxAddDoxy;
		
		ImplementParentVirtualFunctionsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Implement Parent Virtual Functions"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ImplementParentVirtualFunctionsBase();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ImplementParentVirtualFunctionsBase::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

#endif //__implementparentvirtualfunctionsbase__
