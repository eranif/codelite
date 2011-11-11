///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MANAGEPERSPECTIVESBASEDLG_H__
#define __MANAGEPERSPECTIVESBASEDLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_RENAME 1000

///////////////////////////////////////////////////////////////////////////////
/// Class ManagePerspectivesBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ManagePerspectivesBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listBox;
		wxButton* m_button3;
		wxButton* m_button4;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnDeletePerspective( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeletePerspectiveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnRenamePerspective( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRenamePerspectiveUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		ManagePerspectivesBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Perspectives..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ManagePerspectivesBaseDlg();
	
};

#endif //__MANAGEPERSPECTIVESBASEDLG_H__
