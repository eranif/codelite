///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __notebookcustomdlgbase__
#define __notebookcustomdlgbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class NotebookCustomDlgBase
///////////////////////////////////////////////////////////////////////////////
class NotebookCustomDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxFixedWidth;
		wxRadioBox* m_radioBox1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		NotebookCustomDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Customize Notebook"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~NotebookCustomDlgBase();
	
};

#endif //__notebookcustomdlgbase__
