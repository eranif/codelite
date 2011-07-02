///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __workspacesettingsbase__
#define __workspacesettingsbase__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class WorkspaceSettingsBase
///////////////////////////////////////////////////////////////////////////////
class WorkspaceSettingsBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_panelEnv;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxChoice* m_choiceEnvSets;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrlWspEnvVars;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnEnvSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		WorkspaceSettingsBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Workspace Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~WorkspaceSettingsBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CodeCompletionBasePage
///////////////////////////////////////////////////////////////////////////////
class CodeCompletionBasePage : public wxPanel 
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel8;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrlSearchPaths;
		wxPanel* m_panel6;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel9;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrlCmpOptions;
		wxPanel* m_panel10;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textCtrlMacros;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClangCCEnabledUI( wxUpdateUIEvent& event ) { event.Skip(); }
		
	
	public:
		
		CodeCompletionBasePage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~CodeCompletionBasePage();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CodeCompletionBasePage::m_splitter1OnIdle ), NULL, this );
		}
		
		void m_splitter3OnIdle( wxIdleEvent& )
		{
			m_splitter3->SetSashPosition( 0 );
			m_splitter3->Disconnect( wxEVT_IDLE, wxIdleEventHandler( CodeCompletionBasePage::m_splitter3OnIdle ), NULL, this );
		}
	
};

#endif //__workspacesettingsbase__
