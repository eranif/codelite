///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 28 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ext_db_page2__
#define __ext_db_page2__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/wizard.h>

class CheckDirTreeCtrl;

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class ExtDbPage2
///////////////////////////////////////////////////////////////////////////////
class ExtDbPage2 : public wxWizardPageSimple 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText3;
		CheckDirTreeCtrl *m_includeDirs;

	public:
		ExtDbPage2( wxWizard* parent);
		void BuildTree(const wxString &path);
		void GetIncludeDirs(wxArrayString &arr);
};

#endif //__ext_db_page2__
