///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 29 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __codedesigneritembasedlg__
#define __codedesigneritembasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CodeDesignerItemBaseDlg
///////////////////////////////////////////////////////////////////////////////
class CodeDesignerItemBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textPackageName;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textDiagramName;
		wxStaticText* m_staticText4;
		wxChoice* m_choiceLanguage;
		
		wxCheckBox* m_checkBoxMain;
		
		wxCheckBox* m_checkGenerate;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textTreeFolder;
		wxButton* m_buttonBrowse;
		wxStaticLine* m_staticline3;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnInit( wxInitDialogEvent& event ) { event.Skip(); }
		virtual void OnUpdateMain( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBrowse( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOk( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CodeDesignerItemBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New CodeDesigner project"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~CodeDesignerItemBaseDlg();
	
};

#endif //__codedesigneritembasedlg__
