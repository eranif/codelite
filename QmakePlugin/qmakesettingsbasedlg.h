///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __qmakesettingsbasedlg__
#define __qmakesettingsbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class QMakeSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class QMakeSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText5;
		wxNotebook* m_notebook;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonNew;
		wxButton* m_buttonCancel;
		wxButton* m_buttonOk;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRightDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnNewQmakeSettings( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		QMakeSettingsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("QMake Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~QMakeSettingsBaseDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class QmakeSettingsTabBase
///////////////////////////////////////////////////////////////////////////////
class QmakeSettingsTabBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* m_filePickerQmakeExec;
		wxStaticText* m_staticText2;
		wxComboBox* m_comboBoxQmakespec;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlQtdir;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileSelected( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		QmakeSettingsTabBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~QmakeSettingsTabBase();
	
};

#endif //__qmakesettingsbasedlg__
