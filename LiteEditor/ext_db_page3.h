///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ext_db_page3__
#define __ext_db_page3__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/wizard.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ExtDbPage3
///////////////////////////////////////////////////////////////////////////////
class ExtDbPage3 : public wxWizardPageSimple 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxPanel* m_panel1;
		wxComboBox* m_comboDbName;
		wxStaticText* m_staticText3;
		wxCheckBox *m_checkSetAsActive;
		void OnValidate(wxWizardEvent &event);

	public:
		ExtDbPage3(wxWizard* parent);
		wxString GetDbName() const {return m_comboDbName->GetValue();}
		bool AttachDb() const {return m_checkSetAsActive->GetValue();}
		DECLARE_EVENT_TABLE();
};

#endif //__ext_db_page3__

