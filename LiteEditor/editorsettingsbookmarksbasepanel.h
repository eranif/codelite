///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingsbookmarksbasepanel__
#define __editorsettingsbookmarksbasepanel__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsBookmarksBasePanel
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsBookmarksBasePanel : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_displaySelection;
		wxStaticText* m_staticText3;
		wxChoice* m_bookMarkShape;
		wxStaticText* m_staticText4;
		wxColourPickerCtrl* m_backgroundColor;
		wxStaticText* m_staticText5;
		wxColourPickerCtrl* m_foregroundColor;
		wxStaticText* m_staticText6;
		wxColourPickerCtrl* m_highlightColor;
	
	public:
		
		EditorSettingsBookmarksBasePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsBookmarksBasePanel();
	
};

#endif //__editorsettingsbookmarksbasepanel__
