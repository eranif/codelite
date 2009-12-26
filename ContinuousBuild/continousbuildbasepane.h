///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __continousbuildbasepane__
#define __continousbuildbasepane__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ContinousBuildBasePane
///////////////////////////////////////////////////////////////////////////////
class ContinousBuildBasePane : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBox1;
		
		wxButton* m_buttonCancel;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText31;
		wxListBox* m_listBoxQueue;
		wxStaticText* m_staticText4;
		wxListBox* m_listBoxFailedFiles;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnEnableCB( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnEnableContBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		ContinousBuildBasePane( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 389,188 ), long style = wxTAB_TRAVERSAL );
		~ContinousBuildBasePane();
	
};

#endif //__continousbuildbasepane__
