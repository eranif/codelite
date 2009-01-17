///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __svnbasedlg__
#define __svnbasedlg__

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
#include <wx/checklst.h>
#include <wx/splitter.h>
#include <wx/combobox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl;
		wxPanel* m_panel2;
		wxStaticText* m_staticText2;
		wxCheckListBox* m_checkList;
		wxStaticText* m_staticText1;
		wxComboBox* m_comboBoxLastCommitMsgs;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLastCommitMsgSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SvnBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Commit Log:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 497,369 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnBaseDlg();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 0 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( SvnBaseDlg::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

#endif //__svnbasedlg__
