///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __env_var_dlg_base__
#define __env_var_dlg_base__

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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/button.h>
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
		wxNotebook* m_notebook1;
		wxPanel* m_panel1;
		wxScintilla* m_textCtrlDefault;
		wxStaticLine* m_staticline4;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		wxButton* m_buttonNewSet;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLeftUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewSet( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		EnvVarsTableDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Environment Variables"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
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
