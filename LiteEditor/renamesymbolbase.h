///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __renamesymbolbase__
#define __renamesymbolbase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checklst.h>
#include "cl_editor.h"
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RenameSymbolBase
///////////////////////////////////////////////////////////////////////////////
class RenameSymbolBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrlNewName;
		wxStaticText* m_staticText1;
		wxCheckListBox* m_checkListCandidates;
		wxStaticText* m_staticText3;
		LEditor *m_preview;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnItemDClicked( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnItemChecked( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		RenameSymbolBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Rename Symbol"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 895,579 ), long style = wxDEFAULT_DIALOG_STYLE );
		~RenameSymbolBase();
	
};

#endif //__renamesymbolbase__
