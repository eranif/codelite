///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __configuration_manager_base_dlg__
#define __configuration_manager_base_dlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigManagerBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ConfigManagerBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxChoice* m_choiceConfigurations;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText20;
		wxScrolledWindow* m_scrolledWindow;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
	
	public:
		
		ConfigManagerBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Configuration Manager"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 726,425 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~ConfigManagerBaseDlg();
	
};

#endif //__configuration_manager_base_dlg__
