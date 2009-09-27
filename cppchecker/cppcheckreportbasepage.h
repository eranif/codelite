///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cppcheckreportbasepage__
#define __cppcheckreportbasepage__

#include <wx/intl.h>

#include "cppcheckfilelistctrl.h"
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include "progressctrl.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CppCheckReportBasePage
///////////////////////////////////////////////////////////////////////////////
class CppCheckReportBasePage : public wxPanel 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		CppCheckFileListCtrl* m_filelist;
		wxPanel* m_panel2;
		wxListCtrl* m_listCtrlReport;
		wxButton* m_buttonClear;
		wxButton* m_buttonStop;
		wxButton* m_buttonSkip;
		ProgressCtrl *m_progress;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnListCtrlItemActivated( wxListEvent& event ){ event.Skip(); }
		virtual void OnClearReport( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClearReportUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnStopChecking( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnStopCheckingUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnSkipFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSkipFileUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		CppCheckReportBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 482,238 ), long style = wxTAB_TRAVERSAL );
		~CppCheckReportBasePage();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 90 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CppCheckReportBasePage::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

#endif //__cppcheckreportbasepage__
