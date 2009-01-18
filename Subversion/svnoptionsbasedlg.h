///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __svnoptionsbasedlg__
#define __svnoptionsbasedlg__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxListbook* m_listbook1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* m_filePicker;
		wxCheckBox* m_checkBoxUseIconsInWorkspace;
		
		wxCheckBox* m_checkBoxKeepIconsAutoUpdate;
		
		wxCheckBox* m_checkBoxUpdateAfterSave;
		wxCheckBox* m_checkBoxAutoAddNewFiles;
		wxCheckBox* m_checkBoxKeepTagsUpToDate;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl1;
		wxPanel* m_panel3;
		wxCheckBox* m_checkBoxUseExternalDiff;
		
		wxStaticText* m_staticText12;
		wxFilePickerCtrl* m_diffExe;
		wxStaticText* m_staticText13;
		wxTextCtrl* m_diffArgs;
		
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText8;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticText11;
		wxCheckBox* m_checkBoxCaptureDiffOutput;
		wxPanel* m_panel4;
		wxStaticText* m_staticText131;
		wxTextCtrl* m_textCtrlSshClientCmd;
		wxButton* m_button1;
		wxStaticText* m_staticText14;
		wxStaticLine* m_staticline2;
		wxButton* m_button2;
		wxButton* m_button3;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSvnUseIcons( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAutoAddNewFiles( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckUseExternalDiffViewer( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEnableExternalDiffViewerUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonBrowseSSHClient( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SvnOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Subversion Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnOptionsBaseDlg();
	
};

#endif //__svnoptionsbasedlg__
