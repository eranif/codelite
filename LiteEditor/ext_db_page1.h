///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ext_db_page1__
#define __ext_db_page1__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/wizard.h>
#include "dirpicker.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ExtDbPage1
///////////////////////////////////////////////////////////////////////////////
class ExtDbPage1 : public wxWizardPageSimple
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxPanel* m_panel1;
		DirPicker* m_dirPicker;
		wxStaticText* m_staticText3;
		void OnValidate(wxWizardEvent &event);

	public:
		ExtDbPage1(wxWizard* parent);
		wxString GetPath() const { return m_dirPicker->GetPath(); }

		DECLARE_EVENT_TABLE();
	
};

#endif //__ext_db_page1__
