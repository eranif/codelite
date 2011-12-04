///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cscopetabbase__
#define __cscopetabbase__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include "cl_treelistctrl.h"
#include <wx/gauge.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CscopeTabBase
///////////////////////////////////////////////////////////////////////////////
class CscopeTabBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxChoice* m_choiceSearchScope;
		wxCheckBox* m_checkBoxUpdateDb;
		wxCheckBox* m_checkBoxRevertedIndex;
		wxButton* m_buttonUpdateDbNow;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonClear;
		clTreeListCtrl* m_treeCtrlResults;
		wxStaticText* m_statusMessage;
		wxGauge* m_gauge;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeSearchScope( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWorkspaceOpenUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCreateDB( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearResults( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearResultsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CscopeTabBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL ); 
		~CscopeTabBase();
	
};

#endif //__cscopetabbase__
