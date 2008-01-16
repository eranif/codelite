///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __pluginwizard_page1__
#define __pluginwizard_page1__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/wizard.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PluginWizardPage1
///////////////////////////////////////////////////////////////////////////////
class PluginWizardPage1 : public wxWizardPageSimple {
protected:
	void OnValidate(wxWizardEvent &event);
protected:
	wxStaticText* m_staticText5;
	wxStaticLine* m_staticline2;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_textCtrlPluginName;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textCtrlDescription;

public:
	PluginWizardPage1( wxWizard* parent);
	
	wxString GetName() const{return m_textCtrlPluginName->GetValue();}
	wxString GetDescription() const{return m_textCtrlDescription->GetValue();}
	
	DECLARE_EVENT_TABLE()
};

#endif //__pluginwizard_page1__

