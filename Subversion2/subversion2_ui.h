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

#endif //__subversion2_ui__
