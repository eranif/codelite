///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __qmaketabbase__
#define __qmaketabbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QMakeTabBase
///////////////////////////////////////////////////////////////////////////////
class QMakeTabBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_checkBoxUseQmake;
		wxStaticText* m_staticText3;
		wxChoice* m_choiceQmakeSettings;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlQmakeExeLine;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlFreeText;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUseQmake( wxUpdateUIEvent& event ){ event.Skip(); }
		
	
	public:
		QMakeTabBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~QMakeTabBase();
	
};

#endif //__qmaketabbase__
