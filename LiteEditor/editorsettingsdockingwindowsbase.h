///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingsdockingwindowsbase__
#define __editorsettingsdockingwindowsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsDockingWindowsBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsDockingWindowsBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxHideOutputPaneOnClick;
		wxCheckBox* m_checkBoxShowQuickFinder;
	
	public:
		EditorSettingsDockingWindowsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsDockingWindowsBase();
	
};

#endif //__editorsettingsdockingwindowsbase__
