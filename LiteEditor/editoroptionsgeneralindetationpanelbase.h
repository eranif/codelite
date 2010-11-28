///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editoroptionsgeneralindetationpanelbase__
#define __editoroptionsgeneralindetationpanelbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorOptionsGeneralIndetationPanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorOptionsGeneralIndetationPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxDisableSmartIdent;
		wxCheckBox* m_indentsUsesTabs;
		wxStaticText* m_staticText1;
		wxSpinCtrl* m_indentWidth;
		wxStaticText* m_staticText2;
		wxSpinCtrl* m_tabWidth;
	
	public:
		
		EditorOptionsGeneralIndetationPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorOptionsGeneralIndetationPanelBase();
	
};

#endif //__editoroptionsgeneralindetationpanelbase__
