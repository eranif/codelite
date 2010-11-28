///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cppcheckreportbasepage__
#define __cppcheckreportbasepage__

#include <wx/intl.h>

#include <wx/wxscintilla.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CppCheckReportBasePage
///////////////////////////////////////////////////////////////////////////////
class CppCheckReportBasePage : public wxPanel 
{
	private:
	
	protected:
		wxScintilla *m_outputText;
		wxStaticText* m_staticTextFile;
		wxGauge* m_gauge;
		wxButton* m_buttonClear;
		wxButton* m_buttonStop;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClearReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearReportUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnStopChecking( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStopCheckingUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CppCheckReportBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 482,238 ), long style = wxTAB_TRAVERSAL );
		~CppCheckReportBasePage();
	
};

#endif //__cppcheckreportbasepage__
