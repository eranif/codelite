//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : subversion2_ui.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/wxscintilla.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/combobox.h>

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
		
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCtrlFrID;
		wxStaticText* m_staticTextBugID;
		wxTextCtrl* m_textCtrlBugID;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel1;
		wxStaticText* m_staticText2;
		wxCheckListBox* m_checkListFiles;
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrlMessage;
		wxStaticText* m_staticText27;
		wxChoice* m_choiceMessages;
		wxStaticLine* m_staticline1;
		wxButton* m_button2;
		wxButton* m_button3;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChoiceMessage( wxCommandEvent& event ) { event.Skip(); }
		
	
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
		wxScintilla* m_sci;
	
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
		wxCheckBox* m_checkBoxRenameFile;
		wxCheckBox* m_checkBoxUsePosixLocale;
		wxPanel* m_panel4;
		wxCheckBox* m_checkBoxUseExternalDiff;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrlDiffViewer;
		wxButton* m_buttonBrowseExtDiff;
		wxPanel* m_panel5;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrlSSHClient;
		wxButton* m_button12;
		wxStaticText* m_staticText21;
		wxTextCtrl* m_textCtrlSshClientArgs;
		wxStaticText* m_staticText22;
		wxPanel* m_panel6;
		wxCheckBox* m_checkBoxExposeRevisionMacro;
		wxStaticText* m_staticText29;
		wxTextCtrl* m_textCtrlMacroName;
		wxButton* m_button8;
		wxButton* m_button9;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnBrowseSvnExe( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUseExternalDiffUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnBrowseDiffViewer( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseSSHClient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAddRevisionMacroUI( wxUpdateUIEvent& event ) { event.Skip(); }
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

///////////////////////////////////////////////////////////////////////////////
/// Class SvnCheckoutDialogBase
///////////////////////////////////////////////////////////////////////////////
class SvnCheckoutDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText24;
		wxComboBox* m_comboBoxRepoURL;
		
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textCtrl20;
		wxButton* m_buttonBrowseDir;
		
		wxButton* m_button14;
		wxButton* m_button15;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCheckoutDirectoryText( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBrowseDirectory( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOkUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		SvnCheckoutDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Checkout"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnCheckoutDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnLogDialog
///////////////////////////////////////////////////////////////////////////////
class SvnLogDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText28;
		wxStaticText* m_staticText29;
		
		wxStaticLine* m_staticline5;
		wxButton* m_button17;
		wxButton* m_button18;
	
	public:
		wxTextCtrl* m_from;
		wxTextCtrl* m_to;
		wxCheckBox* m_compact;
		
		SvnLogDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Log"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SvnLogDialog();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DiffDialogBase
///////////////////////////////////////////////////////////////////////////////
class DiffDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText25;
		wxTextCtrl* m_textCtrlFromRev;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textCtrlToRev;
		wxStaticLine* m_staticline6;
		wxButton* m_button20;
		wxButton* m_button19;
	
	public:
		
		DiffDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Diff..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DiffDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ChangeLogPageBase
///////////////////////////////////////////////////////////////////////////////
class ChangeLogPageBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnURL( wxTextUrlEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChangeLogPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ChangeLogPageBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SvnPropsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class SvnPropsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticTextURL;
		wxStaticText* m_staticText27;
		wxTextCtrl* m_textCtrlBugURL;
		wxStaticText* m_staticText33;
		wxTextCtrl* m_textCtrlBugMsg;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_textCtrlFrURL;
		wxStaticText* m_staticText34;
		wxTextCtrl* m_textCtrlFrMsg;
		
		wxStaticLine* m_staticline7;
		wxButton* m_button21;
		wxButton* m_button22;
	
	public:
		
		SvnPropsBaseDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Svn Properties..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SvnPropsBaseDlg();
	
};

#endif //__subversion2_ui__
