///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __messagepane_base__
#define __messagepane_base__

#include <wx/intl.h>

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MessagePaneBase
///////////////////////////////////////////////////////////////////////////////
class MessagePaneBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panel2;
		wxStaticBitmap* m_bitmap1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticTextMessage;
		wxBitmapButton* m_bpButtonClose;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonAction;
		wxButton* m_buttonAction1;
		wxButton* m_buttonAction2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnEraseBG( wxEraseEvent& event ) { event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnButtonClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionButton1( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActionButton2( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		MessagePaneBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~MessagePaneBase();
	
};

#endif //__messagepane_base__
