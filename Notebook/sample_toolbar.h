///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __sample_toolbar__
#define __sample_toolbar__

#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MyToolbar
///////////////////////////////////////////////////////////////////////////////
class MyToolbar : public wxPanel 
{
	private:
	
	protected:
		wxChoice* m_choice1;
		wxToolBar* m_toolBar1;
	
	public:
		MyToolbar( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 361,44 ), long style = wxTAB_TRAVERSAL );
		~MyToolbar();
	
};

#endif //__sample_toolbar__
