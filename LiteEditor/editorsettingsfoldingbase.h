///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __editorsettingsfoldingbase__
#define __editorsettingsfoldingbase__

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
/// Class EditorSettingsFoldingBase
///////////////////////////////////////////////////////////////////////////////
class EditorSettingsFoldingBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_displayMargin;
		wxCheckBox* m_underlineFolded;
		wxCheckBox* m_foldPreprocessors;
		wxCheckBox* m_foldCompact;
		wxCheckBox* m_foldElse;
		wxStaticText* m_staticText2;
		wxChoice* m_foldStyle;
		wxStaticText* m_staticText21;
		wxColourPickerCtrl* m_colourPicker;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFoldColourUI( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		EditorSettingsFoldingBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 366,224 ), long style = wxTAB_TRAVERSAL );
		~EditorSettingsFoldingBase();
	
};

#endif //__editorsettingsfoldingbase__
