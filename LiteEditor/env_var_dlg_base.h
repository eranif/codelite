///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __env_var_dlg_base__
#define __env_var_dlg_base__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/wxscintilla.h>
#include <wx/panel.h>
#include <wx/choicebk.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarsTableDlgBase
///////////////////////////////////////////////////////////////////////////////
class EnvVarsTableDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxChoicebook* m_notebook1;
		wxPanel* m_panel1;
		wxScintilla* m_textCtrlDefault;
		wxButton* m_buttonNewSet;
		wxButton* m_buttonDeleteSet;
		wxButton* m_button5;
		wxStaticLine* m_staticline4;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNewSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeleteSetUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EnvVarsTableDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Environment Variables"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~EnvVarsTableDlgBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class EnvVarSetPage
///////////////////////////////////////////////////////////////////////////////
class EnvVarSetPage : public wxPanel 
{
	private:
	
	protected:
	
	public:
		wxScintilla* m_textCtrl;
		
		EnvVarSetPage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~EnvVarSetPage();
	
};

#endif //__env_var_dlg_base__
