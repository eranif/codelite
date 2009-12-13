///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __subversion2_ui__
#define __subversion2_ui__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/checklst.h>
#include <wx/splitter.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SubversionPageBase
///////////////////////////////////////////////////////////////////////////////
class SubversionPageBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrlRootDir;
		wxButton* m_buttonChangeRootDir;
		wxTreeCtrl* m_treeCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChangeRootDir( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeMenu( wxTreeEvent& event ) { event.Skip(); }
		
	
	public:
		
		SubversionPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~SubversionPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CommitDialogBase
///////////////////////////////////////////////////////////////////////////////
class CommitDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText2;
		wxCheckListBox* m_checkListFiles;
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlMessage;
		wxStaticLine* m_staticline1;
		wxButton* m_button2;
		wxButton* m_button3;
	
	public:
		
		CommitDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Commit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CommitDialogBase();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CommitDialogBase::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnCopyDialogBase
///////////////////////////////////////////////////////////////////////////////
class SvnCopyDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlSourceURL;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrlTargetURL;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlComment;
		wxStaticLine* m_staticline2;
		wxButton* m_button4;
		wxButton* m_button5;
	
	public:
		
		SvnCopyDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Create Svn Tag"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 575,315 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnCopyDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnShellBase
///////////////////////////////////////////////////////////////////////////////
class SvnShellBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrlOutput;
	
	public:
		
		SvnShellBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~SvnShellBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnLoginDialogBase
///////////////////////////////////////////////////////////////////////////////
class SvnLoginDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrlUsername;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textCtrlPassword;
		wxStaticLine* m_staticline4;
		wxButton* m_button6;
		wxButton* m_button7;
	
	public:
		
		SvnLoginDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Login"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SvnLoginDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class SvnPreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook;
		wxPanel* m_panel3;
		wxStaticText* m_staticTextExe;
		wxTextCtrl* m_textCtrlSvnExecutable;
		wxButton* m_buttonBrowse;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrlIgnorePattern;
		wxCheckBox* m_checkBoxAddToSvn;
		wxCheckBox* m_checkBoxRetag;
		wxPanel* m_panel4;
		wxCheckBox* m_checkBoxUseExternalDiff;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrlDiffViewer;
		wxButton* m_buttonBrowseExtDiff;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrlDiffViewerCommand;
		
		
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText17;
		
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText19;
		wxPanel* m_panel5;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlSSHClient;
		wxButton* m_button12;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_textCtrlSshClientArgs;
		wxStaticText* m_staticText22;
		wxButton* m_button8;
		wxButton* m_button9;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseSvnExe( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseExternalDiffUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBrowseDiffViewer( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseSSHClient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SvnPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Subversion Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnPreferencesDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnInfoDialog
///////////////////////////////////////////////////////////////////////////////
class SvnInfoDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText21;
		wxStaticText* m_staticText23;
		wxStaticText* m_staticText25;
		wxStaticText* m_staticText27;
		wxButton* m_button13;
	
	public:
		wxTextCtrl* m_textCtrlRootURL;
		wxTextCtrl* m_textCtrlURL;
		wxTextCtrl* m_textCtrlRevision;
		wxTextCtrl* m_textCtrlAuthor;
		wxTextCtrl* m_textCtrlDate;
		
		SvnInfoDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Info"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SvnInfoDialog();
	
};

#endif //__subversion2_ui__
