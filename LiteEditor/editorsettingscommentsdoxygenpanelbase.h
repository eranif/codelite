///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingscommentsdoxygenpanelbase__
#define __editorsettingscommentsdoxygenpanelbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EditorSettingsCommentsDoxygenPanelBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsCommentsDoxygenPanelBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxUseSlash2Stars;
		wxCheckBox* m_checkBoxUseShtroodel;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticText3;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlClassPattern;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlFunctionPattern;
		wxStaticText* m_staticText1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUseAtPrefix( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		
		EditorSettingsCommentsDoxygenPanelBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsCommentsDoxygenPanelBase();
	
};

#endif //__editorsettingscommentsdoxygenpanelbase__
