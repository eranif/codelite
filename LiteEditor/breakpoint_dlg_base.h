///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __breakpoint_dlg_base__
#define __breakpoint_dlg_base__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BreakpointDlgBase
///////////////////////////////////////////////////////////////////////////////
class BreakpointDlgBase : public wxPanel 
{
	private:
	
	protected:
		wxListBox* m_listBreakpoints;
		wxButton* m_buttonDelete;
		wxButton* m_buttonDeleteAll;
		wxStaticLine* m_staticline10;
		wxButton* m_button27;
	
	public:
		BreakpointDlgBase( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 352,285 ), int style = 0 );
};

#endif //__breakpoint_dlg_base__
