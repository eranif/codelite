///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __workspacesettingsbase__
#define __workspacesettingsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class WorkspaceSettingsBase
///////////////////////////////////////////////////////////////////////////////
class WorkspaceSettingsBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxListBox* m_listBoxSearchPaths;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxStaticText* m_staticText2;
		wxListBox* m_listBoxExcludePaths;
		wxButton* m_button6;
		wxButton* m_button7;
		wxButton* m_button8;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddIncludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveIncludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveIncludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAllIncludePaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllIncludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnAddExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePath( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemoveExcludePathUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePaths( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClearAllExcludePathsUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		WorkspaceSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Workspace Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~WorkspaceSettingsBase();
	
};

#endif //__workspacesettingsbase__
