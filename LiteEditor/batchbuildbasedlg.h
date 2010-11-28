///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __batchbuildbasedlg__
#define __batchbuildbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BatchBuildBaseDlg
///////////////////////////////////////////////////////////////////////////////
class BatchBuildBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText;
		wxCheckListBox* m_checkListConfigurations;
		wxButton* m_buttonBuild;
		wxButton* m_buttonClean;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_buttonDown;
		wxButton* m_buttonUp;
		wxStaticLine* m_staticline2;
		wxButton* m_buttonClose;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnItemToggled( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBuild( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBuildUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnClean( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCleanUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnUnCheckAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMoveUp( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMoveUpUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnMoveDown( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnMoveDownUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		BatchBuildBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Batch Build"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,512 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~BatchBuildBaseDlg();
	
};

#endif //__batchbuildbasedlg__
