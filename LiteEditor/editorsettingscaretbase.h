///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingscaretbase__
#define __editorsettingscaretbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsCaretBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsCaretBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxSpinCtrl* m_spinCtrlBlinkPeriod;
		wxStaticText* m_staticText3;
		wxSpinCtrl* m_spinCtrlCaretWidth;
	
	public:
		EditorSettingsCaretBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsCaretBase();
	
};

#endif //__editorsettingscaretbase__
