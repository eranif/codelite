///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __quickfinder_basedlg__
#define __quickfinder_basedlg__

#include <wx/intl.h>

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SelectSymbolDlgBase
///////////////////////////////////////////////////////////////////////////////
class SelectSymbolDlgBase : public wxDialog
{
	private:

	protected:
		wxListView* m_listCtrlTags;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ){ event.Skip(); }


	public:
		SelectSymbolDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select Symbol:"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxCLOSE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~SelectSymbolDlgBase();

};

#endif //__quickfinder_basedlg__
