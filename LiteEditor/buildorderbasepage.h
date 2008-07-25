///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __buildorderbasepage__
#define __buildorderbasepage__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/checklst.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesPageBase
///////////////////////////////////////////////////////////////////////////////
class DependenciesPageBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxChoice* m_choiceProjectConfig;
		wxStaticText* m_staticText1;
		wxCheckListBox* m_checkListProjectList;
		wxStaticText* m_staticText2;
		wxListBox* m_listBoxBuildOrder;
		wxButton* m_buttonDown;
		wxButton* m_buttonUp;
		wxButton* m_buttonApply;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnConfigChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckListItemToggled( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMoveDown( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMoveUp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnApplyButton( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnApplyButtonUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		DependenciesPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 810,492 ), long style = wxTAB_TRAVERSAL );
		~DependenciesPageBase();
	
};

#endif //__buildorderbasepage__
