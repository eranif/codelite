///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 13 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __gitui__
#define __gitui__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checklst.h>
#include <wx/panel.h>
#include "gitCommitEditor.h"
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GitSettingsDlgBase
///////////////////////////////////////////////////////////////////////////////
class GitSettingsDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxColourPickerCtrl* m_colourTrackedFile;
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_colourDiffFile;
		wxStaticText* m_staticText4;
		wxFilePickerCtrl* m_pathGIT;
		wxStaticText* m_staticText5;
		wxFilePickerCtrl* m_pathGITK;
		
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
	
	public:
		
		GitSettingsDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Git settings..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~GitSettingsDlgBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GitCommitDlgBase
///////////////////////////////////////////////////////////////////////////////
class GitCommitDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel3;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText6;
		wxCheckListBox* m_listBox;
		wxPanel* m_panel2;
		wxStaticText* m_staticText7;
		GitCommitEditor* m_editor;
		wxPanel* m_panel4;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_commitMessage;
		wxStaticLine* m_staticline3;
		wxButton* m_button5;
		wxButton* m_button6;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeFile( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GitCommitDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Git commit..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 576,334 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~GitCommitDlgBase();
		
		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 0 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( GitCommitDlgBase::m_splitter2OnIdle ), NULL, this );
		}
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( GitCommitDlgBase::m_splitter1OnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GitCommitListDlgBase
///////////////////////////////////////////////////////////////////////////////
class GitCommitListDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_commitListBox;
		wxListBox* m_fileListBox;
		GitCommitEditor* m_editor;
		wxTextCtrl* m_commitMessage;
		wxStaticLine* m_staticline4;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeCommit( wxListEvent& event ) { event.Skip(); }
		virtual void OnChangeFile( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GitCommitListDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Commit List"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 879,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
		~GitCommitListDlgBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GitDiffDlgBase
///////////////////////////////////////////////////////////////////////////////
class GitDiffDlgBase : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_fileListBox;
		GitCommitEditor* m_editor;
		wxStaticLine* m_staticline4;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeFile( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GitDiffDlgBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Current Diffs"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 879,600 ), long style = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );
		~GitDiffDlgBase();
	
};

#endif //__gitui__
