///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
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
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SvnOptionsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnOptionsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_mainPanel;
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* m_filePicker;
		wxStaticLine* m_staticline;
		wxCheckBox* m_checkBoxUseIconsInWorkspace;
		
		wxCheckBox* m_checkBoxKeepIconsAutoUpdate;
		
		wxCheckBox* m_checkBoxUpdateAfterSave;
		wxCheckBox* m_checkBoxAutoAddNewFiles;
		wxCheckBox* m_checkBoxKeepTagsUpToDate;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl1;
		wxStaticLine* m_staticline7;
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
		wxStaticLine* m_staticline2;
		wxStdDialogButtonSizer* m_buttonSizer;
		wxButton* m_buttonSizerOK;
		wxButton* m_buttonSizerCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSvnUseIcons( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAutoAddNewFiles( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCheckUseExternalDiffViewer( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEnableExternalDiffViewerUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnButtonOk( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SvnOptionsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Subversion Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnOptionsBaseDlg();
	
};

#endif //__svnoptionsbasedlg__
