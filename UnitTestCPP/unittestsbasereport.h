///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __unittestsbasereport__
#define __unittestsbasereport__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include "progressctrl.h"
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UnitTestsBasePage
///////////////////////////////////////////////////////////////////////////////
class UnitTestsBasePage : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText7;
		ProgressCtrl *m_progressPassed;
		wxStaticText* m_staticText8;
		ProgressCtrl *m_progressFailed;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticTextTotalTests;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticTextFailTestsNum;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticTextSuccessTestsNum;
		wxStaticLine* m_staticline4;
		wxStaticText* m_staticText16;
		wxListCtrl* m_listCtrlErrors;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ){ event.Skip(); }
		
	
	public:
		
		UnitTestsBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 969,472 ), long style = wxTAB_TRAVERSAL );
		~UnitTestsBasePage();
	
};

#endif //__unittestsbasereport__
